#include "../app_user.h"

// LIST
typedef enum {
    SEND_MESSAGE,
    CHOOSE_ID,
    SET_ID,
    SET_DATA_LENGHT,
    SET_REQ,
    SET_DATA,
    SET_FIRST_DATA,
    SET_SECOND_DATA,
    SET_THIRD_DATA,
    SET_FOURTH_DATA,
    SET_FIFTH_DATA,
    SET_SIXTH_DATA,
    SET_SEVENTH_DATA,
    SET_EIGHTH_DATA
} list_of_items;

typedef enum {
    SEND_OK,
    SEND_ERROR,
} sender_status;

static int32_t sender_on_work(void* context) {
    App* app = context;
    app->mcp_can->mode = MCP_NORMAL;
    ERROR_CAN debug = ERROR_OK;
    ERROR_CAN error = ERROR_OK;
    debug = mcp2515_init(app->mcp_can);

    furi_delay_ms(10);

    if(debug == ERROR_OK) {
        error = send_can_frame(app->mcp_can, app->frame_to_send);
        furi_delay_ms(500);

        if(error != ERROR_OK)
            scene_manager_handle_custom_event(app->scene_manager, SEND_ERROR);
        else
            scene_manager_handle_custom_event(app->scene_manager, SEND_OK);
    } else {
        scene_manager_handle_custom_event(app->scene_manager, DEVICE_NO_CONNECTED);
    }

    free_mcp2515(app->mcp_can);
    return 0;
}

// Option callback using button OK
void callback_input_sender_options(void* context, uint32_t index) {
    App* app = context;
    app->sender_selected_item = index;
    switch(index) {
    case SEND_MESSAGE:
        scene_manager_next_scene(app->scene_manager, app_scene_send_message);
        break;
    case CHOOSE_ID:
        if(app->num_of_devices > 0) {
            scene_manager_next_scene(app->scene_manager, app_scene_id_list_option);
        } else {
            scene_manager_next_scene(app->scene_manager, app_scene_warning_log_sender);
        }
        break;
    case SET_ID:
        scene_manager_set_scene_state(app->scene_manager, app_scene_input_text_option, SET_ID);
        scene_manager_next_scene(app->scene_manager, app_scene_input_text_option);
        break;

    case SET_DATA:
        scene_manager_set_scene_state(app->scene_manager, app_scene_input_text_option, SET_DATA);
        scene_manager_next_scene(app->scene_manager, app_scene_input_text_option);
        break;

    case SET_FIRST_DATA:
        scene_manager_set_scene_state(
            app->scene_manager, app_scene_input_text_option, SET_FIRST_DATA);
        scene_manager_next_scene(app->scene_manager, app_scene_input_text_option);
        break;

    case SET_SECOND_DATA:
        scene_manager_set_scene_state(
            app->scene_manager, app_scene_input_text_option, SET_SECOND_DATA);
        scene_manager_next_scene(app->scene_manager, app_scene_input_text_option);
        break;

    case SET_THIRD_DATA:
        scene_manager_set_scene_state(
            app->scene_manager, app_scene_input_text_option, SET_THIRD_DATA);
        scene_manager_next_scene(app->scene_manager, app_scene_input_text_option);
        break;

    case SET_FOURTH_DATA:
        scene_manager_set_scene_state(
            app->scene_manager, app_scene_input_text_option, SET_FOURTH_DATA);
        scene_manager_next_scene(app->scene_manager, app_scene_input_text_option);
        break;

    case SET_FIFTH_DATA:
        scene_manager_set_scene_state(
            app->scene_manager, app_scene_input_text_option, SET_FIFTH_DATA);
        scene_manager_next_scene(app->scene_manager, app_scene_input_text_option);
        break;

    case SET_SIXTH_DATA:
        scene_manager_set_scene_state(
            app->scene_manager, app_scene_input_text_option, SET_SIXTH_DATA);
        scene_manager_next_scene(app->scene_manager, app_scene_input_text_option);
        break;

    case SET_SEVENTH_DATA:
        scene_manager_set_scene_state(
            app->scene_manager, app_scene_input_text_option, SET_SEVENTH_DATA);
        scene_manager_next_scene(app->scene_manager, app_scene_input_text_option);
        break;

    case SET_EIGHTH_DATA:
        scene_manager_set_scene_state(
            app->scene_manager, app_scene_input_text_option, SET_EIGHTH_DATA);
        scene_manager_next_scene(app->scene_manager, app_scene_input_text_option);
        break;

    default:
        break;
    }
}

