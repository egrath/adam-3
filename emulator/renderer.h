#ifndef SDL_H
#define SDL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL3/SDL.h>

#include "adm3a.h"
#include "helper.h"
#include "config.h"

#define FRAMEBUFFER_BORDER_SIZE     4              /* border on every side; it's in client space, so it scales */

#define FONT_WIDTH                  8
#define FONT_HEIGHT                 16

bool render_initialize (void);
void render_cleanup (void);

void render_set_fullscreen (bool fullscreen);

void render_cell (Cell *c, int row, int col);
void render_terminal (void);
void render_framebuffer (void);
void update_framebuffer_rect (void);

uint32_t text_blink_callback (void *userdata, SDL_TimerID timerID, Uint32 interval);
uint32_t cursor_blink_callback (void *userdata, SDL_TimerID timerID, Uint32 interval);

void render_print_info (void);

SDL_Texture * build_fontatlas (void);

#endif
