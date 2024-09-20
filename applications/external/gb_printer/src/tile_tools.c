// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2024 KBEmbedded

#include <stddef.h>
#include <stdint.h>

/* Copy src to dst converting from gb tile format to scanline format for
 * standard image data.
 */
void tile_to_scanline(uint8_t* dst, uint8_t* src, size_t tiles_w, size_t tiles_h) {
    //size_t tile_w = 2; // 2 byte wide
    size_t tile_h = 8; // 8 byte tall
    size_t tile_x_max = tiles_w;
    size_t tile_y_max = tiles_h;
    size_t tile_x = 0;
    size_t tile_y = 0;
    size_t tile_suby = 0;
    size_t tmp;
    int i;

    for(tile_y = 0; tile_y < tile_y_max; tile_y++) {
        for(tile_suby = 0; tile_suby < tile_h; tile_suby++) {
            for(tile_x = 0; tile_x < tile_x_max; tile_x++) {
                /* need to shuffle bits */
                /* from: 0 1 2 3 4 5 6 7  8 9 a b c d e f
				 *   to: 8 0 9 1 a 2 b 3  c 4 d 5 e 6 f 7
				 */
                tmp = (tile_x * 16) + (tile_y * 320) + (tile_suby * 2);
                *dst = 0;
                for(i = 0; i < 8; i++) {
                    *dst <<= 1;
                    *dst |= !!(src[tmp + 1] & (0x80 >> i));
                    *dst <<= 1;
                    *dst |= !!(src[tmp] & (0x80 >> i));
                    if(i == 3) {
                        dst++;
                        *dst = 0;
                    }
                }
                dst++;
            }
        }
    }
}