// Options Callback
void callback_sender_options(VariableItem* item) {
    App* app = variable_item_get_context(item);
    uint8_t selected_index = variable_item_list_get_selected_item_index(app->varList);
    uint8_t index_item = variable_item_get_current_value_index(item);

    switch(selected_index) {
    case SET_DATA_LENGHT:
        furi_string_reset(app->text);
        furi_string_cat_printf(app->text, "%u", index_item);
        variable_item_set_current_value_text(item, furi_string_get_cstr(app->text));
        app->frame_to_send->data_lenght = index_item;
        break;
    case SET_REQ:
        furi_string_reset(app->text);
        furi_string_cat_printf(app->text, "%u", index_item);
        variable_item_set_current_value_text(item, furi_string_get_cstr(app->text));
        app->frame_to_send->req = index_item;
        break;
    default:
        break;
    }
}

// Sender Scene On enter
void app_scene_sender_on_enter(void* context) {
    App* app = context;
    VariableItem* item;

    uint8_t data_lenght = app->frame_to_send->data_lenght;
    uint32_t can_id = app->frame_to_send->canId;
    uint8_t request = app->frame_to_send->req;

    app->sender_id_compose[3] = can_id;
    app->sender_id_compose[2] = can_id >> 8;
    app->sender_id_compose[1] = can_id >> 16;
    app->sender_id_compose[0] = can_id >> 24;

    variable_item_list_reset(app->varList);

    // 0
    item = variable_item_list_add(app->varList, "SEND MESSAGE", 0, callback_sender_options, app);
    variable_item_set_current_value_index(item, 0);

    // 1
    item = variable_item_list_add(app->varList, "Choose Id", 0, callback_sender_options, app);
    variable_item_set_current_value_index(item, 0);
    variable_item_set_current_value_text(item, "SEARCH");

    // 2
    item = variable_item_list_add(app->varList, "Id", 0, callback_sender_options, app);
    variable_item_set_current_value_index(item, 0);

    furi_string_reset(app->text);
    furi_string_cat_printf(app->text, "0x%lx", can_id);
    variable_item_set_current_value_text(item, furi_string_get_cstr(app->text));

    // 3
    item = variable_item_list_add(app->varList, "DLC", 9, callback_sender_options, app);
    variable_item_set_current_value_index(item, data_lenght);

    furi_string_reset(app->text);
    furi_string_cat_printf(app->text, "%u", data_lenght);
    variable_item_set_current_value_text(item, furi_string_get_cstr(app->text));

    // 4
    item = variable_item_list_add(app->varList, "Request", 2, callback_sender_options, app);
    variable_item_set_current_value_index(item, request);

    furi_string_reset(app->text);
    furi_string_cat_printf(app->text, "%u", request);
    variable_item_set_current_value_text(item, furi_string_get_cstr(app->text));

    // 5
    item =
        variable_item_list_add(app->varList, "CLIC TO SET DATA", 0, callback_sender_options, app);
    variable_item_set_current_value_index(item, 0);

    // 6
    item = variable_item_list_add(app->varList, "BYTE [0]", 0, callback_sender_options, app);
    variable_item_set_current_value_index(item, 0);

    furi_string_reset(app->text);
    furi_string_cat_printf(app->text, "0x%x", app->frame_to_send->buffer[0]);
    variable_item_set_current_value_text(item, furi_string_get_cstr(app->text));

    // 7
    item = variable_item_list_add(app->varList, "BYTE [1]", 0, callback_sender_options, app);
    variable_item_set_current_value_index(item, 0);

    furi_string_reset(app->text);
    furi_string_cat_printf(app->text, "0x%x", app->frame_to_send->buffer[1]);
    variable_item_set_current_value_text(item, furi_string_get_cstr(app->text));

    // 8
    item = variable_item_list_add(app->varList, "BYTE [2]", 0, callback_sender_options, app);
    variable_item_set_current_value_index(item, 0);

    furi_string_reset(app->text);
    furi_string_cat_printf(app->text, "0x%x", app->frame_to_send->buffer[2]);
    variable_item_set_current_value_text(item, furi_string_get_cstr(app->text));

    // 9
    item = variable_item_list_add(app->varList, "BYTE [3]", 0, callback_sender_options, app);
    variable_item_set_current_value_index(item, 0);

    furi_string_reset(app->text);
    furi_string_cat_printf(app->text, "0x%x", app->frame_to_send->buffer[3]);
    variable_item_set_current_value_text(item, furi_string_get_cstr(app->text));

    // 10
    item = variable_item_list_add(app->varList, "BYTE [4]", 0, callback_sender_options, app);
    variable_item_set_current_value_index(item, 0);

    furi_string_reset(app->text);
    furi_string_cat_printf(app->text, "0x%x", app->frame_to_send->buffer[4]);
    variable_item_set_current_value_text(item, furi_string_get_cstr(app->text));

    // 11
    item = variable_item_list_add(app->varList, "BYTE [5]", 0, callback_sender_options, app);
    variable_item_set_current_value_index(item, 0);

    furi_string_reset(app->text);
    furi_string_cat_printf(app->text, "0x%x", app->frame_to_send->buffer[5]);
    variable_item_set_current_value_text(item, furi_string_get_cstr(app->text));

    // 12
    item = variable_item_list_add(app->varList, "BYTE [6]", 0, callback_sender_options, app);
    variable_item_set_current_value_index(item, 0);

    furi_string_reset(app->text);
    furi_string_cat_printf(app->text, "0x%x", app->frame_to_send->buffer[6]);
    variable_item_set_current_value_text(item, furi_string_get_cstr(app->text));

    // 13
    item = variable_item_list_add(app->varList, "BYTE [7]", 0, callback_sender_options, app);
    variable_item_set_current_value_index(item, 0);

    furi_string_reset(app->text);
    furi_string_cat_printf(app->text, "0x%x", app->frame_to_send->buffer[7]);
    variable_item_set_current_value_text(item, furi_string_get_cstr(app->text));

    variable_item_list_set_enter_callback(app->varList, callback_input_sender_options, app);
    variable_item_list_set_selected_item(app->varList, app->sender_selected_item);
    view_dispatcher_switch_to_view(app->view_dispatcher, VarListView);
}

