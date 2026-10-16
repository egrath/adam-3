#include "helper.h"

RGBAColor uint32_to_rgba (uint32_t argb)
{
    RGBAColor color;

    color.a = (argb & 0xFF000000) >> 24;
    color.r = (argb & 0x00FF0000) >> 16;
    color.g = (argb & 0x0000FF00) >> 8;
    color.b = (argb & 0x000000FF);

    return color;
}

uint32_t rgba_to_uint32 (RGBAColor color)
{
    uint32_t argb;

    argb = 0x00;
    argb |= (color.a << 24);
    argb |= (color.r << 16);
    argb |= (color.g << 8);
    argb |= (color.b);
    
    return argb;
}

uint32_t str_to_uint32 (const char *str)
{
    uint32_t num;

    if (str != NULL)
    {
        if (sscanf (str, "0x%08X", &num)==0)
        {
            fprintf (stdout, "warning: conversion of %s to uint32 failed\n", str);
            return 0;
        }
        else
            return num;
    }

    fprintf (stdout, "warning: conversion of input string to uint32 failed because input is NULL\n");
    return 0;
}

bool str_to_bool (const char *str)
{
    if (str == NULL)
        return false;
    else
    {
        if (strcasecmp (str, "true") == 0)
            return true;
        else if (strcasecmp (str, "false") == 0)
            return false;
    }

    return false;
}
