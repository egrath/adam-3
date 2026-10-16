#ifndef HELPER_H
#define HELPER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} RGBAColor;

RGBAColor uint32_to_rgba (uint32_t argb);
uint32_t rgba_to_uint32 (RGBAColor color);

uint32_t str_to_uint32 (const char *str);
bool str_to_bool (const char *str);

#endif
