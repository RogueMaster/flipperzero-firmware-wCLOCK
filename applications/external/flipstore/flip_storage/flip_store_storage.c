#include "flip_storage/flip_store_storage.h"

void save_settings(const char* ssid, const char* password) {
    // Create the directory for saving settings
    char directory_path[128];
    snprintf(
        directory_path, sizeof(directory_path), STORAGE_EXT_PATH_PREFIX "/apps_data/flip_store");

    // Create the directory
    Storage* storage = furi_record_open(RECORD_STORAGE);
    storage_common_mkdir(storage, directory_path);

    // Open the settings file
    File* file = storage_file_alloc(storage);
    if(!storage_file_open(file, SETTINGS_PATH, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        FURI_LOG_E(TAG, "Failed to open settings file for writing: %s", SETTINGS_PATH);
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);
        return;
    }

    // Save the ssid length and data
    size_t ssid_length = strlen(ssid) + 1; // Include null terminator
    if(storage_file_write(file, &ssid_length, sizeof(size_t)) != sizeof(size_t) ||
       storage_file_write(file, ssid, ssid_length) != ssid_length) {
        FURI_LOG_E(TAG, "Failed to write SSID");
    }

    // Save the password length and data
    size_t password_length = strlen(password) + 1; // Include null terminator
    if(storage_file_write(file, &password_length, sizeof(size_t)) != sizeof(size_t) ||
       storage_file_write(file, password, password_length) != password_length) {
        FURI_LOG_E(TAG, "Failed to write password");
    }

    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
}

bool load_settings(char* ssid, size_t ssid_size, char* password, size_t password_size) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);

    if(!storage_file_open(file, SETTINGS_PATH, FSAM_READ, FSOM_OPEN_EXISTING)) {
        FURI_LOG_E(TAG, "Failed to open settings file for reading: %s", SETTINGS_PATH);
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);
        return false; // Return false if the file does not exist
    }

    // Load the ssid
    size_t ssid_length;
    if(storage_file_read(file, &ssid_length, sizeof(size_t)) != sizeof(size_t) ||
       ssid_length > ssid_size || storage_file_read(file, ssid, ssid_length) != ssid_length) {
        FURI_LOG_E(TAG, "Failed to read SSID");
        storage_file_close(file);
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);
        return false;
    }
    ssid[ssid_length - 1] = '\0'; // Ensure null-termination

    // Load the password
    size_t password_length;
    if(storage_file_read(file, &password_length, sizeof(size_t)) != sizeof(size_t) ||
       password_length > password_size ||
       storage_file_read(file, password, password_length) != password_length) {
        FURI_LOG_E(TAG, "Failed to read password");
        storage_file_close(file);
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);
        return false;
    }
    password[password_length - 1] = '\0'; // Ensure null-termination

    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);

    return true;
}

// future implenetation because we need the app category
bool delete_app(const char* app_id, const char* app_category) {
    // Create the directory for saving settings
    char directory_path[128];
    snprintf(
        directory_path,
        sizeof(directory_path),
        STORAGE_EXT_PATH_PREFIX "/apps/%s/%s",
        app_category,
        app_id);

    // Create the directory
    Storage* storage = furi_record_open(RECORD_STORAGE);
    if(!storage_simply_remove_recursive(storage, directory_path)) {
        FURI_LOG_E(TAG, "Failed to delete app: %s", app_id);
        furi_record_close(RECORD_STORAGE);
        return false;
    }

    furi_record_close(RECORD_STORAGE);
    return true;
}

bool app_exists(const char* app_id, const char* app_category) {
    // Check if the app exists
    char directory_path[128];
    snprintf(
        directory_path,
        sizeof(directory_path),
        STORAGE_EXT_PATH_PREFIX "/apps/%s/%s.fap",
        app_category,
        app_id);

    Storage* storage = furi_record_open(RECORD_STORAGE);
    bool exists = storage_common_exists(storage, directory_path);
    furi_record_close(RECORD_STORAGE);

    return exists;
}

