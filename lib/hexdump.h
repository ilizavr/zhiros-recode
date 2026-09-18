/*
 * Copyright (c) 2026 ilizavr & yellowhat
 * SPDX-License-Identifier: MIT
 */

static void hexdump(char *data, u32 size)
{
    u8 *ptr = (u8*)data;
    const char* hex_digits= "0123456789ABCDEF";

    for (u32 i = 0; i < size; i += 16)
    {
        u32 chunk = size - i;
        if (chunk > 16) chunk = 16;

        for (int j = 28; j >= 0; j -= 4)
        {
            putchar(hex_digits[(i >> j) & 0x0F]);
        }
        print(": ");

        for (u32 j = 0; j < 16; j++)
        {
            if (j < chunk)
            {
                u8 b = ptr[i + j];
                putchar(hex_digits[b >> 4]);
                putchar(hex_digits[b & 0x0F]);
            }
            else
            {
                print("  ");
            }
            putchar(' ');
            if (j == 7) print(" ");
        }

        print(" |");

        for (u32 j = 0; j < chunk; j++)
        {
            u8 b = ptr[i + j];
            if (b >= 32 && b <= 126)
            {
                putchar(b);
            }
            else
            {
                putchar('.');
            }
        }
        print("|\n");
    }
}
