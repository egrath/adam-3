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
