#include "renderer.h"

RGBAColor renderer_hex_to_rgba (uint32_t argb)
{
    RGBAColor color;

    color.a = (argb & 0xFF000000) >> 24;
    color.r = (argb & 0x00FF0000) >> 16;
    color.g = (argb & 0x0000FF00) >> 8;
    color.b = (argb & 0x000000FF);

    return color;
}
