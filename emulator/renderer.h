#ifndef SDL_H
#define SDL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} RGBAColor;

RGBAColor renderer_hex_to_rgba (uint32_t argb);

#endif
