#include "flip_social_friends.h"

FlipSocialModel* flip_social_friends_alloc() {
    // Allocate memory for each username only if not already allocated
    FlipSocialModel* friends = malloc(sizeof(FlipSocialModel));
    for(size_t i = 0; i < MAX_FRIENDS; i++) {
        if(friends->usernames[i] == NULL) {
            friends->usernames[i] = malloc(MAX_USER_LENGTH);
            if(friends->usernames[i] == NULL) {
                FURI_LOG_E(TAG, "Failed to allocate memory for username %zu", i);
                return NULL; // Return false on memory allocation failure
            }
        }
    }
    return friends;
}

void flip_social_free_friends() {
    if(!flip_social_friends) {
        FURI_LOG_E(TAG, "Friends model is NULL");
        return;
    }
    for(int i = 0; i < flip_social_friends->count; i++) {
        if(flip_social_friends->usernames[i]) {
            free(flip_social_friends->usernames[i]);
        }
    }
}

// for now we're just listing the current users
// as the feed is upgraded, then we can port more to the friends view
bool flip_social_get_friends() {
    // will return true unless the devboard is not connected
    char url[100];
    snprintf(
        fhttp.file_path,
        sizeof(fhttp.file_path),
        STORAGE_EXT_PATH_PREFIX "/apps_data/flip_social/friends.txt");

    fhttp.save_received_data = true;
    auth_headers_alloc();
    snprintf(
        url,
        100,
        "https://www.flipsocial.net/api/user/friends/%s/",
        app_instance->login_username_logged_in);
    if(!flipper_http_get_request_with_headers(url, auth_headers)) {
        FURI_LOG_E(TAG, "Failed to send HTTP request for friends");
        fhttp.state = ISSUE;
        return false;
    }
    fhttp.state = RECEIVING;
    return true;
}

bool flip_social_update_friends() {
    if(!app_instance->submenu_friends) {
        FURI_LOG_E(TAG, "Friends submenu is NULL");
        return false;
    }
    if(!flip_social_friends) {
        FURI_LOG_E(TAG, "Friends model is NULL");
        return false;
    }
    // Add submenu items for the users
    submenu_reset(app_instance->submenu_friends);
    submenu_set_header(app_instance->submenu_friends, "Friends");
    for(int i = 0; i < flip_social_friends->count; i++) {
        submenu_add_item(
            app_instance->submenu_friends,
            flip_social_friends->usernames[i],
            FlipSocialSubmenuLoggedInIndexFriendsStart + i,
            flip_social_callback_submenu_choices,
            app_instance);
    }
    return true;
}

bool flip_social_parse_json_friends() {
    // load the received data from the saved file
    FuriString* friend_data = flipper_http_load_from_file(fhttp.file_path);
    if(friend_data == NULL) {
        FURI_LOG_E(TAG, "Failed to load received data from file.");
        return false;
    }
    char* data_cstr = (char*)furi_string_get_cstr(friend_data);
    if(data_cstr == NULL) {
        FURI_LOG_E(TAG, "Failed to get C-string from FuriString.");
        furi_string_free(friend_data);
        return false;
    }

    // Allocate memory for each username only if not already allocated
    flip_social_friends = flip_social_friends_alloc();
    if(flip_social_friends == NULL) {
        FURI_LOG_E(TAG, "Failed to allocate memory for friends usernames.");
        furi_string_free(friend_data);
        free(data_cstr);
        return false;
    }

    // Remove newlines
    char* pos = data_cstr;
    while((pos = strchr(pos, '\n')) != NULL) {
        *pos = ' ';
    }

    // Initialize friends count
    flip_social_friends->count = 0;

    // Extract the users array from the JSON
    char* json_users = get_json_value("friends", data_cstr, MAX_TOKENS);
    if(json_users == NULL) {
        FURI_LOG_E(TAG, "Failed to parse friends array.");
        furi_string_free(friend_data);
        free(data_cstr);
        return false;
    }

    // Manual tokenization for comma-separated values
    char* start = json_users + 1; // Skip the opening bracket
    char* end;
    while((end = strchr(start, ',')) != NULL && flip_social_friends->count < MAX_FRIENDS) {
        *end = '\0'; // Null-terminate the current token

        // Remove quotes
        if(*start == '"') start++;
        if(*(end - 1) == '"') *(end - 1) = '\0';

        // Copy username to pre-allocated memory
        snprintf(
            flip_social_friends->usernames[flip_social_friends->count],
            MAX_USER_LENGTH,
            "%s",
            start);
        flip_social_friends->count++;
        start = end + 1;
    }

    // Handle the last token
    if(*start != '\0' && flip_social_friends->count < MAX_FRIENDS) {
        if(*start == '"') start++;
        if(*(start + strlen(start) - 1) == ']') *(start + strlen(start) - 1) = '\0';
        if(*(start + strlen(start) - 1) == '"') *(start + strlen(start) - 1) = '\0';

        snprintf(
            flip_social_friends->usernames[flip_social_friends->count],
            MAX_USER_LENGTH,
            "%s",
            start);
        flip_social_friends->count++;
    }

    // Add submenu items for the friends
    if(!flip_social_update_friends()) {
        FURI_LOG_E(TAG, "Failed to update friends submenu");
        furi_string_free(friend_data);
        free(data_cstr);
        return false;
    }

    // Free the json_users
    free(json_users);
    free(start);
    free(end);
    furi_string_free(friend_data);
    free(data_cstr);
    return true;
}
