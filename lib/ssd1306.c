/*
MIT License

Copyright (c) 2021 David Schramm

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/**
 * @file ssd1306.c
 * @brief Implementación del controlador para pantallas OLED SSD1306 vía I2C.
 *
 * Este módulo contiene la implementación del controlador para pantallas OLED
 * monocromáticas SSD1306, incluyendo funciones de inicialización, manipulación
 * de píxeles, líneas, texto, figuras y bitmaps.
 *
 * Adaptado e integrado por **Duván Felipe Vélez Restrepo** para su uso en el
 * proyecto **Piscitec**, donde se utiliza para visualizar variables clave del sistema.
 */

#include <pico/stdlib.h>
#include <hardware/i2c.h>
#include <pico/binary_info.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ssd1306.h"
#include "font.h"

/// @brief Intercambia dos enteros por referencia.
inline static void swap(int32_t *a, int32_t *b) {
    int32_t *t = a;
    *a = *b;
    *b = *t;
}

/// @brief Envía datos por I2C con mensajes de error.
inline static void fancy_write(i2c_inst_t *i2c, uint8_t addr, const uint8_t *src, size_t len, char *name) {
    switch (i2c_write_blocking(i2c, addr, src, len, false)) {
        case PICO_ERROR_GENERIC:
            printf("[%s] addr not acknowledged!\n", name);
            break;
        case PICO_ERROR_TIMEOUT:
            printf("[%s] timeout!\n", name);
            break;
        default:
            break;
    }
}

/// @brief Envía un byte de comando al SSD1306.
inline static void ssd1306_write(ssd1306_t *p, uint8_t val) {
    uint8_t d[2] = {0x00, val};
    fancy_write(p->i2c_i, p->address, d, 2, "ssd1306_write");
}

bool ssd1306_init(ssd1306_t *p, uint16_t width, uint16_t height, uint8_t address, i2c_inst_t *i2c_instance) {
    p->width = width;
    p->height = height;
    p->pages = height / 8;
    p->address = address;
    p->i2c_i = i2c_instance;
    p->bufsize = p->pages * p->width;

    if ((p->buffer = malloc(p->bufsize + 1)) == NULL) {
        p->bufsize = 0;
        return false;
    }

    ++(p->buffer);  // espacio previo reservado para control (0x40)

    // Configuración inicial del SSD1306
    uint8_t cmds[] = {
        SET_DISP,
        SET_DISP_CLK_DIV, 0x80,
        SET_MUX_RATIO, height - 1,
        SET_DISP_OFFSET, 0x00,
        SET_DISP_START_LINE,
        SET_CHARGE_PUMP, p->external_vcc ? 0x10 : 0x14,
        SET_SEG_REMAP | 0x01,
        SET_COM_OUT_DIR | 0x08,
        SET_COM_PIN_CFG, width > 2 * height ? 0x02 : 0x12,
        SET_CONTRAST, 0xFF,
        SET_PRECHARGE, p->external_vcc ? 0x22 : 0xF1,
        SET_VCOM_DESEL, 0x30,
        SET_ENTIRE_ON,
        SET_NORM_INV,
        SET_DISP | 0x01,
        SET_MEM_ADDR, 0x00
    };

    for (size_t i = 0; i < sizeof(cmds); ++i)
        ssd1306_write(p, cmds[i]);

    return true;
}

inline void ssd1306_deinit(ssd1306_t *p) {
    free(p->buffer - 1);
}

inline void ssd1306_poweroff(ssd1306_t *p) {
    ssd1306_write(p, SET_DISP | 0x00);
}

inline void ssd1306_poweron(ssd1306_t *p) {
    ssd1306_write(p, SET_DISP | 0x01);
}

inline void ssd1306_contrast(ssd1306_t *p, uint8_t val) {
    ssd1306_write(p, SET_CONTRAST);
    ssd1306_write(p, val);
}

inline void ssd1306_invert(ssd1306_t *p, uint8_t inv) {
    ssd1306_write(p, SET_NORM_INV | (inv & 1));
}

inline void ssd1306_clear(ssd1306_t *p) {
    memset(p->buffer, 0, p->bufsize);
}

void ssd1306_clear_pixel(ssd1306_t *p, uint32_t x, uint32_t y) {
    if (x >= p->width || y >= p->height) return;
    p->buffer[x + p->width * (y >> 3)] &= ~(0x1 << (y & 0x07));
}

void ssd1306_draw_pixel(ssd1306_t *p, uint32_t x, uint32_t y) {
    if (x >= p->width || y >= p->height) return;
    p->buffer[x + p->width * (y >> 3)] |= 0x1 << (y & 0x07);
}

void ssd1306_draw_line(ssd1306_t *p, int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
    if (x1 > x2) {
        swap(&x1, &x2);
        swap(&y1, &y2);
    }

    if (x1 == x2) {
        if (y1 > y2) swap(&y1, &y2);
        for (int32_t i = y1; i <= y2; ++i)
            ssd1306_draw_pixel(p, x1, i);
        return;
    }

    float m = (float)(y2 - y1) / (float)(x2 - x1);
    for (int32_t i = x1; i <= x2; ++i) {
        float y = m * (i - x1) + y1;
        ssd1306_draw_pixel(p, i, (uint32_t)y);
    }
}

