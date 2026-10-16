/*
 * SDL3 based renderer.
 * general approach is to create a framebuffer which holds enough space for the 
 * 80x24 glyphs and a border. The content of the 80x24 matrix is rendererd
 * to this framebuffer at the display refresh rate and then rendered to
 * the display in a aspect correct manner
 */
#include "renderer.h"

SDL_Renderer *renderer;
SDL_Window *window;
SDL_Texture *framebuffer;
SDL_FRect framebufferRenderRect;

SDL_Texture *fontAtlas;     /* this stores our font */

bool render_blink_attribute = true;

/* render a single cell (a position in the 80x24 terminal matrix) including all 
   it's attributes to the framebuffer */
void render_cell (Cell *c, int row, int col)
{
    SDL_FRect sourceRect, destRect;
    bool is_cursor = false;
    RGBAColor render_fg, render_bg;

    /* map to either the regular glyph or the graphic drawing one */
    unsigned char glyph = ((c->state & CS_GRAPHICS) == CS_GRAPHICS) ? c->content+129 : c->content;

    /* is this cell the cursor? */
    if (cursor.y == row && cursor.x == col)
        is_cursor = true;

    /* where in the atlas is our glyph? */
    sourceRect.w = (float) 8;
    sourceRect.h = (float) 16;
    sourceRect.x = (float) ((glyph % 16 ) * 8);
    sourceRect.y = (float) ((glyph / 16 ) * 16);

    /* where do we have to place the glyph? */
    destRect.w = (float) 8;
    destRect.h = (float) 16;
    destRect.x = (float) ((col -1) * 8) + FRAMEBUFFER_BORDER_SIZE;
    destRect.y = (float) ((row -1) * 16) + FRAMEBUFFER_BORDER_SIZE;

    /* determine the colors we need to use */
    if (is_cursor)
    {
        render_fg = config.cursor_foreground;
        render_bg = config.cursor_background;
    }
    else
    {
        /* per default, we use the configured foreground and background colors */
        render_bg = config.terminal_background;
        render_fg = config.terminal_foreground;
        
        /* but, if we have reverse video, we switch that */
        if ((c->state & CS_INVERSE) == CS_INVERSE)
        {
            render_bg = config.terminal_foreground;
            render_fg = config.terminal_background;
        }
    }

    /* Draw the background of the cell */
    SDL_SetRenderDrawColor (renderer, render_bg.r, render_bg.g, render_bg.b, 255);
    SDL_RenderFillRect (renderer, &destRect);

    /* if cell has blinking attribute set, check if we should render it or not */
    if ((c->state & CS_BLINK) == CS_BLINK && !render_blink_attribute)
        return;

    /* Blit the (colored) glyph */
    SDL_SetTextureColorMod (fontAtlas, render_fg.r, render_fg.g, render_fg.b);
    SDL_RenderTexture (renderer, fontAtlas, &sourceRect, &destRect);

    /* Underline if necessary */
    if ((c->state & CS_UNDERLINE) == CS_UNDERLINE)
    {
        SDL_FRect barRect;
        barRect.x = destRect.x;
        barRect.y = destRect.y + destRect.h - 2;
        barRect.w = destRect.w;
        barRect.h = 1;
        SDL_SetRenderDrawColor (renderer, render_fg.r, render_fg.g, render_fg.b, 255);
        SDL_RenderFillRect (renderer, &barRect);
    }
}

