/*
 * Copyright (c) 2026 ilizavr & yellowhat
 * SPDX-License-Identifier: MIT
 */
//код написан gemini 3.1 pro для теста


#include "lib/zhirtypes.h"
#include "lib/string.h"

u32 (*__getchar)();
void (*__printf)(char* fmt, ...);
bool *(*__get_key_state_matrix)();

struct fb_info* (*__fbcon_stop)();
struct fb_info* fb;

// Шрифт 8x8 (Цифры, буквы, знаки)
static const u8 font8x8_basic[128][8] = {
    [' '] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    [':'] = {0x00, 0x36, 0x36, 0x00, 0x00, 0x36, 0x36, 0x00},
    ['0'] = {0x3C, 0x66, 0x6E, 0x76, 0x66, 0x66, 0x3C, 0x00},
    ['1'] = {0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00},
    ['2'] = {0x3C, 0x66, 0x06, 0x0C, 0x18, 0x30, 0x7E, 0x00},
    ['3'] = {0x3C, 0x66, 0x06, 0x1C, 0x06, 0x66, 0x3C, 0x00},
    ['4'] = {0x0C, 0x1C, 0x3C, 0x6C, 0x7E, 0x0C, 0x0C, 0x00},
    ['5'] = {0x7E, 0x60, 0x7C, 0x06, 0x06, 0x66, 0x3C, 0x00},
    ['6'] = {0x3C, 0x66, 0x60, 0x7C, 0x66, 0x66, 0x3C, 0x00},
    ['7'] = {0x7E, 0x66, 0x0C, 0x18, 0x18, 0x18, 0x18, 0x00},
    ['8'] = {0x3C, 0x66, 0x66, 0x3C, 0x66, 0x66, 0x3C, 0x00},
    ['9'] = {0x3C, 0x66, 0x66, 0x3E, 0x06, 0x66, 0x3C, 0x00},
    ['S'] = {0x3C, 0x66, 0x60, 0x3C, 0x06, 0x66, 0x3C, 0x00},
    ['C'] = {0x3C, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3C, 0x00},
    ['O'] = {0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00},
    ['R'] = {0x7C, 0x66, 0x66, 0x7C, 0x6C, 0x66, 0x63, 0x00},
    ['E'] = {0x7E, 0x60, 0x60, 0x78, 0x60, 0x60, 0x7E, 0x00}
};

void put_pixel(u32 x, u32 y, u32 color) {
    if (x >= fb->screen_width || y >= fb->screen_height) return;
    volatile u32 *pixel_addr = (volatile u32*)(fb->fb_addr + y * fb->screen_pitch + x * fb->bpp);
    *pixel_addr = color;
}

void draw_rect(s32 startx, s32 endx, s32 starty, s32 endy, u32 color) {
    if (startx < 0) startx = 0;
    if (starty < 0) starty = 0;
    if (endx > (s32)fb->screen_width) endx = fb->screen_width;
    if (endy > (s32)fb->screen_height) endy = fb->screen_height;

    for (s32 y = starty; y < endy; y++) {
        for (s32 x = startx; x < endx; x++) {
            put_pixel((u32)x, (u32)y, color);
        }
    }
}

void draw_char(char c, u32 x, u32 y, u32 color, u32 scale) {
    u8 ascii = (u8)c;
    if (ascii > 127) return;

    for (u32 row = 0; row < 8; row++) {
        u8 icon = font8x8_basic[ascii][row];
        for (u32 col = 0; col < 8; col++) {
            if (icon & (1 << (7 - col))) {
                draw_rect(x + col * scale, x + (col + 1) * scale,
                          y + row * scale, y + (row + 1) * scale, color);
            }
        }
    }
}

void draw_string(const char *str, u32 x, u32 y, u32 color, u32 scale) {
    u32 cur_x = x;
    while (*str) {
        draw_char(*str, cur_x, y, color, scale);
        cur_x += 8 * scale + scale;
        str++;
    }
}

void draw_num(u32 num, u32 x, u32 y, u32 color, u32 scale) {
    char buf[12];
    s32 i = 0;
    if (num == 0) {
        buf[i++] = '0';
    } else {
        char temp[12];
        s32 t = 0;
        while (num > 0) {
            temp[t++] = '0' + (num % 10);
            num /= 10;
        }
        while (t > 0) buf[i++] = temp[--t];
    }
    buf[i] = '\0';
    draw_string(buf, x, y, color, scale);
}

bool check_collision(s32 ball_x, u32 ball_size, s32 paddle_x, u32 paddle_w) {
    return (ball_x + (s32)ball_size >= paddle_x) && (ball_x <= paddle_x + (s32)paddle_w);
}

