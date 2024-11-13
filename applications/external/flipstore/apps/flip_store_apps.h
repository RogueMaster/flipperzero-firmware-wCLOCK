#ifndef FLIP_STORE_APPS_H
#define FLIP_STORE_APPS_H

#include <flip_store.h>
#include <flip_storage/flip_store_storage.h>
#include <callback/flip_store_callback.h>

// Define maximum limits
#define MAX_APP_NAME_LENGTH        32
#define MAX_ID_LENGTH              32
#define MAX_APP_COUNT              100
#define MAX_APP_DESCRIPTION_LENGTH 100
#define MAX_APP_VERSION_LENGTH     5

typedef struct {
    char* app_name;
    char* app_id;
    char* app_build_id;
    char* app_version;
    char* app_description;
} FlipStoreAppInfo;

extern FlipStoreAppInfo* flip_catalog;

extern uint32_t app_selected_index;
extern bool flip_store_sent_request;
extern bool flip_store_success;
extern bool flip_store_saved_data;
extern bool flip_store_saved_success;
extern uint32_t flip_store_category_index;

enum ObjectState {
    OBJECT_EXPECT_KEY,
    OBJECT_EXPECT_COLON,
    OBJECT_EXPECT_VALUE,
    OBJECT_EXPECT_COMMA_OR_END
};

FlipStoreAppInfo* flip_catalog_alloc();

void flip_catalog_free();

// Utility function to parse JSON incrementally from a file
bool flip_store_process_app_list();

bool flip_store_get_fap_file(
    char* build_id,
    uint8_t target,
    uint16_t api_major,
    uint16_t api_minor);

void flip_store_request_error(Canvas* canvas);

// function to handle the entire installation process "asynchronously"
bool flip_store_install_app(Canvas* canvas, char* category);

// process the app list and return view
int32_t flip_store_handle_app_list(
    FlipStoreApp* app,
    int32_t success_view,
    char* category,
    Submenu** submenu);

#endif // FLIP_STORE_APPS_H