void ssd1306_clear_square(ssd1306_t *p, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    for (uint32_t i = 0; i < width; ++i)
        for (uint32_t j = 0; j < height; ++j)
            ssd1306_clear_pixel(p, x + i, y + j);
}

void ssd1306_draw_square(ssd1306_t *p, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    for (uint32_t i = 0; i < width; ++i)
        for (uint32_t j = 0; j < height; ++j)
            ssd1306_draw_pixel(p, x + i, y + j);
}

void ssd1306_draw_empty_square(ssd1306_t *p, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    ssd1306_draw_line(p, x, y, x + width, y);
    ssd1306_draw_line(p, x, y + height, x + width, y + height);
    ssd1306_draw_line(p, x, y, x, y + height);
    ssd1306_draw_line(p, x + width, y, x + width, y + height);
}

void ssd1306_draw_char_with_font(ssd1306_t *p, uint32_t x, uint32_t y, uint32_t scale, const uint8_t *font, char c) {
    if (c < font[3] || c > font[4]) return;

    uint32_t parts = (font[0] >> 3) + ((font[0] & 7) > 0);
    for (uint8_t w = 0; w < font[1]; ++w) {
        uint32_t ptr = (c - font[3]) * font[1] * parts + w * parts + 5;
        for (uint32_t l = 0; l < parts; ++l) {
            uint8_t line = font[ptr++];
            for (int8_t j = 0; j < 8; ++j, line >>= 1) {
                if (line & 1)
                    ssd1306_draw_square(p, x + w * scale, y + ((l << 3) + j) * scale, scale, scale);
            }
        }
    }
}

void ssd1306_draw_string_with_font(ssd1306_t *p, uint32_t x, uint32_t y, uint32_t scale, const uint8_t *font, const char *s) {
    for (int32_t x_n = x; *s; x_n += (font[1] + font[2]) * scale)
        ssd1306_draw_char_with_font(p, x_n, y, scale, font, *(s++));
}

void ssd1306_draw_char(ssd1306_t *p, uint32_t x, uint32_t y, uint32_t scale, char c) {
    ssd1306_draw_char_with_font(p, x, y, scale, font_8x5, c);
}

void ssd1306_draw_string(ssd1306_t *p, uint32_t x, uint32_t y, uint32_t scale, const char *s) {
    ssd1306_draw_string_with_font(p, x, y, scale, font_8x5, s);
}

static inline uint32_t ssd1306_bmp_get_val(const uint8_t *data, const size_t offset, uint8_t size) {
    switch (size) {
        case 1: return data[offset];
        case 2: return data[offset] | (data[offset + 1] << 8);
        case 4: return data[offset] | (data[offset + 1] << 8) | (data[offset + 2] << 16) | (data[offset + 3] << 24);
        default: __builtin_unreachable();
    }
}

void ssd1306_bmp_show_image_with_offset(ssd1306_t *p, const uint8_t *data, const long size, uint32_t x_offset, uint32_t y_offset) {
    if (size < 54) return;

    const uint32_t offset = ssd1306_bmp_get_val(data, 10, 4);
    const uint32_t width = ssd1306_bmp_get_val(data, 18, 4);
    const int32_t height = (int32_t)ssd1306_bmp_get_val(data, 22, 4);
    const uint16_t bit_count = (uint16_t)ssd1306_bmp_get_val(data, 28, 2);
    const uint32_t compression = ssd1306_bmp_get_val(data, 30, 4);
    if (bit_count != 1 || compression != 0) return;

    const int table_start = 14 + ssd1306_bmp_get_val(data, 14, 4);
    uint8_t color_val = 0;
    for (uint8_t i = 0; i < 2; ++i) {
        if (!(data[table_start + i * 4] | data[table_start + i * 4 + 1] | data[table_start + i * 4 + 2])) {
            color_val = i;
            break;
        }
    }

    uint32_t bpl = (width / 8) + (width & 7 ? 1 : 0);
    if (bpl & 3) bpl = (bpl ^ (bpl & 3)) + 4;
    const uint8_t *img_data = data + offset;

    int32_t step = height > 0 ? -1 : 1;
    int32_t end = height > 0 ? -1 : -height;

    for (uint32_t y = height > 0 ? height - 1 : 0; y != (uint32_t)end; y += step) {
        for (uint32_t x = 0; x < width; ++x) {
            if (((img_data[x >> 3] >> (7 - (x & 7))) & 1) == color_val)
                ssd1306_draw_pixel(p, x_offset + x, y_offset + y);
        }
        img_data += bpl;
    }
}

inline void ssd1306_bmp_show_image(ssd1306_t *p, const uint8_t *data, const long size) {
    ssd1306_bmp_show_image_with_offset(p, data, size, 0, 0);
}

void ssd1306_show(ssd1306_t *p) {
    uint8_t payload[] = {SET_COL_ADDR, 0, p->width - 1, SET_PAGE_ADDR, 0, p->pages - 1};
    if (p->width == 64) {
        payload[1] += 32;
        payload[2] += 32;
    }

    for (size_t i = 0; i < sizeof(payload); ++i)
        ssd1306_write(p, payload[i]);

    *(p->buffer - 1) = 0x40;
    fancy_write(p->i2c_i, p->address, p->buffer - 1, p->bufsize + 1, "ssd1306_show");
}
