#ifndef HELPER_H
#define HELPER_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>

/* Windows specific */
#ifdef _WIN32
    #include <windows.h>
    #define realpath(N, R) _fullpath((R), (N), PATH_MAX)
#endif

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

char * build_absolute_path (char *relative);

#endif
