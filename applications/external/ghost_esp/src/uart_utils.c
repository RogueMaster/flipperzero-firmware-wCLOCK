#include "uart_utils.h"
#include <furi.h>
#include <stdlib.h>
#include <string.h>
#include "sequential_file.h"
#include <gui/modules/text_box.h>
#include <furi_hal_serial.h>
#include <cfw/cfw.h>

#define WORKER_ALL_RX_EVENTS   (WorkerEvtStop | WorkerEvtRxDone | WorkerEvtPcapDone)
#define PCAP_WRITE_CHUNK_SIZE  1024
#define AP_LIST_TIMEOUT_MS     5000
#define INITIAL_BUFFER_SIZE    2048
#define BUFFER_GROWTH_FACTOR   1.5
#define MAX_BUFFER_SIZE        (8 * 1024) // 8KB max
#define MUTEX_TIMEOUT_MS       2500
#define BUFFER_CLEAR_SIZE      128
#define BUFFER_RESIZE_CHUNK    1024
#define TEXT_SCROLL_GUARD_SIZE 64

typedef enum {
    MARKER_STATE_IDLE,
    MARKER_STATE_BEGIN,
    MARKER_STATE_CLOSE
} MarkerState;

static TextBufferManager* text_buffer_alloc(void) {
    TextBufferManager* manager = malloc(sizeof(TextBufferManager));
    if(!manager) return NULL;

    manager->ring_buffer = malloc(RING_BUFFER_SIZE);
    manager->view_buffer = malloc(VIEW_BUFFER_SIZE);
    manager->mutex = furi_mutex_alloc(FuriMutexTypeNormal);

    if(!manager->ring_buffer || !manager->view_buffer || !manager->mutex) {
        free(manager->ring_buffer);
        free(manager->view_buffer);
        free(manager->mutex);
        free(manager);
        return NULL;
    }

    manager->ring_read_index = 0;
    manager->ring_write_index = 0;
    manager->view_buffer_len = 0;
    manager->buffer_full = false;

    memset(manager->ring_buffer, 0, RING_BUFFER_SIZE);
    memset(manager->view_buffer, 0, VIEW_BUFFER_SIZE);

    return manager;
}
static void text_buffer_free(TextBufferManager* manager) {
    if(!manager) return;
    if(manager->mutex) furi_mutex_free(manager->mutex);
    free(manager->ring_buffer);
    free(manager->view_buffer);
    free(manager);
}

