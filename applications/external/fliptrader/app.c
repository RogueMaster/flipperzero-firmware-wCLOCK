#include <flip_trader.h>
#include <alloc/flip_trader_alloc.h>

// Entry point for the FlipTrader application
int32_t flip_trader_app(void* p) {
    // Suppress unused parameter warning
    UNUSED(p);

    // Initialize the FlipTrader application
    FlipTraderApp* app = flip_trader_app_alloc();
    if(!app) {
        FURI_LOG_E(TAG, "Failed to allocate FlipTraderApp");
        return -1;
    }

    if(!flipper_http_ping()) {
        FURI_LOG_E(TAG, "Failed to ping the device");
        return -1;
    }

    // Run the view dispatcher
    view_dispatcher_run(app->view_dispatcher);

    // Free the resources used by the FlipTrader application
    flip_trader_app_free(app);

    // Return 0 to indicate success
    return 0;
}