// Function to parse JSON incrementally from a file
bool parse_json_incrementally(
    const char* file_path,
    const char* target_key,
    char* value_buffer,
    size_t value_buffer_size) {
    Storage* _storage = NULL;
    File* _file = NULL;
    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    bool key_found = false;
    bool in_string = false;
    bool is_escaped = false;
    bool reading_key = false;
    bool reading_value = false;
    char current_key[MAX_KEY_LENGTH] = {0};
    size_t key_index = 0;
    size_t value_index = 0;

    // Open storage and file
    _storage = furi_record_open(RECORD_STORAGE);
    if(!_storage) {
        FURI_LOG_E("JSON_PARSE", "Failed to open storage.");
        return false;
    }

    _file = storage_file_alloc(_storage);
    if(!_file) {
        FURI_LOG_E("JSON_PARSE", "Failed to allocate file.");
        furi_record_close(RECORD_STORAGE);
        return false;
    }

    if(!storage_file_open(_file, file_path, FSAM_READ, FSOM_OPEN_EXISTING)) {
        FURI_LOG_E("JSON_PARSE", "Failed to open JSON file for reading.");
        goto cleanup;
    }

    while((bytes_read = storage_file_read(_file, buffer, BUFFER_SIZE)) > 0) {
        for(size_t i = 0; i < bytes_read; ++i) {
            char c = buffer[i];

            if(is_escaped) {
                is_escaped = false;
                if(reading_key) {
                    if(key_index < MAX_KEY_LENGTH - 1) {
                        current_key[key_index++] = c;
                    }
                } else if(reading_value) {
                    if(value_index < value_buffer_size - 1) {
                        value_buffer[value_index++] = c;
                    }
                }
                continue;
            }

            if(c == '\\') {
                is_escaped = true;
                continue;
            }

            if(c == '\"') {
                in_string = !in_string;

                if(in_string) {
                    // Start of a string
                    if(!reading_key && !reading_value) {
                        // Possible start of a key
                        reading_key = true;
                        key_index = 0;
                        current_key[0] = '\0';
                    }
                } else {
                    // End of a string
                    if(reading_key) {
                        reading_key = false;
                        current_key[key_index] = '\0';

                        if(strcmp(current_key, target_key) == 0) {
                            key_found = true;
                        }
                    } else if(reading_value) {
                        reading_value = false;
                        value_buffer[value_index] = '\0';

                        if(key_found) {
                            // Found the target value
                            goto success;
                        }
                    }
                }
                continue;
            }

            if(in_string) {
                if(reading_key) {
                    if(key_index < MAX_KEY_LENGTH - 1) {
                        current_key[key_index++] = c;
                    }
                } else if(reading_value) {
                    if(value_index < value_buffer_size - 1) {
                        value_buffer[value_index++] = c;
                    }
                }
                continue;
            }

            if(c == ':' && key_found && !reading_value) {
                // After colon, start reading the value
                // Skip whitespace and possible opening quote
                while(i + 1 < bytes_read &&
                      (buffer[i + 1] == ' ' || buffer[i + 1] == '\n' || buffer[i + 1] == '\r')) {
                    i++;
                }

                if(i + 1 < bytes_read && buffer[i + 1] == '\"') {
                    i++; // Move to the quote
                    in_string = true;
                    reading_value = true;
                    value_index = 0;
                } else {
                    // Handle non-string values (e.g., numbers, booleans)
                    reading_value = true;
                    value_index = 0;
                }
                continue;
            }

            if(reading_value && (c == ',' || c == '}' || c == ']')) {
                // End of the value
                reading_value = false;
                value_buffer[value_index] = '\0';

                if(key_found) {
                    // Found the target value
                    goto success;
                }
                key_found = false;
            }
        }
    }

success:
    storage_file_close(_file);
    storage_file_free(_file);
    furi_record_close(RECORD_STORAGE);
    return key_found;

cleanup:
    if(_file) {
        storage_file_free(_file);
    }
    if(_storage) {
        furi_record_close(RECORD_STORAGE);
    }
    return false;
}