static void text_buffer_add(TextBufferManager* manager, const char* data, size_t len) {
    if(!manager || !data || !len) return;

    furi_mutex_acquire(manager->mutex, FuriWaitForever);

    for(size_t i = 0; i < len; i++) {
        manager->ring_buffer[manager->ring_write_index] = data[i];
        manager->ring_write_index = (manager->ring_write_index + 1) % RING_BUFFER_SIZE;

        if(manager->ring_write_index == manager->ring_read_index) {
            manager->ring_read_index = (manager->ring_read_index + 1) % RING_BUFFER_SIZE;
            manager->buffer_full = true;
        }
    }

    furi_mutex_release(manager->mutex);
}
static void text_buffer_update_view(TextBufferManager* manager, bool view_from_start) {
    if(!manager) return;

    furi_mutex_acquire(manager->mutex, FuriWaitForever);

    // Calculate available data
    size_t available;
    if(manager->buffer_full) {
        available = RING_BUFFER_SIZE;
    } else if(manager->ring_write_index >= manager->ring_read_index) {
        available = manager->ring_write_index - manager->ring_read_index;
    } else {
        available = RING_BUFFER_SIZE - manager->ring_read_index + manager->ring_write_index;
    }

    // Limit to view buffer size
    size_t copy_size = (available > VIEW_BUFFER_SIZE - 1) ? VIEW_BUFFER_SIZE - 1 : available;

    // Choose starting point based on view preference
    size_t start;
    if(view_from_start) {
        // Start from oldest data
        start = manager->ring_read_index;
    } else {
        // Start from newest data that will fit in view
        size_t data_to_skip = available > copy_size ? available - copy_size : 0;
        start = (manager->ring_read_index + data_to_skip) % RING_BUFFER_SIZE;
    }

    // Copy data to view buffer
    size_t j = 0;
    for(size_t i = 0; i < copy_size; i++) {
        size_t idx = (start + i) % RING_BUFFER_SIZE;
        manager->view_buffer[j++] = manager->ring_buffer[idx];
    }

    manager->view_buffer[j] = '\0';
    manager->view_buffer_len = j;

    furi_mutex_release(manager->mutex);
}
static void
    uart_rx_callback(FuriHalSerialHandle* handle, FuriHalSerialRxEvent event, void* context) {
    UartContext* uart = (UartContext*)context;
    const char* mark_begin = "[BUF/BEGIN]";
    const char* mark_close = "[BUF/CLOSE]";
    size_t mark_len = 11;

    if(!uart || !uart->text_manager || event != FuriHalSerialRxEventData) {
        return;
    }

    uint8_t data = furi_hal_serial_async_rx(handle);

    // Check if we're collecting a marker
    if(uart->mark_test_idx > 0) {
        // Prevent buffer overflow
        if(uart->mark_test_idx >= sizeof(uart->mark_test_buf)) {
            uart->mark_test_idx = 0;
            return;
        }

        if(uart->mark_test_idx < mark_len &&
           (data == mark_begin[uart->mark_test_idx] || data == mark_close[uart->mark_test_idx])) {
            uart->mark_test_buf[uart->mark_test_idx++] = data;

            if(uart->mark_test_idx == mark_len) {
                furi_mutex_acquire(uart->text_manager->mutex, FuriWaitForever);

                if(!memcmp(uart->mark_test_buf, mark_begin, mark_len)) {
                    uart->pcap = true;
                    FURI_LOG_I("UART", "Capture started");
                } else if(!memcmp(uart->mark_test_buf, mark_close, mark_len)) {
                    uart->pcap = false;
                    FURI_LOG_I("UART", "Capture ended");
                }

                furi_mutex_release(uart->text_manager->mutex);
                uart->mark_test_idx = 0;
                return;
            }
            // Don't process marker bytes
            return;
        } else {
            // Mismatch occurred, handle buffered bytes atomically
            furi_mutex_acquire(uart->text_manager->mutex, FuriWaitForever);

            // Ensure valid thread and stream before sending
            if(uart->rx_thread && (uart->pcap ? uart->pcap_stream : uart->rx_stream)) {
                if(uart->pcap) {
                    if(furi_stream_buffer_send(
                           uart->pcap_stream, uart->mark_test_buf, uart->mark_test_idx, 0) ==
                       uart->mark_test_idx) {
                        furi_thread_flags_set(
                            furi_thread_get_id(uart->rx_thread), WorkerEvtPcapDone);
                    }
                } else {
                    if(furi_stream_buffer_send(
                           uart->rx_stream, uart->mark_test_buf, uart->mark_test_idx, 0) ==
                       uart->mark_test_idx) {
                        furi_thread_flags_set(
                            furi_thread_get_id(uart->rx_thread), WorkerEvtRxDone);
                    }
                }
            }

            furi_mutex_release(uart->text_manager->mutex);
            uart->mark_test_idx = 0;
        }
    }

    // Start of a potential marker
    if(data == mark_begin[0] || data == mark_close[0]) {
        uart->mark_test_buf[0] = data;
        uart->mark_test_idx = 1;
        return;
    }

    // Handle regular data atomically
    furi_mutex_acquire(uart->text_manager->mutex, FuriWaitForever);

    bool current_pcap = uart->pcap;
    bool success = false;

    // Ensure valid thread and stream before sending
    if(uart->rx_thread && (current_pcap ? uart->pcap_stream : uart->rx_stream)) {
        if(current_pcap) {
            if(furi_stream_buffer_send(uart->pcap_stream, &data, 1, 0) == 1) {
                furi_thread_flags_set(furi_thread_get_id(uart->rx_thread), WorkerEvtPcapDone);
                success = true;
                FURI_LOG_D("UART", "Captured data byte (pcap=true): 0x%02X", data);
            }
        } else {
            if(furi_stream_buffer_send(uart->rx_stream, &data, 1, 0) == 1) {
                furi_thread_flags_set(furi_thread_get_id(uart->rx_thread), WorkerEvtRxDone);
                success = true;
                FURI_LOG_D("UART", "Logged data byte (pcap=false): 0x%02X", data);
            }
        }
    }

    if(!success) {
        FURI_LOG_W("UART", "Failed to send data byte: 0x%02X", data);
    }

    furi_mutex_release(uart->text_manager->mutex);
}

