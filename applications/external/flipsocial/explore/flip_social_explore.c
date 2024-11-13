#include "flip_social_explore.h"

FlipSocialModel* flip_social_explore_alloc() {
    // Allocate memory for each username only if not already allocated
    FlipSocialModel* explore = malloc(sizeof(FlipSocialModel));
    if(explore == NULL) {
        FURI_LOG_E(TAG, "Failed to allocate memory for explore model.");
        return NULL;
    }
    for(size_t i = 0; i < MAX_EXPLORE_USERS; i++) {
        if(explore->usernames[i] == NULL) {
            explore->usernames[i] = malloc(MAX_USER_LENGTH);
            if(explore->usernames[i] == NULL) {
                FURI_LOG_E(TAG, "Failed to allocate memory for username %zu", i);
                return NULL; // Return false on memory allocation failure
            }
        }
    }
    return explore;
}

void flip_social_free_explore() {
    if(!flip_social_explore) {
        FURI_LOG_E(TAG, "Explore model is NULL");
        return;
    }
    for(int i = 0; i < flip_social_explore->count; i++) {
        if(flip_social_explore->usernames[i]) {
            free(flip_social_explore->usernames[i]);
        }
    }
}

// for now we're just listing the current users
// as the feed is upgraded, then we can port more to the explore view
bool flip_social_get_explore() {
    snprintf(
        fhttp.file_path,
        sizeof(fhttp.file_path),
        STORAGE_EXT_PATH_PREFIX "/apps_data/flip_social/users.txt");

    fhttp.save_received_data = true;
    auth_headers_alloc();
    if(!flipper_http_get_request_with_headers(
           "https://www.flipsocial.net/api/user/users/", auth_headers)) {
        FURI_LOG_E(TAG, "Failed to send HTTP request for explore");
        fhttp.state = ISSUE;
        return false;
    }
    fhttp.state = RECEIVING;
    return true;
}

bool flip_social_parse_json_explore() {
    // load the received data from the saved file
    FuriString* user_data = flipper_http_load_from_file(fhttp.file_path);
    if(user_data == NULL) {
        FURI_LOG_E(TAG, "Failed to load received data from file.");
        return false;
    }
    char* data_cstr = (char*)furi_string_get_cstr(user_data);
    if(data_cstr == NULL) {
        FURI_LOG_E(TAG, "Failed to get C-string from FuriString.");
        furi_string_free(user_data);
        return false;
    }

    // Allocate memory for each username only if not already allocated
    flip_social_explore = flip_social_explore_alloc();
    if(flip_social_explore == NULL) {
        FURI_LOG_E(TAG, "Failed to allocate memory for explore usernames.");
        furi_string_free(user_data);
        free(data_cstr);
        return false;
    }

    // Remove newlines
    char* pos = data_cstr;
    while((pos = strchr(pos, '\n')) != NULL) {
        *pos = ' ';
    }

    // Initialize explore count
    flip_social_explore->count = 0;

    // Extract the users array from the JSON
    char* json_users = get_json_value("users", data_cstr, MAX_TOKENS);
    if(json_users == NULL) {
        FURI_LOG_E(TAG, "Failed to parse users array.");
        furi_string_free(user_data);
        free(data_cstr);
        return false;
    }

    // Manual tokenization for comma-separated values
    char* start = json_users + 1; // Skip the opening bracket
    char* end;
    while((end = strchr(start, ',')) != NULL && flip_social_explore->count < MAX_EXPLORE_USERS) {
        *end = '\0'; // Null-terminate the current token

        // Remove quotes
        if(*start == '"') start++;
        if(*(end - 1) == '"') *(end - 1) = '\0';

        // Copy username to pre-allocated memory
        snprintf(
            flip_social_explore->usernames[flip_social_explore->count],
            MAX_USER_LENGTH,
            "%s",
            start);
        flip_social_explore->count++;
        start = end + 1;
    }

    // Handle the last token
    if(*start != '\0' && flip_social_explore->count < MAX_EXPLORE_USERS) {
        if(*start == '"') start++;
        if(*(start + strlen(start) - 1) == ']') *(start + strlen(start) - 1) = '\0';
        if(*(start + strlen(start) - 1) == '"') *(start + strlen(start) - 1) = '\0';

        snprintf(
            flip_social_explore->usernames[flip_social_explore->count],
            MAX_USER_LENGTH,
            "%s",
            start);
        flip_social_explore->count++;
    }

    // Add submenu items for the users
    submenu_reset(app_instance->submenu_explore);
    submenu_set_header(app_instance->submenu_explore, "Explore");
    for(int i = 0; i < flip_social_explore->count; i++) {
        submenu_add_item(
            app_instance->submenu_explore,
            flip_social_explore->usernames[i],
            FlipSocialSubmenuExploreIndexStartIndex + i,
            flip_social_callback_submenu_choices,
            app_instance);
    }

    // Free the json_users
    free(json_users);
    free(start);
    free(end);
    furi_string_free(user_data);
    free(data_cstr);
    return flip_social_explore->count > 0;
}
