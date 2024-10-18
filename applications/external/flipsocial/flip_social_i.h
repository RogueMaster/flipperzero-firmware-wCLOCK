// flip_social.i.h
#ifndef FLIP_SOCIAL_I
#define FLIP_SOCIAL_I

/**
 * @brief Function to allocate resources for the FlipSocialApp.
 * @details Initializes all components and views of the application.
 * @return Pointer to the initialized FlipSocialApp, or NULL on failure.
 */
static FlipSocialApp* flip_social_app_alloc() {
    // Initiailize the app
    FlipSocialApp* app = (FlipSocialApp*)malloc(sizeof(FlipSocialApp));
    if(!app) {
        // Allocation failed
        FURI_LOG_E(TAG, "Failed to allocate FlipSocialApp");
        return NULL;
    }
    memset(app, 0, sizeof(FlipSocialApp));

    // Initialize gui
    Gui* gui = furi_record_open(RECORD_GUI);
    if(!gui) {
        // Failed to open GUI
        FURI_LOG_E(TAG, "Failed to open GUI record");
        return NULL;
    }

    // Initialize UART
    if(!flipper_http_init(flipper_http_rx_callback, app)) {
        FURI_LOG_E(TAG, "Failed to initialize UART");
        return NULL;
    }

    // Allocate ViewDispatcher
    app->view_dispatcher = view_dispatcher_alloc();
    if(!app->view_dispatcher) {
        FURI_LOG_E(TAG, "Failed to allocate ViewDispatcher");
        return NULL;
    }
    // Attach ViewDispatcher to GUI
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    // initialize members
    app->wifi_ssid_logged_out_temp_buffer_size = 128;
    app->wifi_password_logged_out_temp_buffer_size = 128;
    app->wifi_ssid_logged_out_temp_buffer =
        (char*)malloc(app->wifi_ssid_logged_out_temp_buffer_size);
    app->wifi_password_logged_out_temp_buffer =
        (char*)malloc(app->wifi_password_logged_out_temp_buffer_size);
    app->wifi_ssid_logged_out = (char*)malloc(app->wifi_ssid_logged_out_temp_buffer_size);
    app->wifi_password_logged_out = (char*)malloc(app->wifi_password_logged_out_temp_buffer_size);

    app->login_username_logged_out_temp_buffer_size = 128;
    app->login_password_logged_out_temp_buffer_size = 128;
    app->login_username_logged_out_temp_buffer =
        (char*)malloc(app->login_username_logged_out_temp_buffer_size);
    app->login_password_logged_out_temp_buffer =
        (char*)malloc(app->login_password_logged_out_temp_buffer_size);
    app->login_username_logged_out =
        (char*)malloc(app->login_username_logged_out_temp_buffer_size);
    app->login_password_logged_out =
        (char*)malloc(app->login_password_logged_out_temp_buffer_size);

    app->register_username_logged_out_temp_buffer_size = 128;
    app->register_password_logged_out_temp_buffer_size = 128;
    app->register_password_2_logged_out_temp_buffer_size = 128;
    app->register_username_logged_out_temp_buffer =
        (char*)malloc(app->register_username_logged_out_temp_buffer_size);
    app->register_password_logged_out_temp_buffer =
        (char*)malloc(app->register_password_logged_out_temp_buffer_size);
    app->register_password_2_logged_out_temp_buffer =
        (char*)malloc(app->register_password_2_logged_out_temp_buffer_size);
    app->register_username_logged_out =
        (char*)malloc(app->register_username_logged_out_temp_buffer_size);
    app->register_password_logged_out =
        (char*)malloc(app->register_password_logged_out_temp_buffer_size);
    app->register_password_2_logged_out =
        (char*)malloc(app->register_password_2_logged_out_temp_buffer_size);

    app->change_password_logged_in_temp_buffer_size = 128;
    app->compose_pre_save_logged_in_temp_buffer_size = 128;
    app->wifi_ssid_logged_in_temp_buffer_size = 128;
    app->wifi_password_logged_in_temp_buffer_size = 128;
    app->is_logged_in_size = 128;
    app->login_username_logged_in_temp_buffer_size = 128;

    app->change_password_logged_in_temp_buffer =
        (char*)malloc(app->change_password_logged_in_temp_buffer_size);
    app->compose_pre_save_logged_in_temp_buffer =
        (char*)malloc(app->compose_pre_save_logged_in_temp_buffer_size);
    app->wifi_ssid_logged_in_temp_buffer =
        (char*)malloc(app->wifi_ssid_logged_in_temp_buffer_size);
    app->wifi_password_logged_in_temp_buffer =
        (char*)malloc(app->wifi_password_logged_in_temp_buffer_size);
    app->change_password_logged_in =
        (char*)malloc(app->change_password_logged_in_temp_buffer_size);
    app->compose_pre_save_logged_in =
        (char*)malloc(app->compose_pre_save_logged_in_temp_buffer_size);
    app->wifi_ssid_logged_in = (char*)malloc(app->wifi_ssid_logged_in_temp_buffer_size);
    app->wifi_password_logged_in = (char*)malloc(app->wifi_password_logged_in_temp_buffer_size);
    app->is_logged_in = (char*)malloc(app->is_logged_in_size);
    app->login_username_logged_in = (char*)malloc(app->login_username_logged_in_temp_buffer_size);

    if(!app->wifi_ssid_logged_out || !app->wifi_password_logged_out ||
       !app->login_username_logged_out || !app->login_password_logged_out ||
       !app->register_username_logged_out || !app->register_password_logged_out ||
       !app->register_password_2_logged_out || !app->change_password_logged_in ||
       !app->compose_pre_save_logged_in || !app->wifi_ssid_logged_in ||
       !app->wifi_password_logged_in || !app->change_password_logged_in ||
       !app->compose_pre_save_logged_in || !app->wifi_ssid_logged_in ||
       !app->wifi_password_logged_in || !app->is_logged_in || !app->login_username_logged_in)

    {
        // Allocation failed
        FURI_LOG_E(TAG, "Failed to allocate buffers");
        return NULL;
    }

    // Initialize buffers with empty strings
    app->wifi_ssid_logged_out_temp_buffer[0] = '\0';
    app->wifi_password_logged_out_temp_buffer[0] = '\0';
    app->login_username_logged_out_temp_buffer[0] = '\0';
    app->login_password_logged_out_temp_buffer[0] = '\0';
    app->register_username_logged_out_temp_buffer[0] = '\0';
    app->register_password_logged_out_temp_buffer[0] = '\0';
    app->register_password_2_logged_out_temp_buffer[0] = '\0';
    app->change_password_logged_in_temp_buffer[0] = '\0';
    app->compose_pre_save_logged_in_temp_buffer[0] = '\0';
    app->wifi_ssid_logged_in_temp_buffer[0] = '\0';
    app->wifi_password_logged_in_temp_buffer[0] = '\0';
    app->change_password_logged_in[0] = '\0';
    app->compose_pre_save_logged_in[0] = '\0';
    app->wifi_ssid_logged_in[0] = '\0';
    app->wifi_password_logged_in[0] = '\0';
    app->is_logged_in[0] = '\0';
    app->login_username_logged_in[0] = '\0';

    // Setup Submenu(s)
    app->submenu_logged_out = submenu_alloc();
    app->submenu_logged_in = submenu_alloc();
    app->submenu_compose = submenu_alloc();
    if(!app->submenu_logged_out || !app->submenu_logged_in || !app->submenu_compose) {
        FURI_LOG_E(TAG, "Failed to allocate Submenus");
        return NULL;
    }
    submenu_set_header(app->submenu_logged_out, "FlipSocial");
    submenu_set_header(app->submenu_logged_in, "FlipSocial");
    submenu_set_header(app->submenu_compose, "Create A Post");
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

    submenu_add_item(
        app->submenu_compose,
        "Add Pre-Save",
        FlipSocialSubmenuComposeIndexAddPreSave,
        flip_social_callback_submenu_choices,
        app);

    view_set_previous_callback(
        submenu_get_view(app->submenu_logged_out), flip_social_callback_exit_app);
    view_set_previous_callback(
        submenu_get_view(app->submenu_logged_in),
        flip_social_callback_exit_app); // exit the app isntead of returning to the logged out screen
    view_set_previous_callback(
        submenu_get_view(app->submenu_compose), flip_social_callback_to_submenu_logged_in);
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipSocialViewLoggedOutSubmenu,
        submenu_get_view(app->submenu_logged_out));
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipSocialViewLoggedInSubmenu,
        submenu_get_view(app->submenu_logged_in));
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipSocialViewLoggedInCompose,
        submenu_get_view(app->submenu_compose));

    app->view_process_login = view_alloc();
    app->view_process_register = view_alloc();
    app->view_process_feed = view_alloc();
    app->view_process_compose = view_alloc();
    if(!app->view_process_login || !app->view_process_register || !app->view_process_feed ||
       !app->view_process_compose) {
        FURI_LOG_E(TAG, "Failed to allocate View");
        return NULL;
    }
    view_set_draw_callback(app->view_process_login, flip_social_callback_draw_login);
    view_set_draw_callback(app->view_process_register, flip_social_callback_draw_register);
    view_set_draw_callback(app->view_process_feed, flip_social_callback_draw_feed);
    view_set_draw_callback(app->view_process_compose, flip_social_callback_draw_compose);
    view_set_context(app->view_process_login, app);
    view_set_context(app->view_process_register, app);
    view_set_context(app->view_process_feed, app);
    view_set_context(app->view_process_compose, app);
    view_set_previous_callback(app->view_process_login, flip_social_callback_to_login_logged_out);
    view_set_previous_callback(
        app->view_process_register, flip_social_callback_to_register_logged_out);
    view_set_previous_callback(app->view_process_feed, flip_social_callback_to_submenu_logged_in);
    view_set_previous_callback(
        app->view_process_compose, flip_social_callback_to_compose_logged_in);
    view_dispatcher_add_view(
        app->view_dispatcher, FlipSocialViewLoggedOutProcessLogin, app->view_process_login);
    view_dispatcher_add_view(
        app->view_dispatcher, FlipSocialViewLoggedOutProcessRegister, app->view_process_register);
    view_dispatcher_add_view(
        app->view_dispatcher, FlipSocialViewLoggedInFeed, app->view_process_feed);
    view_dispatcher_add_view(
        app->view_dispatcher, FlipSocialViewLoggedInProcessCompose, app->view_process_compose);

    // Setup Variable Item List(s)
    app->variable_item_list_logged_out_wifi_settings = variable_item_list_alloc();
    app->variable_item_list_logged_out_login = variable_item_list_alloc();
    app->variable_item_list_logged_out_register = variable_item_list_alloc();
    app->variable_item_list_logged_in_profile = variable_item_list_alloc();
    app->variable_item_list_logged_in_settings = variable_item_list_alloc();
    app->variable_item_list_logged_in_settings_wifi = variable_item_list_alloc();
    if(!app->variable_item_list_logged_out_wifi_settings ||
       !app->variable_item_list_logged_out_login || !app->variable_item_list_logged_out_register ||
       !app->variable_item_list_logged_in_profile || !app->variable_item_list_logged_in_settings ||
       !app->variable_item_list_logged_in_settings_wifi) {
        FURI_LOG_E(TAG, "Failed to allocate VariableItemList");
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

    variable_item_list_set_enter_callback(
        app->variable_item_list_logged_out_wifi_settings,
        flip_social_text_input_logged_out_wifi_settings_item_selected,
        app);
    view_set_previous_callback(
        variable_item_list_get_view(app->variable_item_list_logged_out_wifi_settings),
        flip_social_callback_to_submenu_logged_out);
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipSocialViewLoggedOutWifiSettings,
        variable_item_list_get_view(app->variable_item_list_logged_out_wifi_settings));
    //
    variable_item_list_set_enter_callback(
        app->variable_item_list_logged_out_login,
        flip_social_text_input_logged_out_login_item_selected,
        app);
    view_set_previous_callback(
        variable_item_list_get_view(app->variable_item_list_logged_out_login),
        flip_social_callback_to_submenu_logged_out);
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipSocialViewLoggedOutLogin,
        variable_item_list_get_view(app->variable_item_list_logged_out_login));
    //
    variable_item_list_set_enter_callback(
        app->variable_item_list_logged_out_register,
        flip_social_text_input_logged_out_register_item_selected,
        app);
    view_set_previous_callback(
        variable_item_list_get_view(app->variable_item_list_logged_out_register),
        flip_social_callback_to_submenu_logged_out);
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipSocialViewLoggedOutRegister,
        variable_item_list_get_view(app->variable_item_list_logged_out_register));
    //
    variable_item_list_set_enter_callback(
        app->variable_item_list_logged_in_profile,
        flip_social_text_input_logged_in_profile_item_selected,
        app);
    view_set_previous_callback(
        variable_item_list_get_view(app->variable_item_list_logged_in_profile),
        flip_social_callback_to_submenu_logged_in);
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipSocialViewLoggedInProfile,
        variable_item_list_get_view(app->variable_item_list_logged_in_profile));
    //
    variable_item_list_set_enter_callback(
        app->variable_item_list_logged_in_settings,
        flip_social_text_input_logged_in_settings_item_selected,
        app);
    view_set_previous_callback(
        variable_item_list_get_view(app->variable_item_list_logged_in_settings),
        flip_social_callback_to_submenu_logged_in);
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipSocialViewLoggedInSettings,
        variable_item_list_get_view(app->variable_item_list_logged_in_settings));
    //
    variable_item_list_set_enter_callback(
        app->variable_item_list_logged_in_settings_wifi,
        flip_social_text_input_logged_in_wifi_settings_item_selected,
        app);
    view_set_previous_callback(
        variable_item_list_get_view(app->variable_item_list_logged_in_settings_wifi),
        flip_social_callback_to_settings_logged_in);
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipSocialViewLoggedInSettingsWifi,
        variable_item_list_get_view(app->variable_item_list_logged_in_settings_wifi));

    // Setup Text Input(s)
    app->text_input_logged_out_wifi_settings_ssid = uart_text_input_alloc();
    app->text_input_logged_out_wifi_settings_password = uart_text_input_alloc();
    app->text_input_logged_out_login_username = uart_text_input_alloc();
    app->text_input_logged_out_login_password = uart_text_input_alloc();
    app->text_input_logged_out_register_username = uart_text_input_alloc();
    app->text_input_logged_out_register_password = uart_text_input_alloc();
    app->text_input_logged_out_register_password_2 = uart_text_input_alloc();
    app->text_input_logged_in_change_password = uart_text_input_alloc();
    app->text_input_logged_in_compose_pre_save_input = uart_text_input_alloc();
    app->text_input_logged_in_wifi_settings_ssid = uart_text_input_alloc();
    app->text_input_logged_in_wifi_settings_password = uart_text_input_alloc();
    if(!app->text_input_logged_out_wifi_settings_ssid ||
       !app->text_input_logged_out_wifi_settings_password ||
       !app->text_input_logged_out_login_username || !app->text_input_logged_out_login_password ||
       !app->text_input_logged_out_register_username ||
       !app->text_input_logged_out_register_password ||
       !app->text_input_logged_out_register_password_2 ||
       !app->text_input_logged_in_change_password ||
       !app->text_input_logged_in_compose_pre_save_input ||
       !app->text_input_logged_in_wifi_settings_ssid ||
       !app->text_input_logged_in_wifi_settings_password) {
        FURI_LOG_E(TAG, "Failed to allocate Text Input");
        return NULL;
    }
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipSocialViewLoggedOutWifiSettingsSSIDInput,
        uart_text_input_get_view(app->text_input_logged_out_wifi_settings_ssid));
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipSocialViewLoggedOutWifiSettingsPasswordInput,
        uart_text_input_get_view(app->text_input_logged_out_wifi_settings_password));
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipSocialViewLoggedOutLoginUsernameInput,
        uart_text_input_get_view(app->text_input_logged_out_login_username));
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipSocialViewLoggedOutLoginPasswordInput,
        uart_text_input_get_view(app->text_input_logged_out_login_password));
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipSocialViewLoggedOutRegisterUsernameInput,
        uart_text_input_get_view(app->text_input_logged_out_register_username));
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipSocialViewLoggedOutRegisterPasswordInput,
        uart_text_input_get_view(app->text_input_logged_out_register_password));
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipSocialViewLoggedOutRegisterPassword2Input,
        uart_text_input_get_view(app->text_input_logged_out_register_password_2));
    //
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipSocialViewLoggedInChangePasswordInput,
        uart_text_input_get_view(app->text_input_logged_in_change_password));
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipSocialViewLoggedInComposeAddPreSaveInput,
        uart_text_input_get_view(app->text_input_logged_in_compose_pre_save_input));
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipSocialViewLoggedInWifiSettingsSSIDInput,
        uart_text_input_get_view(app->text_input_logged_in_wifi_settings_ssid));
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipSocialViewLoggedInWifiSettingsPasswordInput,
        uart_text_input_get_view(app->text_input_logged_in_wifi_settings_password));

    view_set_previous_callback(
        uart_text_input_get_view(app->text_input_logged_out_wifi_settings_ssid),
        flip_social_callback_to_wifi_settings_logged_out);
    view_set_previous_callback(
        uart_text_input_get_view(app->text_input_logged_out_wifi_settings_password),
        flip_social_callback_to_wifi_settings_logged_out);
    view_set_previous_callback(
        uart_text_input_get_view(app->text_input_logged_out_login_username),
        flip_social_callback_to_login_logged_out);
    view_set_previous_callback(
        uart_text_input_get_view(app->text_input_logged_out_login_password),
        flip_social_callback_to_login_logged_out);
    view_set_previous_callback(
        uart_text_input_get_view(app->text_input_logged_out_register_username),
        flip_social_callback_to_register_logged_out);
    view_set_previous_callback(
        uart_text_input_get_view(app->text_input_logged_out_register_password),
        flip_social_callback_to_register_logged_out);
    view_set_previous_callback(
        uart_text_input_get_view(app->text_input_logged_out_register_password_2),
        flip_social_callback_to_register_logged_out);
    //
    view_set_previous_callback(
        uart_text_input_get_view(app->text_input_logged_in_change_password),
        flip_social_callback_to_profile_logged_in);
    view_set_previous_callback(
        uart_text_input_get_view(app->text_input_logged_in_compose_pre_save_input),
        flip_social_callback_to_compose_logged_in);
    view_set_previous_callback(
        uart_text_input_get_view(app->text_input_logged_in_wifi_settings_ssid),
        flip_social_callback_to_wifi_settings_logged_in);
    view_set_previous_callback(
        uart_text_input_get_view(app->text_input_logged_in_wifi_settings_password),
        flip_social_callback_to_wifi_settings_logged_in);

    // Setup About(s)
    app->widget_logged_out_about = widget_alloc();
    app->widget_logged_in_about = widget_alloc();
    if(!app->widget_logged_out_about || !app->widget_logged_in_about) {
        FURI_LOG_E(TAG, "Failed to allocate Widget");
        return NULL;
    }
    widget_add_text_scroll_element(
        app->widget_logged_out_about,
        0,
        0,
        128,
        64,
        "Welcome to FlipSocial\n---\nThe social media app for\nFlipper Zero, created by\nJBlanked.\n---\nPress BACK to return.");
    widget_add_text_scroll_element(
        app->widget_logged_in_about,
        0,
        0,
        128,
        64,
        "Welcome to FlipSocial\n---\nThe social media app for\nFlipper Zero, created by\nJBlanked.\n---\nPress BACK to return.");
    view_set_previous_callback(
        widget_get_view(app->widget_logged_in_about), flip_social_callback_to_settings_logged_in);
    view_set_previous_callback(
        widget_get_view(app->widget_logged_out_about), flip_social_callback_to_submenu_logged_out);
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipSocialViewLoggedInSettingsAbout,
        widget_get_view(app->widget_logged_in_about));
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipSocialViewLoggedOutAbout,
        widget_get_view(app->widget_logged_out_about));

    // load the playlist
    if(load_playlist(&app->pre_saved_messages)) {
        // Update the submenu
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

        if(app->is_logged_in != NULL) {
            app->is_logged_in = "false";
        }
        app_instance = app;
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedOutSubmenu);
    } else {
        // set variable item text
        // Update variable items
        variable_item_set_current_value_text(
            app->variable_item_logged_in_wifi_settings_ssid, app->wifi_ssid_logged_in);
        variable_item_set_current_value_text(
            app->variable_item_logged_in_wifi_settings_password, app->wifi_password_logged_in);
        variable_item_set_current_value_text(
            app->variable_item_logged_out_wifi_settings_ssid, app->wifi_ssid_logged_out);
        variable_item_set_current_value_text(
            app->variable_item_logged_out_wifi_settings_password, app->wifi_password_logged_out);

        variable_item_set_current_value_text(
            app->variable_item_logged_out_login_username, app->login_username_logged_out);
        variable_item_set_current_value_text(
            app->variable_item_logged_out_login_password, app->login_password_logged_out);
        variable_item_set_current_value_text(
            app->variable_item_logged_in_profile_username, app->login_username_logged_in);
        variable_item_set_current_value_text(
            app->variable_item_logged_in_profile_change_password, app->change_password_logged_in);

        // Copy items into their temp buffers with safety checks
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
        if(app->change_password_logged_in && app->change_password_logged_in_temp_buffer) {
            strncpy(
                app->change_password_logged_in_temp_buffer,
                app->change_password_logged_in,
                app->change_password_logged_in_temp_buffer_size - 1);
            app->change_password_logged_in_temp_buffer
                [app->change_password_logged_in_temp_buffer_size - 1] = '\0';
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
            app->login_username_logged_in[app->login_username_logged_in_temp_buffer_size - 1] =
                '\0';
        }
        // if login password is empty but logged out isnt, copy it over
        if(strlen(app->login_password_logged_out) > 0 &&
           strlen(app->change_password_logged_in) == 0) {
            strncpy(
                app->change_password_logged_in,
                app->login_password_logged_out,
                app->login_password_logged_out_temp_buffer_size - 1);
            app->change_password_logged_in[app->login_password_logged_out_temp_buffer_size - 1] =
                '\0';
        }

        app_instance = app;
        if(app->is_logged_in != NULL && strcmp(app->is_logged_in, "true") == 0) {
            view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedInSubmenu);
        } else {
            view_dispatcher_switch_to_view(app->view_dispatcher, FlipSocialViewLoggedOutSubmenu);
        }
    }

    return app;
}

#endif // FLIP_SOCIAL_I