void handle_uart_rx_data(uint8_t* buf, size_t len, void* context) {
    AppState* state = (AppState*)context;
    if(!state || !state->uart_context || !state->uart_context->is_serial_active || !buf ||
       len == 0) {
        FURI_LOG_W("UART", "Invalid parameters in handle_uart_rx_data");
        return;
    }

    // Only log data if NOT in PCAP mode
    if(!state->uart_context->pcap && state->uart_context->storageContext &&
       state->uart_context->storageContext->log_file &&
       state->uart_context->storageContext->HasOpenedFile) {
        static size_t bytes_since_sync = 0;

        size_t written =
            storage_file_write(state->uart_context->storageContext->log_file, buf, len);

        if(written != len) {
            FURI_LOG_E("UART", "Failed to write log data: expected %zu, wrote %zu", len, written);
        } else {
            bytes_since_sync += written;
            if(bytes_since_sync >= 8192) {
                storage_file_sync(state->uart_context->storageContext->log_file);
                bytes_since_sync = 0;
                FURI_LOG_D("UART", "Synced log file to storage");
            }
        }
    }

    // Update text display
    text_buffer_add(state->uart_context->text_manager, (char*)buf, len);
    text_buffer_update_view(
        state->uart_context->text_manager, state->settings.view_logs_from_start_index);

    bool view_from_start = state->settings.view_logs_from_start_index;
    text_box_set_text(state->text_box, state->uart_context->text_manager->view_buffer);
    text_box_set_focus(state->text_box, view_from_start ? TextBoxFocusStart : TextBoxFocusEnd);
}

static int32_t uart_worker(void* context) {
    UartContext* uart = (UartContext*)context;

    FURI_LOG_I("Worker", "UART worker thread started");

    while(1) {
        uint32_t events =
            furi_thread_flags_wait(WORKER_ALL_RX_EVENTS, FuriFlagWaitAny, FuriWaitForever);

        FURI_LOG_D("Worker", "Received events: 0x%08lX", (unsigned long)events);

        if(events & WorkerEvtStop) {
            FURI_LOG_I("Worker", "Stopping worker thread");
            break;
        }

        if(events & WorkerEvtRxDone) {
            size_t len = furi_stream_buffer_receive(uart->rx_stream, uart->rx_buf, RX_BUF_SIZE, 0);

            FURI_LOG_D("Worker", "Processing rx_stream data: %zu bytes", len);

            if(len > 0 && uart->handle_rx_data_cb) {
                FURI_LOG_D("Worker", "Invoking handle_rx_data_cb with %zu bytes", len);
                uart->handle_rx_data_cb(uart->rx_buf, len, uart->state);
                FURI_LOG_D("Worker", "rx_stream callback invoked successfully");
            } else {
                if(len == 0) {
                    FURI_LOG_W("Worker", "Received zero bytes from rx_stream");
                }
                if(!uart->handle_rx_data_cb) {
                    FURI_LOG_E("Worker", "handle_rx_data_cb is NULL");
                }
            }
        }

        if(events & WorkerEvtPcapDone) {
            size_t len =
                furi_stream_buffer_receive(uart->pcap_stream, uart->rx_buf, RX_BUF_SIZE, 0);

            FURI_LOG_D("Worker", "Processing pcap_stream data: %zu bytes", len);

            if(len > 0 && uart->handle_rx_pcap_cb) {
                FURI_LOG_D("Worker", "Invoking handle_rx_pcap_cb with %zu bytes", len);
                uart->handle_rx_pcap_cb(uart->rx_buf, len, uart); // Corrected context
                FURI_LOG_D("Worker", "pcap_stream callback invoked successfully");
            } else {
                if(len == 0) {
                    FURI_LOG_W("Worker", "Received zero bytes from pcap_stream");
                }
                if(!uart->handle_rx_pcap_cb) {
                    FURI_LOG_E("Worker", "handle_rx_pcap_cb is NULL");
                }
            }
        }
    }

    // Clean up streams with detailed logging
    FURI_LOG_I("Worker", "Cleaning up rx_stream and pcap_stream buffers");
    furi_stream_buffer_free(uart->rx_stream);
    furi_stream_buffer_free(uart->pcap_stream);

    FURI_LOG_I("Worker", "Worker thread exited");
    return 0;
}