/* render the content of the whole terminal buffer to the framebuffer */
void render_terminal (void)
{
    int x, y;
    RGBAColor clearColor;

    /* uint64_t start, end, duration;

    start = SDL_GetPerformanceCounter(); */

    /* We only set the render target once before rendering a whopping
       1920 cells to the framebuffer. Otherwise we have a performance
       issue */
    SDL_SetRenderTarget (renderer, framebuffer);

    /* clear the framebuffer first (because of the border) */
    clearColor = config.terminal_background;
    SDL_SetRenderDrawColor (renderer, clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    SDL_RenderClear (renderer);

    /* draw all the symbols in the terminal matrix */
    for (y = 0; y < TERM_ROWS; y ++)
    {
        for (x = 0; x < TERM_COLUMNS; x ++)
        {
            /* render a single cell. As always, we use logical coordinates for row, column */
            render_cell (&(buffer[y][x]), y+1, x+1);
        }
    }

    //render_cursor ();

    /* Switch back the render target to the display */
    SDL_SetRenderTarget (renderer, NULL);

    /* profiling */
    /*
    end = SDL_GetPerformanceCounter();
    duration = ((end-start) * 1000) / SDL_GetPerformanceFrequency();

    fprintf (stdout, "render_terminal took: %lld ms\n", duration);
    */
}

void render_print_info (void)
{
    const char *renderName, *driverName;
    SDL_GPUDevice *gpuDevice;

    renderName = SDL_GetRendererName (renderer);
    gpuDevice = SDL_GetGPURendererDevice (renderer);
    if (gpuDevice)
    {
        driverName = SDL_GetGPUDeviceDriver (gpuDevice);
        fprintf (stdout, "Renderer: %s, Driver: %s\n", renderName, driverName);
    }
    else
    {
        fprintf (stdout, "Renderer is not GPU accelerated!\n");
    }
}

bool render_initialize (void)
{
    SDL_WindowFlags flags;
    int fbWidth, fbHeight;

    /* what is the size of our framebuffer? It has to fit 80x24 characters with a size
       of 8x16 pixels and also has some space around for better visibility */
    fbWidth = (FONT_WIDTH * TERM_COLUMNS) + (2 * FRAMEBUFFER_BORDER_SIZE);
    fbHeight = (FONT_HEIGHT * TERM_ROWS) + (2 * FRAMEBUFFER_BORDER_SIZE);

    #ifdef DEBUG
    fprintf (stdout, "DEBUG: framebuffer size will be a total of %d x %d pixels\n", fbWidth, fbHeight);
    #endif

    if (SDL_InitSubSystem (SDL_INIT_VIDEO) == 0)
    {
        fprintf (stderr, "SDL_InitSubsystem failed: %s (%s, %d)\n", SDL_GetError(), __FILE__, __LINE__);
        return false;
    }

    /* create the window, if specified in fullscreen mode */
    flags = SDL_WINDOW_RESIZABLE;
    if (config.fullscreen)
        flags |= SDL_WINDOW_FULLSCREEN;
    if ((window = SDL_CreateWindow ("ADAM-3", fbWidth*2, fbHeight*2, flags)) == NULL)
    {
        fprintf (stderr, "SDL_CreateWindow failed: %s (%s, %d)\n", SDL_GetError(), __FILE__, __LINE__);
        return false;
    }
    SDL_ShowWindow (window);

    if ((renderer = SDL_CreateGPURenderer (NULL, window)) == NULL)
    {
        fprintf (stderr, "SDL_CreateGPURenderer failed: %s (%s, %d)\n", SDL_GetError(), __FILE__, __LINE__);
        return false;
    }

    /* print information about which render backend has been chosen by SDL */
    render_print_info ();

    SDL_SetRenderDrawColor (renderer, 0, 0, 0, 255);
    SDL_SetRenderVSync (renderer, SDL_RENDERER_VSYNC_DISABLED);

    /* create our framebuffer - we just use a XRGB8888 surface, we don't need a alpha channel here */
    framebuffer = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_XRGB8888, SDL_TEXTUREACCESS_TARGET, fbWidth, fbHeight);
    if (framebuffer == NULL)
    {
        fprintf (stderr, "SDL_CreateTexture failed: %s (%s, %d)\n", SDL_GetError(), __FILE__, __LINE__);
        return false;
    }
    if (SDL_SetTextureScaleMode (framebuffer, SDL_SCALEMODE_PIXELART) == 0)
    {
        fprintf (stderr, "SDL_SetTextureScaleMode failed: %s (%s, %d)\n", SDL_GetError(), __FILE__, __LINE__);
        return false;
    }

    SDL_StartTextInput (window);

    /* build the font atlas */
    if ((fontAtlas = build_fontatlas ()) == NULL)
    {
        #ifdef DEBUG
        fprintf (stderr, "DEBUG: failed to build font atlas\n");
        #endif
        return false;
    }

    return true;
}