INIT void init(void* _resolve_function(char* name))
{
    __getchar = _resolve_function("_getchar");
    __printf = _resolve_function("_printf");
    __fbcon_stop = _resolve_function("_fbcon_stop");
    __get_key_state_matrix = _resolve_function("_get_key_state_matrix");

    bool *key_state_matrix = __get_key_state_matrix();

    u8 wcode_a = ascii_to_keycode['a'];
    u8 wcode_d = ascii_to_keycode['d'];
    u8 wcode_j = ascii_to_keycode['j'];
    u8 wcode_l = ascii_to_keycode['l'];

    fb = __fbcon_stop();
    memset((u32*)fb->fb_addr, 0, fb->screen_height * fb->screen_pitch);

    // Параметры объектов
    u32 paddle_w = 120;
    u32 paddle_h = 10;
    u32 paddle_speed = 4;
    s32 x_bottom = (fb->screen_width / 2) - (paddle_w / 2);
    s32 y_bottom = fb->screen_height - 30;

    s32 x_top = (fb->screen_width / 2) - (paddle_w / 2);
    s32 y_top = 30;

    u32 ball_size = 12;
    s32 cube_x = fb->screen_width / 2;
    s32 cube_y = fb->screen_height / 2;
    s32 dx = 2; // Шаг 2px для большей плавности
    s32 dy = -2;

    u32 score_p1 = 0;
    u32 score_p2 = 0;

    draw_string("SCORE", 10, 10, 0x555555, 1);

    while (1) {
        // 1. СТИРАНИЕ ТЕКУЩИХ ПОЗИЦИЙ С ЗАПАСОМ (+5px)
        // Гарантированно уничтожает любые остаточные пиксели шлейфа
        draw_rect(cube_x - 5, cube_x + ball_size + 5, cube_y - 5, cube_y + ball_size + 5, 0x000000);
        draw_rect(x_bottom - 8, x_bottom + paddle_w + 8, y_bottom - 2, y_bottom + paddle_h + 2, 0x000000);
        draw_rect(x_top - 8, x_top + paddle_w + 8, y_top - 2, y_top + paddle_h + 2, 0x000000);

        // 2. ОБРАБОТКА ВВОДА
        if (key_state_matrix[wcode_a] && x_bottom >= (s32)paddle_speed) {
            x_bottom -= paddle_speed;
        }
        if (key_state_matrix[wcode_d] && (x_bottom + paddle_w + paddle_speed) < fb->screen_width) {
            x_bottom += paddle_speed;
        }

        if (key_state_matrix[wcode_j] && x_top >= (s32)paddle_speed) {
            x_top -= paddle_speed;
        }
        if (key_state_matrix[wcode_l] && (x_top + paddle_w + paddle_speed) < fb->screen_width) {
            x_top += paddle_speed;
        }

        // 3. РАСЧЕТ ДВИЖЕНИЯ И ФИЗИКИ
        cube_x += dx;
        cube_y += dy;

        // Отскок от боковых стен
        if (cube_x <= 0) {
            cube_x = 0;
            dx = -dx;
        } else if ((u32)(cube_x + ball_size) >= fb->screen_width) {
            cube_x = fb->screen_width - ball_size;
            dx = -dx;
        }

        // Верхняя ракетка
        if (cube_y <= (s32)(y_top + paddle_h) && (cube_y + (s32)ball_size) >= y_top) {
            if (check_collision(cube_x, ball_size, x_top, paddle_w)) {
                dy = -dy;
                cube_y = y_top + paddle_h;
            }
        }

        // Нижняя ракетка
        if ((cube_y + ball_size) >= y_bottom && cube_y <= (s32)(y_bottom + paddle_h)) {
            if (check_collision(cube_x, ball_size, x_bottom, paddle_w)) {
                dy = -dy;
                cube_y = y_bottom - ball_size;
            }
        }

        // Гол
        if (cube_y <= 0) {
            score_p2++;
            cube_x = fb->screen_width / 2;
            cube_y = fb->screen_height / 2;
            dy = -dy;
        } else if (cube_y >= (s32)fb->screen_height) {
            score_p1++;
            cube_x = fb->screen_width / 2;
            cube_y = fb->screen_height / 2;
            dy = -dy;
        }

        // 4. ОТРИСОВКА НОВЫХ ПОЗИЦИЙ
        draw_rect(x_bottom, x_bottom + paddle_w, y_bottom, y_bottom + paddle_h, 0xFFFFFF);
        draw_rect(x_top, x_top + paddle_w, y_top, y_top + paddle_h, 0xFFFFFF);
        draw_rect(cube_x, cube_x + ball_size, cube_y, cube_y + ball_size, 0xFFFFFF);

        // Счет
        draw_rect(70, 150, 10, 26, 0x000000);
        draw_num(score_p1, 70, 10, 0x00FF00, 2);
        draw_string(":", 90, 10, 0xFFFFFF, 2);
        draw_num(score_p2, 105, 10, 0x00FF00, 2);

        // Задержка кадра
        for (volatile u32 i = 0; i < 5000000; i++) {
            asm volatile("");
        }
    }
}
