// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2024 KBEmbedded

#ifndef FGP_APP_H
#define FGP_APP_H

#pragma once

// Libraries
#include <gblink/include/gblink.h>

// FURI
#include <furi.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/variable_item_list.h>
#include <storage/storage.h>
#include <src/include/fgp_palette.h>

struct plte {
    uint32_t data_len; // Longitud de los datos de la paleta (en bytes)
    uint8_t type[4]; // Tipo de chunk PLTE ('P', 'L', 'T', 'E')
    uint8_t color[4][3]; // Colores en formato RGB, 4 colores con 3 bytes cada uno
};

struct fgp_app {
    ViewDispatcher* view_dispatcher;

    // Scenes
    SceneManager* scene_manager;
    Submenu* submenu;
    VariableItemList* variable_item_list;
    void* receive_view;

    Storage* storage;

    void* printer_handle;

    bool add_header;
    unsigned int palette_idx;
};

typedef enum {
    fgpViewSubmenu,
    fgpViewVariableItemList,
    fgpViewReceive,
} fgpView;

#endif // FGP_APP_H