bool app_scene_sender_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case SET_ID:
            scene_manager_next_scene(app->scene_manager, app_scene_input_text_option);
            consumed = true;
            break;

        default:
            break;
        }
    }
    return consumed;
}

void app_scene_sender_on_exit(void* context) {
    App* app = context;
    variable_item_list_reset(app->varList);
}

void app_scene_send_message_on_enter(void* context) {
    App* app = context;

    widget_reset(app->widget);
    widget_add_string_element(
        app->widget, 65, 32, AlignCenter, AlignCenter, FontPrimary, "Waiting to send...");

    view_dispatcher_switch_to_view(app->view_dispatcher, ViewWidget);

    app->thread = furi_thread_alloc_ex("Sender_on_work", 1024, sender_on_work, app);
    furi_thread_start(app->thread);
}

bool app_scene_send_message_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case SEND_OK:
            draw_send_ok(app);
            break;

        case SEND_ERROR:
            draw_send_wrong(app);
            break;

        case DEVICE_NO_CONNECTED:
            draw_device_no_connected(app);
            break;

        default:
            break;
        }
    }

    return consumed;
}

void app_scene_send_message_on_exit(void* context) {
    App* app = context;
    furi_thread_join(app->thread);
    furi_thread_free(app->thread);

    widget_reset(app->widget);
}

