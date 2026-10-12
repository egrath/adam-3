#ifndef SDL_H
#define SDL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL3/SDL.h>

#include "adm3a.h"
#include "commandline.h"
#include "helper.h"

#define FRAMEBUFFER_BORDER_SIZE     4              /* border on every side; it's in client space, so it scales */

#define FONT_WIDTH                  8
#define FONT_HEIGHT                 16

bool render_initialize (void);
void render_cleanup (void);

void render_char (unsigned char c, int y, int x);
void render_cursor (void);
void render_terminal (void);
void render_framebuffer (void);
void update_framebuffer_rect (void);

SDL_Texture * load_fontatlas (uint32_t foreground, uint32_t background);

#endif
