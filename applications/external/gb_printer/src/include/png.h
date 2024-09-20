// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2024 KBEmbedded

#ifndef PNG_H
#define PNG_H

#pragma once

void* png_alloc(uint32_t width, uint32_t height);

/* Set up image */
void png_reset(void* png_handle, size_t px_w, size_t px_h);

void png_populate(void* png_handle, uint8_t* image_buf);

void png_palette_set(void* png_handle, uint8_t rgb[3][4]);

void png_free(void* png_handle);

uint8_t* png_buf_get(void* png_handle);

size_t png_len_get(void* png_handle);

#endif // PNG_H
