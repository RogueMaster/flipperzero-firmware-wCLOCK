#include <alloc/flip_social_alloc.h>

FlipSocialApp* flip_social_app_alloc() {
    // Initiailize the app
    FlipSocialApp* app = (FlipSocialApp*)malloc(sizeof(FlipSocialApp));

    // Initialize gui
    Gui* gui = furi_record_open(RECORD_GUI);

    // Initialize UART
    if(!flipper_http_init(flipper_http_rx_callback, app)) {
        FURI_LOG_E(TAG, "Failed to initialize UART");
        return NULL;
    }

    // Allocate ViewDispatcher
    if(!easy_flipper_set_view_dispatcher(&app->view_dispatcher, gui, app)) {
        return NULL;
    }

    // Allocate the text input buffers
    app->wifi_ssid_logged_out_temp_buffer_size = MAX_USER_LENGTH;
    app->wifi_password_logged_out_temp_buffer_size = MAX_USER_LENGTH;
    app->login_username_logged_out_temp_buffer_size = MAX_USER_LENGTH;
    app->login_password_logged_out_temp_buffer_size = MAX_USER_LENGTH;
    app->register_username_logged_out_temp_buffer_size = MAX_USER_LENGTH;
    app->register_password_logged_out_temp_buffer_size = MAX_USER_LENGTH;
    app->register_password_2_logged_out_temp_buffer_size = MAX_USER_LENGTH;
    app->change_password_logged_in_temp_buffer_size = MAX_USER_LENGTH;
    app->compose_pre_save_logged_in_temp_buffer_size = MAX_MESSAGE_LENGTH;
    app->wifi_ssid_logged_in_temp_buffer_size = MAX_USER_LENGTH;
    app->wifi_password_logged_in_temp_buffer_size = MAX_USER_LENGTH;
    app->is_logged_in_size = 8;
    app->login_username_logged_in_temp_buffer_size = MAX_USER_LENGTH;
    app->messages_new_message_logged_in_temp_buffer_size = MAX_MESSAGE_LENGTH;
    app->message_user_choice_logged_in_temp_buffer_size = MAX_MESSAGE_LENGTH;
    if(!easy_flipper_set_buffer(
           &app->wifi_ssid_logged_out_temp_buffer, app->wifi_ssid_logged_out_temp_buffer_size)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->wifi_password_logged_out_temp_buffer,
           app->wifi_password_logged_out_temp_buffer_size)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->login_username_logged_out_temp_buffer,
           app->login_username_logged_out_temp_buffer_size)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->login_password_logged_out_temp_buffer,
           app->login_password_logged_out_temp_buffer_size)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->register_username_logged_out_temp_buffer,
           app->register_username_logged_out_temp_buffer_size)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->register_password_logged_out_temp_buffer,
           app->register_password_logged_out_temp_buffer_size)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->register_password_2_logged_out_temp_buffer,
           app->register_password_2_logged_out_temp_buffer_size)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->change_password_logged_in_temp_buffer,
           app->change_password_logged_in_temp_buffer_size)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->compose_pre_save_logged_in_temp_buffer,
           app->compose_pre_save_logged_in_temp_buffer_size)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->wifi_ssid_logged_in_temp_buffer, app->wifi_ssid_logged_in_temp_buffer_size)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->wifi_password_logged_in_temp_buffer,
           app->wifi_password_logged_in_temp_buffer_size)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(&app->is_logged_in, app->is_logged_in_size)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->login_username_logged_in_temp_buffer,
           app->login_username_logged_in_temp_buffer_size)) {
        return NULL;
    }

    if(!easy_flipper_set_buffer(
           &app->wifi_ssid_logged_out, app->wifi_ssid_logged_out_temp_buffer_size)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->wifi_password_logged_out, app->wifi_password_logged_out_temp_buffer_size)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->login_username_logged_out, app->login_username_logged_out_temp_buffer_size)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->login_password_logged_out, app->login_password_logged_out_temp_buffer_size)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->register_username_logged_out,
           app->register_username_logged_out_temp_buffer_size)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->register_password_logged_out,
           app->register_password_logged_out_temp_buffer_size)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->register_password_2_logged_out,
           app->register_password_2_logged_out_temp_buffer_size)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->change_password_logged_in, app->change_password_logged_in_temp_buffer_size)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->compose_pre_save_logged_in, app->compose_pre_save_logged_in_temp_buffer_size)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->wifi_ssid_logged_in, app->wifi_ssid_logged_in_temp_buffer_size)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->wifi_password_logged_in, app->wifi_password_logged_in_temp_buffer_size)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->login_username_logged_in, app->login_username_logged_in_temp_buffer_size)) {
        return NULL;
    }
    //
    if(!easy_flipper_set_buffer(
           &app->messages_new_message_logged_in,
           app->messages_new_message_logged_in_temp_buffer_size)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->messages_new_message_logged_in_temp_buffer,
           app->messages_new_message_logged_in_temp_buffer_size)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->message_user_choice_logged_in,
           app->message_user_choice_logged_in_temp_buffer_size)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &app->message_user_choice_logged_in_temp_buffer,
           app->message_user_choice_logged_in_temp_buffer_size)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &selected_message, app->message_user_choice_logged_in_temp_buffer_size)) {
        return NULL;
    }
    if(!easy_flipper_set_buffer(
           &last_explore_response, app->message_user_choice_logged_in_temp_buffer_size)) {
        return NULL;
    }

    // Allocate Submenu(s)
    if(!easy_flipper_set_submenu(
           &app->submenu_logged_out,
           FlipSocialViewLoggedOutSubmenu,
           "FlipSocial v0.6",
           flip_social_callback_exit_app,
           &app->view_dispatcher)) {
        return NULL;
    }
    if(!easy_flipper_set_submenu(
           &app->submenu_logged_in,
           FlipSocialViewLoggedInSubmenu,
           "FlipSocial v0.6",
           flip_social_callback_exit_app,
           &app->view_dispatcher)) {
        return NULL;
    }
    if(!easy_flipper_set_submenu(
           &app->submenu_compose,
           FlipSocialViewLoggedInCompose,
           "Create A Post",
           flip_social_callback_to_submenu_logged_in,
           &app->view_dispatcher)) {
        return NULL;
    }
    if(!easy_flipper_set_submenu(
           &app->submenu_explore,
           FlipSocialViewLoggedInExploreSubmenu,
           "Explore",
           flip_social_callback_to_submenu_logged_in,
           &app->view_dispatcher)) {
        return NULL;
    }
    if(!easy_flipper_set_submenu(
           &app->submenu_friends,
           FlipSocialViewLoggedInFriendsSubmenu,
           "Friends",
           flip_social_callback_to_profile_logged_in,
           &app->view_dispatcher)) {
        return NULL;
    }
    if(!easy_flipper_set_submenu(
           &app->submenu_messages,
           FlipSocialViewLoggedInMessagesSubmenu,
           "Messages",
           flip_social_callback_to_submenu_logged_in,
           &app->view_dispatcher)) {
        return NULL;
    }
    if(!easy_flipper_set_submenu(
           &app->submenu_messages_user_choices,
           FlipSocialViewLoggedInMessagesUserChoices,
           "Users",
           flip_social_callback_to_messages_logged_in,
           &app->view_dispatcher)) {
        return NULL;
    }

    submenu_add_item(
        app->submenu_logged_out,
        "Login",
        FlipSocialSubmenuLoggedOutIndexLogin,
        flip_social_callback_submenu_choices,
        app);
    submenu_add_item(
        app->submenu_logged_out,
        "Register",
        FlipSocialSubmenuLoggedOutIndexRegister,
        flip_social_callback_submenu_choices,
        app);
    submenu_add_item(
        app->submenu_logged_out,
        "About",
        FlipSocialSubmenuLoggedOutIndexAbout,
        flip_social_callback_submenu_choices,
        app);
    submenu_add_item(
        app->submenu_logged_out,
        "Settings",
        FlipSocialSubmenuLoggedOutIndexWifiSettings,
        flip_social_callback_submenu_choices,
        app);
    //
    submenu_add_item(
        app->submenu_logged_in,
        "Explore",
        FlipSocialSubmenuExploreIndex,
        flip_social_callback_submenu_choices,
        app);
    submenu_add_item(
        app->submenu_logged_in,
        "Feed",
        FlipSocialSubmenuLoggedInIndexFeed,
        flip_social_callback_submenu_choices,
        app);
    submenu_add_item(
        app->submenu_logged_in,
        "Post",
        FlipSocialSubmenuLoggedInIndexCompose,
        flip_social_callback_submenu_choices,
        app);
    submenu_add_item(
        app->submenu_logged_in,
        "Messages",
        FlipSocialSubmenuLoggedInIndexMessages,
        flip_social_callback_submenu_choices,
        app);
    submenu_add_item(
        app->submenu_logged_in,
        "Profile",
        FlipSocialSubmenuLoggedInIndexProfile,
        flip_social_callback_submenu_choices,
        app);
    submenu_add_item(
        app->submenu_logged_in,
        "Settings",
        FlipSocialSubmenuLoggedInIndexSettings,
        flip_social_callback_submenu_choices,
        app);
    submenu_add_item(
        app->submenu_logged_in,
        "Sign Out",
        FlipSocialSubmenuLoggedInSignOutButton,
        flip_social_callback_submenu_choices,
        app);
    //
    submenu_add_item(
        app->submenu_compose,
        "Add Pre-Save",
        FlipSocialSubmenuComposeIndexAddPreSave,
        flip_social_callback_submenu_choices,
        app);
    //

    // Allocate View(s)
    if(!easy_flipper_set_view(
           &app->view_process_login,
           FlipSocialViewLoggedOutProcessLogin,
           flip_social_callback_draw_login,
           NULL,
           flip_social_callback_to_login_logged_out,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_view(
           &app->view_process_register,
           FlipSocialViewLoggedOutProcessRegister,
           flip_social_callback_draw_register,
           NULL,
           flip_social_callback_to_register_logged_out,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_view(
           &app->view_process_feed,
           FlipSocialViewLoggedInFeed,
           flip_social_callback_draw_feed,
           NULL,
           flip_social_callback_to_submenu_logged_in,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_view(
           &app->view_process_compose,
           FlipSocialViewLoggedInProcessCompose,
           flip_social_callback_draw_compose,
           NULL,
           flip_social_callback_to_compose_logged_in,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_view(
           &app->view_process_explore,
           FlipSocialViewLoggedInExploreProccess,
           flip_social_callback_draw_explore,
           NULL,
           flip_social_callback_to_explore_logged_in,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_view(
           &app->view_process_friends,
           FlipSocialViewLoggedInFriendsProcess,
           flip_social_callback_draw_friends,
           NULL,
           flip_social_callback_to_friends_logged_in,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_view(
           &app->view_process_messages,
           FlipSocialViewLoggedInMessagesProcess,
           flip_social_callback_draw_messages,
           NULL,
           flip_social_callback_to_messages_logged_in,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }

    // Setup Variable Item List(s)
    if(!easy_flipper_set_variable_item_list(
           &app->variable_item_list_logged_out_wifi_settings,
           FlipSocialViewLoggedOutWifiSettings,
           flip_social_text_input_logged_out_wifi_settings_item_selected,
           flip_social_callback_to_submenu_logged_out,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_variable_item_list(
           &app->variable_item_list_logged_out_login,
           FlipSocialViewLoggedOutLogin,
           flip_social_text_input_logged_out_login_item_selected,
           flip_social_callback_to_submenu_logged_out,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_variable_item_list(
           &app->variable_item_list_logged_out_register,
           FlipSocialViewLoggedOutRegister,
           flip_social_text_input_logged_out_register_item_selected,
           flip_social_callback_to_submenu_logged_out,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_variable_item_list(
           &app->variable_item_list_logged_in_profile,
           FlipSocialViewLoggedInProfile,
           flip_social_text_input_logged_in_profile_item_selected,
           flip_social_callback_to_submenu_logged_in,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_variable_item_list(
           &app->variable_item_list_logged_in_settings,
           FlipSocialViewLoggedInSettings,
           flip_social_text_input_logged_in_settings_item_selected,
           flip_social_callback_to_submenu_logged_in,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_variable_item_list(
           &app->variable_item_list_logged_in_settings_wifi,
           FlipSocialViewLoggedInSettingsWifi,
           flip_social_text_input_logged_in_wifi_settings_item_selected,
           flip_social_callback_to_settings_logged_in,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }

    app->variable_item_logged_out_wifi_settings_ssid = variable_item_list_add(
        app->variable_item_list_logged_out_wifi_settings, "SSID", 1, NULL, NULL);
    app->variable_item_logged_out_wifi_settings_password = variable_item_list_add(
        app->variable_item_list_logged_out_wifi_settings, "Password", 1, NULL, NULL);
    //
    app->variable_item_logged_out_login_username = variable_item_list_add(
        app->variable_item_list_logged_out_login, "Username", 1, NULL, NULL);
    app->variable_item_logged_out_login_password = variable_item_list_add(
        app->variable_item_list_logged_out_login, "Password", 1, NULL, NULL);
    app->variable_item_logged_out_login_button =
        variable_item_list_add(app->variable_item_list_logged_out_login, "Login", 0, NULL, NULL);
    //
    app->variable_item_logged_out_register_username = variable_item_list_add(
        app->variable_item_list_logged_out_register, "Username", 1, NULL, NULL);
    app->variable_item_logged_out_register_password = variable_item_list_add(
        app->variable_item_list_logged_out_register, "Password", 1, NULL, NULL);
    app->variable_item_logged_out_register_password_2 = variable_item_list_add(
        app->variable_item_list_logged_out_register, "Confirm Password", 1, NULL, NULL);
    app->variable_item_logged_out_register_button = variable_item_list_add(
        app->variable_item_list_logged_out_register, "Register", 0, NULL, NULL);
    //
    app->variable_item_logged_in_profile_username = variable_item_list_add(
        app->variable_item_list_logged_in_profile, "Username", 0, NULL, NULL);
    app->variable_item_logged_in_profile_change_password = variable_item_list_add(
        app->variable_item_list_logged_in_profile, "Change Password", 0, NULL, NULL);
    app->variable_item_logged_in_profile_friends = variable_item_list_add(
        app->variable_item_list_logged_in_profile, "Friends", 0, NULL, NULL);
    //
    app->variable_item_logged_in_settings_about =
        variable_item_list_add(app->variable_item_list_logged_in_settings, "About", 0, NULL, NULL);
    app->variable_item_logged_in_settings_wifi =
        variable_item_list_add(app->variable_item_list_logged_in_settings, "WiFi", 0, NULL, NULL);
    //
    app->variable_item_logged_in_wifi_settings_ssid = variable_item_list_add(
        app->variable_item_list_logged_in_settings_wifi, "SSID", 1, NULL, NULL);
    app->variable_item_logged_in_wifi_settings_password = variable_item_list_add(
        app->variable_item_list_logged_in_settings_wifi, "Password", 1, NULL, NULL);

    // Setup Text Input(s)
    if(!easy_flipper_set_uart_text_input(
           &app->text_input_logged_out_wifi_settings_ssid,
           FlipSocialViewLoggedOutWifiSettingsSSIDInput,
           "Enter SSID",
           app->wifi_ssid_logged_out_temp_buffer,
           app->wifi_ssid_logged_out_temp_buffer_size,
           flip_social_logged_out_wifi_settings_ssid_updated,
           flip_social_callback_to_wifi_settings_logged_out,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_uart_text_input(
           &app->text_input_logged_out_wifi_settings_password,
           FlipSocialViewLoggedOutWifiSettingsPasswordInput,
           "Enter Password",
           app->wifi_password_logged_out_temp_buffer,
           app->wifi_password_logged_out_temp_buffer_size,
           flip_social_logged_out_wifi_settings_password_updated,
           flip_social_callback_to_wifi_settings_logged_out,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_uart_text_input(
           &app->text_input_logged_out_login_username,
           FlipSocialViewLoggedOutLoginUsernameInput,
           "Enter Username",
           app->login_username_logged_out_temp_buffer,
           app->login_username_logged_out_temp_buffer_size,
           flip_social_logged_out_login_username_updated,
           flip_social_callback_to_login_logged_out,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_uart_text_input(
           &app->text_input_logged_out_login_password,
           FlipSocialViewLoggedOutLoginPasswordInput,
           "Enter Password",
           app->login_password_logged_out_temp_buffer,
           app->login_password_logged_out_temp_buffer_size,
           flip_social_logged_out_login_password_updated,
           flip_social_callback_to_login_logged_out,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_uart_text_input(
           &app->text_input_logged_out_register_username,
           FlipSocialViewLoggedOutRegisterUsernameInput,
           "Enter Username",
           app->register_username_logged_out_temp_buffer,
           app->register_username_logged_out_temp_buffer_size,
           flip_social_logged_out_register_username_updated,
           flip_social_callback_to_register_logged_out,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_uart_text_input(
           &app->text_input_logged_out_register_password,
           FlipSocialViewLoggedOutRegisterPasswordInput,
           "Enter Password",
           app->register_password_logged_out_temp_buffer,
           app->register_password_logged_out_temp_buffer_size,
           flip_social_logged_out_register_password_updated,
           flip_social_callback_to_register_logged_out,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_uart_text_input(
           &app->text_input_logged_out_register_password_2,
           FlipSocialViewLoggedOutRegisterPassword2Input,
           "Confirm Password",
           app->register_password_2_logged_out_temp_buffer,
           app->register_password_2_logged_out_temp_buffer_size,
           flip_social_logged_out_register_password_2_updated,
           flip_social_callback_to_register_logged_out,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    //
    if(!easy_flipper_set_uart_text_input(
           &app->text_input_logged_in_change_password,
           FlipSocialViewLoggedInChangePasswordInput,
           "Enter New Password",
           app->change_password_logged_in_temp_buffer,
           app->change_password_logged_in_temp_buffer_size,
           flip_social_logged_in_profile_change_password_updated,
           flip_social_callback_to_profile_logged_in,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_uart_text_input(
           &app->text_input_logged_in_compose_pre_save_input,
           FlipSocialViewLoggedInComposeAddPreSaveInput,
           "Enter Pre-Save Message",
           app->compose_pre_save_logged_in_temp_buffer,
           app->compose_pre_save_logged_in_temp_buffer_size,
           flip_social_logged_in_compose_pre_save_updated,
           flip_social_callback_to_compose_logged_in,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_uart_text_input(
           &app->text_input_logged_in_wifi_settings_ssid,
           FlipSocialViewLoggedInWifiSettingsSSIDInput,
           "Enter SSID",
           app->wifi_ssid_logged_in_temp_buffer,
           app->wifi_ssid_logged_in_temp_buffer_size,
           flip_social_logged_in_wifi_settings_ssid_updated,
           flip_social_callback_to_wifi_settings_logged_in,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_uart_text_input(
           &app->text_input_logged_in_wifi_settings_password,
           FlipSocialViewLoggedInWifiSettingsPasswordInput,
           "Enter Password",
           app->wifi_password_logged_in_temp_buffer,
           app->wifi_password_logged_in_temp_buffer_size,
           flip_social_logged_in_wifi_settings_password_updated,
           flip_social_callback_to_wifi_settings_logged_in,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    //
    if(!easy_flipper_set_uart_text_input(
           &app->text_input_logged_in_messages_new_message,
           FlipSocialViewLoggedInMessagesNewMessageInput,
           "Enter Message",
           app->messages_new_message_logged_in_temp_buffer,
           app->messages_new_message_logged_in_temp_buffer_size,
           flip_social_logged_in_messages_new_message_updated,
           flip_social_callback_to_messages_logged_in,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }
    if(!easy_flipper_set_uart_text_input(
           &app->text_input_logged_in_messages_new_message_user_choices,
           FlipSocialViewLoggedInMessagesNewMessageUserChoicesInput,
           "Enter Message",
           app->message_user_choice_logged_in_temp_buffer,
           app->message_user_choice_logged_in_temp_buffer_size,
           flip_social_logged_in_messages_user_choice_message_updated,
           flip_social_callback_to_messages_user_choices,
           &app->view_dispatcher,
           app)) {
        return NULL;
    }

    // Setup About(s)
    if(!easy_flipper_set_widget(
           &app->widget_logged_out_about,
           FlipSocialViewLoggedOutAbout,
           "Welcome to FlipSocial\n---\nThe social media app for\nFlipper Zero, created by\nJBlanked: www.flipsocial.net\n---\nPress BACK to return.",
           flip_social_callback_to_submenu_logged_out,
           &app->view_dispatcher)) {
        return NULL;
    }
    if(!easy_flipper_set_widget(
           &app->widget_logged_in_about,
           FlipSocialViewLoggedInSettingsAbout,
           "Welcome to FlipSocial\n---\nThe social media app for\nFlipper Zero, created by\nJBlanked: www.flipsocial.net\n---\nPress BACK to return.",
           flip_social_callback_to_settings_logged_in,
           &app->view_dispatcher)) {
        return NULL;
    }

    // load the playlist
    if(load_playlist(&app->pre_saved_messages)) {
        // Update the playlist submenu
        for(uint32_t i = 0; i < app->pre_saved_messages.count; i++) {
            submenu_add_item(
                app->submenu_compose,
                app->pre_saved_messages.messages[i],
                FlipSocialSubemnuComposeIndexStartIndex + i,
                flip_social_callback_submenu_choices,
                app);
        }
    }

    // Load the settings
    if(!load_settings(
           app->wifi_ssid_logged_out,
           app->wifi_ssid_logged_out_temp_buffer_size,
           app->wifi_password_logged_out,
           app->wifi_password_logged_out_temp_buffer_size,
           app->login_username_logged_out,
           app->login_username_logged_out_temp_buffer_size,
           app->login_username_logged_in,
           app->login_username_logged_in_temp_buffer_size,
           app->login_password_logged_out,
           app->login_password_logged_out_temp_buffer_size,
           app->change_password_logged_in,
           app->change_password_logged_in_temp_buffer_size,
           app->is_logged_in,
           app->is_logged_in_size))

    {
        FURI_LOG_E(TAG, "Failed to load settings");

        if(app->is_logged_in == NULL) {
            app->is_logged_in = (char*)malloc(app->is_logged_in_size);
            app->is_logged_in = "false";
        }
        app_instance = app;
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedOutSubmenu);
    } else {
        // Copy items into their temp buffers with safety checks
        if(app->wifi_ssid_logged_out && app->wifi_ssid_logged_out_temp_buffer) {
            strncpy(
                app->wifi_ssid_logged_out_temp_buffer,
                app->wifi_ssid_logged_out,
                app->wifi_ssid_logged_out_temp_buffer_size - 1);
            app->wifi_ssid_logged_out_temp_buffer[app->wifi_ssid_logged_out_temp_buffer_size - 1] =
                '\0';
        }
        if(app->wifi_password_logged_out && app->wifi_password_logged_out_temp_buffer) {
            strncpy(
                app->wifi_password_logged_out_temp_buffer,
                app->wifi_password_logged_out,
                app->wifi_password_logged_out_temp_buffer_size - 1);
            app->wifi_password_logged_out_temp_buffer
                [app->wifi_password_logged_out_temp_buffer_size - 1] = '\0';
        }
        if(app->login_username_logged_out && app->login_username_logged_out_temp_buffer) {
            strncpy(
                app->login_username_logged_out_temp_buffer,
                app->login_username_logged_out,
                app->login_username_logged_out_temp_buffer_size - 1);
            app->login_username_logged_out_temp_buffer
                [app->login_username_logged_out_temp_buffer_size - 1] = '\0';
        }
        if(app->login_password_logged_out && app->login_password_logged_out_temp_buffer) {
            strncpy(
                app->login_password_logged_out_temp_buffer,
                app->login_password_logged_out,
                app->login_password_logged_out_temp_buffer_size - 1);
            app->login_password_logged_out_temp_buffer
                [app->login_password_logged_out_temp_buffer_size - 1] = '\0';
        }
        if(app->register_username_logged_out && app->register_username_logged_out_temp_buffer) {
            strncpy(
                app->register_username_logged_out_temp_buffer,
                app->register_username_logged_out,
                app->register_username_logged_out_temp_buffer_size - 1);
            app->register_username_logged_out_temp_buffer
                [app->register_username_logged_out_temp_buffer_size - 1] = '\0';
        }
        if(app->register_password_logged_out && app->register_password_logged_out_temp_buffer) {
            strncpy(
                app->register_password_logged_out_temp_buffer,
                app->register_password_logged_out,
                app->register_password_logged_out_temp_buffer_size - 1);
            app->register_password_logged_out_temp_buffer
                [app->register_password_logged_out_temp_buffer_size - 1] = '\0';
        }
        if(app->register_password_2_logged_out &&
           app->register_password_2_logged_out_temp_buffer) {
            strncpy(
                app->register_password_2_logged_out_temp_buffer,
                app->register_password_2_logged_out,
                app->register_password_2_logged_out_temp_buffer_size - 1);
            app->register_password_2_logged_out_temp_buffer
                [app->register_password_2_logged_out_temp_buffer_size - 1] = '\0';
        }
        if(app->change_password_logged_in && app->change_password_logged_in_temp_buffer) {
            strncpy(
                app->change_password_logged_in_temp_buffer,
                app->change_password_logged_in,
                app->change_password_logged_in_temp_buffer_size - 1);
            app->change_password_logged_in_temp_buffer
                [app->change_password_logged_in_temp_buffer_size - 1] = '\0';
        }
        if(app->compose_pre_save_logged_in && app->compose_pre_save_logged_in_temp_buffer) {
            strncpy(
                app->compose_pre_save_logged_in_temp_buffer,
                app->compose_pre_save_logged_in,
                app->compose_pre_save_logged_in_temp_buffer_size - 1);
            app->compose_pre_save_logged_in_temp_buffer
                [app->compose_pre_save_logged_in_temp_buffer_size - 1] = '\0';
        }
        if(app->wifi_ssid_logged_in && app->wifi_ssid_logged_in_temp_buffer) {
            strncpy(
                app->wifi_ssid_logged_in_temp_buffer,
                app->wifi_ssid_logged_in,
                app->wifi_ssid_logged_in_temp_buffer_size - 1);
            app->wifi_ssid_logged_in_temp_buffer[app->wifi_ssid_logged_in_temp_buffer_size - 1] =
                '\0';
        }
        if(app->wifi_password_logged_in && app->wifi_password_logged_in_temp_buffer) {
            strncpy(
                app->wifi_password_logged_in_temp_buffer,
                app->wifi_password_logged_in,
                app->wifi_password_logged_in_temp_buffer_size - 1);
            app->wifi_password_logged_in_temp_buffer
                [app->wifi_password_logged_in_temp_buffer_size - 1] = '\0';
        }
        if(app->login_username_logged_in && app->login_username_logged_in_temp_buffer) {
            strncpy(
                app->login_username_logged_in_temp_buffer,
                app->login_username_logged_in,
                app->login_username_logged_in_temp_buffer_size - 1);
            app->login_username_logged_in_temp_buffer
                [app->login_username_logged_in_temp_buffer_size - 1] = '\0';
        }

        // if login username is empty but logged out isnt, copy it over
        if(strlen(app->login_username_logged_out) > 0 &&
           strlen(app->login_username_logged_in) == 0) {
            strncpy(
                app->login_username_logged_in,
                app->login_username_logged_out,
                app->login_username_logged_in_temp_buffer_size - 1);
            strncpy(
                app->login_username_logged_in_temp_buffer,
                app->login_username_logged_out,
                app->login_username_logged_in_temp_buffer_size - 1);
            app->login_username_logged_in[app->login_username_logged_in_temp_buffer_size - 1] =
                '\0';
            app->login_username_logged_in_temp_buffer
                [app->login_username_logged_in_temp_buffer_size - 1] = '\0';
        }
        // logout username is empty but logged in isnt, copy it over
        if(strlen(app->login_username_logged_in) > 0 &&
           strlen(app->login_username_logged_out) == 0) {
            strncpy(
                app->login_username_logged_out,
                app->login_username_logged_in,
                app->login_username_logged_in_temp_buffer_size - 1);
            strncpy(
                app->login_username_logged_out_temp_buffer,
                app->login_username_logged_in,
                app->login_username_logged_in_temp_buffer_size - 1);
            app->login_username_logged_out[app->login_username_logged_in_temp_buffer_size - 1] =
                '\0';
            app->login_username_logged_out_temp_buffer
                [app->login_username_logged_in_temp_buffer_size - 1] = '\0';
        }
        // if login password is empty but logged out isnt, copy it over
        if(strlen(app->login_password_logged_out) > 0 &&
           strlen(app->change_password_logged_in) == 0) {
            strncpy(
                app->change_password_logged_in,
                app->login_password_logged_out,
                app->login_password_logged_out_temp_buffer_size - 1);
            strncpy(
                app->change_password_logged_in_temp_buffer,
                app->login_password_logged_out,
                app->login_password_logged_out_temp_buffer_size - 1);
            app->change_password_logged_in[app->login_password_logged_out_temp_buffer_size - 1] =
                '\0';
            app->change_password_logged_in_temp_buffer
                [app->login_password_logged_out_temp_buffer_size - 1] = '\0';
        }
        // if logout password is empty but logged in isnt, copy it over
        if(strlen(app->change_password_logged_in) > 0 &&
           strlen(app->login_password_logged_out) == 0) {
            strncpy(
                app->login_password_logged_out,
                app->change_password_logged_in,
                app->login_password_logged_out_temp_buffer_size - 1);
            strncpy(
                app->login_password_logged_out_temp_buffer,
                app->change_password_logged_in,
                app->login_password_logged_out_temp_buffer_size - 1);
            app->login_password_logged_out[app->login_password_logged_out_temp_buffer_size - 1] =
                '\0';
            app->login_password_logged_out_temp_buffer
                [app->login_password_logged_out_temp_buffer_size - 1] = '\0';
        }
        // if wifi password is empty but logged out isnt, copy it over
        if(strlen(app->wifi_password_logged_out) > 0 &&
           strlen(app->wifi_password_logged_in) == 0) {
            strncpy(
                app->wifi_password_logged_in,
                app->wifi_password_logged_out,
                app->wifi_password_logged_in_temp_buffer_size - 1);
            strncpy(
                app->wifi_password_logged_in_temp_buffer,
                app->wifi_password_logged_out,
                app->wifi_password_logged_in_temp_buffer_size - 1);
            app->wifi_password_logged_in[app->wifi_password_logged_in_temp_buffer_size - 1] = '\0';
            app->wifi_password_logged_in_temp_buffer
                [app->wifi_password_logged_in_temp_buffer_size - 1] = '\0';
        }
        // ifi wifi password is empty but logged in isnt, copy it over
        if(strlen(app->wifi_password_logged_in) > 0 &&
           strlen(app->wifi_password_logged_out) == 0) {
            strncpy(
                app->wifi_password_logged_out,
                app->wifi_password_logged_in,
                app->wifi_password_logged_in_temp_buffer_size - 1);
            strncpy(
                app->wifi_password_logged_out_temp_buffer,
                app->wifi_password_logged_in,
                app->wifi_password_logged_in_temp_buffer_size - 1);
            app->wifi_password_logged_out[app->wifi_password_logged_in_temp_buffer_size - 1] =
                '\0';
            app->wifi_password_logged_out_temp_buffer
                [app->wifi_password_logged_in_temp_buffer_size - 1] = '\0';
        }
        // if wifi ssid is empty but logged out isnt, copy it over
        if(strlen(app->wifi_ssid_logged_out) > 0 && strlen(app->wifi_ssid_logged_in) == 0) {
            strncpy(
                app->wifi_ssid_logged_in,
                app->wifi_ssid_logged_out,
                app->wifi_ssid_logged_in_temp_buffer_size - 1);
            strncpy(
                app->wifi_ssid_logged_in_temp_buffer,
                app->wifi_ssid_logged_out,
                app->wifi_ssid_logged_in_temp_buffer_size - 1);
            app->wifi_ssid_logged_in[app->wifi_ssid_logged_in_temp_buffer_size - 1] = '\0';
            app->wifi_ssid_logged_in_temp_buffer[app->wifi_ssid_logged_in_temp_buffer_size - 1] =
                '\0';
        }
        // if wifi ssid is empty but logged in isnt, copy it over
        if(strlen(app->wifi_ssid_logged_in) > 0 && strlen(app->wifi_ssid_logged_out) == 0) {
            strncpy(
                app->wifi_ssid_logged_out,
                app->wifi_ssid_logged_in,
                app->wifi_ssid_logged_in_temp_buffer_size - 1);
            strncpy(
                app->wifi_ssid_logged_out_temp_buffer,
                app->wifi_ssid_logged_in,
                app->wifi_ssid_logged_in_temp_buffer_size - 1);
            app->wifi_ssid_logged_out[app->wifi_ssid_logged_in_temp_buffer_size - 1] = '\0';
            app->wifi_ssid_logged_out_temp_buffer[app->wifi_ssid_logged_in_temp_buffer_size - 1] =
                '\0';
        }

        auth_headers_alloc();

        // set variable item text (ommit the passwords)
        variable_item_set_current_value_text(
            app->variable_item_logged_in_wifi_settings_ssid, app->wifi_ssid_logged_in);
        variable_item_set_current_value_text(
            app->variable_item_logged_out_wifi_settings_ssid, app->wifi_ssid_logged_out);
        variable_item_set_current_value_text(
            app->variable_item_logged_out_login_username, app->login_username_logged_out);
        variable_item_set_current_value_text(
            app->variable_item_logged_in_profile_username, app->login_username_logged_in);
        //

        if(app->is_logged_in != NULL && strcmp(app->is_logged_in, "true") == 0) {
            view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedInSubmenu);
        } else {
            view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedOutSubmenu);
        }
    }

    return app;
}