void update_text_box_view(AppState* state) {
    if(!state || !state->text_box || !state->uart_context || !state->uart_context->text_manager)
        return;

    text_buffer_update_view(
        state->uart_context->text_manager, state->settings.view_logs_from_start_index);

    bool view_from_start = state->settings.view_logs_from_start_index;
    text_box_set_text(state->text_box, state->uart_context->text_manager->view_buffer);
    text_box_set_focus(state->text_box, view_from_start ? TextBoxFocusStart : TextBoxFocusEnd);
}
UartContext* uart_init(AppState* state) {
    uint32_t start_time = furi_get_tick();
    FURI_LOG_I("UART", "Starting UART initialization");

    UartContext* uart = malloc(sizeof(UartContext));
    if(!uart) {
        FURI_LOG_E("UART", "Failed to allocate UART context");
        return NULL;
    }
    memset(uart, 0, sizeof(UartContext));

    uart->state = state;
    uart->is_serial_active = false;
    uart->pcap = false;
    uart->mark_test_idx = 0;
    uart->pcap_buf_len = 0;

    // Initialize rx/pcap streams
    uart->rx_stream = furi_stream_buffer_alloc(RX_BUF_SIZE, 1);
    uart->pcap_stream = furi_stream_buffer_alloc(PCAP_BUF_SIZE, 1);

    if(!uart->rx_stream || !uart->pcap_stream) {
        FURI_LOG_E("UART", "Failed to allocate stream buffers");
        uart_free(uart);
        return NULL;
    }

    // Set callbacks
    uart->handle_rx_data_cb = handle_uart_rx_data;
    uart->handle_rx_pcap_cb = uart_storage_rx_callback;

    // Initialize thread
    uart->rx_thread = furi_thread_alloc();
    if(uart->rx_thread) {
        furi_thread_set_name(uart->rx_thread, "UART_Receive");
        furi_thread_set_stack_size(uart->rx_thread, 2048);
        furi_thread_set_context(uart->rx_thread, uart);
        furi_thread_set_callback(uart->rx_thread, uart_worker);
        furi_thread_start(uart->rx_thread);
    } else {
        FURI_LOG_E("UART", "Failed to allocate rx thread");
        uart_free(uart);
        return NULL;
    }

    // Initialize storage
    uart->storageContext = uart_storage_init(uart);
    if(!uart->storageContext) {
        FURI_LOG_E("UART", "Failed to initialize storage");
        uart_free(uart);
        return NULL;
    }

    // Initialize serial
    uart->serial_handle = furi_hal_serial_control_acquire(cfw_settings.uart_esp_channel);
    if(uart->serial_handle) {
        furi_hal_serial_init(uart->serial_handle, 115200);
        uart->is_serial_active = true;
        furi_hal_serial_async_rx_start(uart->serial_handle, uart_rx_callback, uart, false);
    } else {
        FURI_LOG_E("UART", "Failed to acquire serial handle");
        uart_free(uart);
        return NULL;
    }

    // Initialize text manager
    uart->text_manager = text_buffer_alloc();
    if(!uart->text_manager) {
        FURI_LOG_E("UART", "Failed to allocate text manager");
        uart_free(uart);
        return NULL;
    }

    uint32_t duration = furi_get_tick() - start_time;
    FURI_LOG_I("UART", "UART initialization complete (Time taken: %lu ms)", duration);

    return uart;
}

void uart_free(UartContext* uart) {
    if(!uart) return;

    if(uart->text_manager) {
        text_buffer_free(uart->text_manager);
        uart->text_manager = NULL;
    }

    // Stop thread and wait for it to finish
    if(uart->rx_thread) {
        furi_thread_flags_set(furi_thread_get_id(uart->rx_thread), WorkerEvtStop);
        furi_thread_join(uart->rx_thread);
        furi_thread_free(uart->rx_thread);
    }

    // Clean up serial after thread is stopped
    if(uart->serial_handle) {
        furi_hal_serial_deinit(uart->serial_handle);
        furi_hal_serial_control_release(uart->serial_handle);
    }

    // Free streams after everything is stopped
    if(uart->rx_stream) furi_stream_buffer_free(uart->rx_stream);
    if(uart->pcap_stream) furi_stream_buffer_free(uart->pcap_stream);

    // Clean up storage context last
    if(uart->storageContext) uart_storage_free(uart->storageContext);

    free(uart);
}

// Stop the UART thread (typically when exiting)
void uart_stop_thread(UartContext* uart) {
    if(uart && uart->rx_thread) {
        furi_thread_flags_set(furi_thread_get_id(uart->rx_thread), WorkerEvtStop);
    }
}

