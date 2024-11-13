#ifndef FLIP_STORE_STORAGE_H
#define FLIP_STORE_STORAGE_H

#include <furi.h>
#include <storage/storage.h>
#include <flip_store.h>

#define SETTINGS_PATH    STORAGE_EXT_PATH_PREFIX "/apps_data/flip_store/settings.bin"
#define BUFFER_SIZE      64
#define MAX_KEY_LENGTH   16
#define MAX_VALUE_LENGTH 100

void save_settings(const char* ssid, const char* password);

bool load_settings(char* ssid, size_t ssid_size, char* password, size_t password_size);

bool delete_app(const char* app_id, const char* app_category);

bool app_exists(const char* app_id, const char* app_category);

// Function to parse JSON incrementally from a file
bool parse_json_incrementally(
    const char* file_path,
    const char* target_key,
    char* value_buffer,
    size_t value_buffer_size);

#endif