/* recalculate the size of the framebuffer within the window */
void update_framebuffer_rect (void)
{
    int window_w, window_h;
    float texture_w, texture_h;
    float scale_w, scale_h, scale;

    #ifdef DEBUG
    fprintf (stdout, "DEBUG: update_framebuffer_rect ()\n");
    #endif

    SDL_GetWindowSizeInPixels (window, &window_w, &window_h);
    SDL_GetTextureSize (framebuffer, &texture_w, &texture_h);

    /* calculate scale factor */
    scale_w = (float) window_w / texture_w;
    scale_h = (float) window_h / texture_h;
    scale = (scale_w < scale_h) ? scale_w : scale_h;

    /* set destination size and position of renderered framebuffer */
    framebufferRenderRect.h = texture_h * scale;
    framebufferRenderRect.w = texture_w * scale;
    framebufferRenderRect.x = (window_w - framebufferRenderRect.w) / 2;
    framebufferRenderRect.y = (window_h - framebufferRenderRect.h) / 2;
}

/*
 * builds a white on transparent glyph atlas from the font dat file 
 */
SDL_Texture * build_fontatlas (void)
{
    SDL_Texture *atlas;
    FILE *input;
    uint32_t *pixels;
    int pitch;
    
    uint32_t glyphPixel;
    unsigned char c;

    uint64_t start, end, duration;

    start = SDL_GetPerformanceCounter ();

    /* we need a alpha channel, so we use a ARGB8888 surface */
    atlas = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 128, 256);
    if (atlas == NULL)
    {
        fprintf (stderr, "SDL_CreateTexture failed: %s (%s, %d)\n", SDL_GetError(), __FILE__, __LINE__);
        exit (1);
    }

    if ((input = fopen("font.dat","rb")) == NULL)
    {
        fprintf (stderr, "failed to load font atlas file (%s, %d)\n", __FILE__, __LINE__);
        exit (1);
    }

    SDL_LockTexture (atlas, NULL, (void *) &pixels, &pitch);

    while (!feof(input))
    {
        fread (&c, 1, 1, input);
        if (c=='.') 
            glyphPixel = 0x00000000;
        else if (c=='@')
            glyphPixel = 0xFFFFFFFF;
        else
            continue;

        *pixels = glyphPixel;
        pixels++;
    }

    fclose (input);
    SDL_UnlockTexture (atlas);
    SDL_SetTextureBlendMode (atlas, SDL_BLENDMODE_BLEND);

    #ifdef DEBUG
    SDL_Surface *s;
    if (!SDL_LockTextureToSurface (atlas, NULL, &s))
    {
        fprintf (stderr, "SDL_LockTextureToSurface failed: %s\n", SDL_GetError());
    }
    SDL_SavePNG (s, "font.png");
    #endif

    /* profiling */
    end = SDL_GetPerformanceCounter ();
    duration = (end - start)  / ( SDL_GetPerformanceFrequency() / 1000);
    
    #ifdef DEBUG
    fprintf (stdout, "DEBUG: loading of the font atlas took: %lld ms\n", duration);
    #endif

    return atlas;
}

// This is called at a frequency of 1/2 hz to update the blinking text state
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
uint32_t blinky_callback (void *userdata, SDL_TimerID timerID, Uint32 interval)
{
    /* toggle blink text rendering */
    render_blink_attribute = !render_blink_attribute;

    /* call again in 500 ms */
    return 500;
}
#pragma clang diagnostic pop

/* render the framebuffer to the window */
void render_framebuffer (void)
{
    /* set draw color and clear the renderer (e.g. clear the window)  */
    SDL_SetRenderDrawColor (renderer, config.terminal_background.r, config.terminal_background.g, config.terminal_background.b, 255);
    SDL_RenderClear (renderer);

    /* now render our framebuffer texture to the display and show it */
    SDL_RenderTexture (renderer, framebuffer, NULL, &framebufferRenderRect);
    SDL_RenderPresent (renderer);
}

void render_cleanup (void)
{
    /* clean up */
    SDL_StopTextInput (window);
}

/* enable / disable fullscreen mode */
void render_set_fullscreen (bool fullscreen)
{
    SDL_SetWindowFullscreen (window, fullscreen);
}