// Send data over UART
void uart_send(UartContext* uart, const uint8_t* data, size_t len) {
    // Only try to send if serial is active
    if(uart && uart->serial_handle && uart->is_serial_active) {
        furi_hal_serial_tx(uart->serial_handle, data, len);
    }
}

bool uart_is_esp_connected(UartContext* uart) {
    FURI_LOG_D("UART", "Checking ESP connection...");

    if(!uart || !uart->serial_handle || !uart->text_manager) {
        FURI_LOG_E("UART", "Invalid UART context");
        return false;
    }

    // Temporarily disable callbacks
    furi_hal_serial_async_rx_stop(uart->serial_handle);

    // Clear and reset buffers atomically
    furi_mutex_acquire(uart->text_manager->mutex, FuriWaitForever);
    memset(uart->text_manager->ring_buffer, 0, RING_BUFFER_SIZE);
    memset(uart->text_manager->view_buffer, 0, VIEW_BUFFER_SIZE);
    uart->text_manager->ring_read_index = 0;
    uart->text_manager->ring_write_index = 0;
    uart->text_manager->buffer_full = false;
    uart->text_manager->view_buffer_len = 0;
    furi_mutex_release(uart->text_manager->mutex);

    // Re-enable callbacks with clean state
    furi_hal_serial_async_rx_start(uart->serial_handle, uart_rx_callback, uart, false);

    // Flush any pending data
    furi_hal_serial_tx(uart->serial_handle, (uint8_t*)"\r\n", 2);
    furi_delay_ms(50); // Allow time for flush

    const char* test_cmd = "stop\n";
    bool connected = false;
    const uint32_t CMD_TIMEOUT_MS = 500;

    // Send test command
    uart_send(uart, (uint8_t*)test_cmd, strlen(test_cmd));
    FURI_LOG_D("UART", "Sent command: %s", test_cmd);

    uint32_t start_time = furi_get_tick();
    while(furi_get_tick() - start_time < CMD_TIMEOUT_MS) {
        furi_mutex_acquire(uart->text_manager->mutex, FuriWaitForever);

        // Check if we received any data
        size_t available;
        if(uart->text_manager->buffer_full) {
            available = RING_BUFFER_SIZE;
        } else if(uart->text_manager->ring_write_index >= uart->text_manager->ring_read_index) {
            available = uart->text_manager->ring_write_index - uart->text_manager->ring_read_index;
        } else {
            available = RING_BUFFER_SIZE - uart->text_manager->ring_read_index +
                        uart->text_manager->ring_write_index;
        }

        if(available > 0) {
            connected = true;
            FURI_LOG_D("UART", "Received %d bytes response", available);
        }

        furi_mutex_release(uart->text_manager->mutex);

        if(connected) break;
        furi_delay_ms(10);
    }

    FURI_LOG_I("UART", "ESP connection check: %s", connected ? "Success" : "Failed");
    return connected;
}

bool uart_receive_data(
    UartContext* uart,
    ViewDispatcher* view_dispatcher,
    AppState* state,
    const char* prefix,
    const char* extension,
    const char* TargetFolder) {
    if(!uart || !uart->storageContext || !view_dispatcher || !state) {
        FURI_LOG_E("UART", "Invalid parameters to uart_receive_data");
        return false;
    }

    // Close any existing file
    if(uart->storageContext->HasOpenedFile) {
        storage_file_sync(uart->storageContext->current_file);
        storage_file_close(uart->storageContext->current_file);
        uart->storageContext->HasOpenedFile = false;
    }

    uart->pcap = false; // Reset capture state
    furi_stream_buffer_reset(uart->pcap_stream);

    // Clear display
    text_box_set_text(state->text_box, "");
    text_box_set_focus(state->text_box, TextBoxFocusEnd);

    // Open new file if needed
    if(prefix && extension && TargetFolder && strlen(prefix) > 1) {
        uart->storageContext->HasOpenedFile = sequential_file_open(
            uart->storageContext->storage_api,
            uart->storageContext->current_file,
            TargetFolder,
            prefix,
            extension);

        if(!uart->storageContext->HasOpenedFile) {
            FURI_LOG_E(
                "UART",
                "Failed to open file: Prefix=%s, Ext=%s, Folder=%s",
                prefix,
                extension,
                TargetFolder);
            return false;
        }
    }

    view_dispatcher_switch_to_view(view_dispatcher, 5);
    state->current_view = 5;

    return true; // Indicate success
}
