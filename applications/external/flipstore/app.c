#include <flip_store.h>
#include <alloc/flip_store_alloc.h>
#include <apps/flip_store_apps.h>

// Entry point for the Hello World application
int32_t main_flip_store(void* p) {
    // Suppress unused parameter warning
    UNUSED(p);

    // Initialize the Hello World application
    FlipStoreApp* app = flip_store_app_alloc();
    if(!app) {
        FURI_LOG_E(TAG, "Failed to allocate FlipStoreApp");
        return -1;
    }

    if(!flipper_http_ping()) {
        FURI_LOG_E(TAG, "Failed to ping the device");
        return -1;
    }

    // Run the view dispatcher
    view_dispatcher_run(app->view_dispatcher);

    // Free the resources used by the Hello World application
    flip_store_app_free(app);
    flip_catalog_free();

    // Return 0 to indicate success
    return 0;
}