void app_scene_warning_log_on_enter(void* context) {
    App* app = context;
    widget_reset(app->widget);

    widget_add_string_element(
        app->widget, 65, 20, AlignCenter, AlignBottom, FontPrimary, "W A R N I N G !");

    widget_add_string_multiline_element(
        app->widget,
        65,
        35,
        AlignCenter,
        AlignCenter,
        FontSecondary,
        "First go to the Sniffing option\nin the menu and get the ID's");

    view_dispatcher_switch_to_view(app->view_dispatcher, ViewWidget);
}

bool app_scene_warning_log_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    bool consumed = false;

    return consumed;
}

void app_scene_warning_log_on_exit(void* context) {
    App* app = context;
    widget_reset(app->widget);
}

// ---------------------------- MENU OF THE CAN ID's ---------------------------

void menu_ids_callback(void* context, uint32_t index) {
    App* app = context;
    app->frame_to_send->canId = app->frameArray[index].canId;
    app->frame_to_send->data_lenght = app->frameArray[index].data_lenght;
    app->frame_to_send->ext = app->frameArray[index].ext;
    app->frame_to_send->req = app->frameArray[index].req;

    for(uint8_t i = 0; i < (app->frame_to_send->data_lenght); i++) {
        app->frame_to_send->buffer[i] = app->frameArray[index].buffer[i];
    }

    scene_manager_handle_custom_event(app->scene_manager, ReturnEvent);
}

void app_scene_id_list_on_enter(void* context) {
    App* app = context;
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "CAN ADDRESS TO SEND");

    for(uint8_t i = 0; i < app->num_of_devices; i++) {
        furi_string_reset(app->text);
        furi_string_cat_printf(app->text, "0x%lx", app->frameArray[i].canId);
        submenu_add_item(app->submenu, furi_string_get_cstr(app->text), i, menu_ids_callback, app);
    }

    view_dispatcher_switch_to_view(app->view_dispatcher, SubmenuView);
}

bool app_scene_id_list_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_previous_scene(app->scene_manager);
        consumed = true;
    }
    return consumed;
}

void app_scene_id_list_on_exit(void* context) {
    App* app = context;
    submenu_reset(app->submenu);
}

void input_byte_sender_callback(void* context) {
    App* app = context;
    app->frame_to_send->canId = app->sender_id_compose[3] | (app->sender_id_compose[2] << 8) |
                                (app->sender_id_compose[1] << 16) |
                                (app->sender_id_compose[0] << 24);
    scene_manager_handle_custom_event(app->scene_manager, ReturnEvent);
}

void app_scene_input_text_on_enter(void* context) {
    App* app = context;
    ByteInput* scene = app->input_byte_value;

    uint32_t state =
        scene_manager_get_scene_state(app->scene_manager, app_scene_input_text_option);

    app->sender_selected_item = state;

    switch(state) {
    case SET_ID:
        byte_input_set_result_callback(
            scene, input_byte_sender_callback, NULL, app, app->sender_id_compose, 4);
        byte_input_set_header_text(app->input_byte_value, "SET ADDRESS");
        break;

    case SET_DATA:
        byte_input_set_result_callback(
            scene, input_byte_sender_callback, NULL, app, app->frame_to_send->buffer, 8);
        byte_input_set_header_text(app->input_byte_value, "SET ALL DATA");
        break;

    default:
        if(state > 5) {
            byte_input_set_result_callback(
                scene,
                input_byte_sender_callback,
                NULL,
                app,
                &(app->frame_to_send->buffer[state - 6]),
                1);

            furi_string_reset(app->text);
            furi_string_cat_printf(app->text, "SET BYTE [%lu]", state - 6);
            byte_input_set_header_text(app->input_byte_value, furi_string_get_cstr(app->text));
        }
        break;
    }

    view_dispatcher_switch_to_view(app->view_dispatcher, InputByteView);
}

bool app_scene_input_text_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case ReturnEvent:
            scene_manager_previous_scene(app->scene_manager);
            break;

        default:
            break;
        }
    }
    return consumed;
}

void app_scene_input_text_on_exit(void* context) {
    UNUSED(context);
}
