#include "renderer.h"

SDL_Renderer *renderer;
SDL_Window *window;
SDL_Texture *framebuffer;
SDL_FRect framebufferRenderRect;

SDL_Texture *fontNormal;        /* this is the font normally used */
SDL_Texture *fontInverted;      /* this is for inverted text */
SDL_Texture *fontCursor;        /* this is just for where to cursor is */

/* render a single char to the proper position in our framebuffer using ADM-3A
   coordinates: row from 1 to 24 and column from 1 to 80 */ 
void render_char (unsigned char c, int y, int x)
{
    SDL_FRect sourceRect, destRect;

    /* where in the atlas is our glyph? */
    sourceRect.w = (float) 8;
    sourceRect.h = (float) 16;
    sourceRect.x = (float) ((c % 16 ) * 8);
    sourceRect.y = (float) ((c / 16 ) * 16);

    /* where do we have to place the glyph? */
    destRect.w = (float) 8;
    destRect.h = (float) 16;
    destRect.x = (float) ((x-1) * 8) + FRAMEBUFFER_BORDER_SIZE;
    destRect.y = (float) ((y-1) * 16) + FRAMEBUFFER_BORDER_SIZE;

    /* Blit the character to the screen */
    SDL_RenderTexture (renderer, fontNormal, &sourceRect, &destRect);
}

/* render the cursor at the current position. Very similar to rendering
   a regular character */
void render_cursor (void)
{
    SDL_FRect sourceRect, destRect;
    char c;

    /* which char is at the current cursor position? */
    c = buffer[cursor.y-1][cursor.x-1].content;

    /* where in the atlas is our glyph? */
    sourceRect.w = (float) 8;
    sourceRect.h = (float) 16;
    sourceRect.x = (float) ((c % 16 ) * 8);
    sourceRect.y = (float) ((c / 16 ) * 16);  

    /* where do we have to place the glyph? */
    destRect.w = (float) 8;
    destRect.h = (float) 16;
    destRect.x = (float) ((cursor.x-1) * 8) + FRAMEBUFFER_BORDER_SIZE;
    destRect.y = (float) ((cursor.y-1) * 16) + FRAMEBUFFER_BORDER_SIZE;

    /* Blit the character to the screen */
    SDL_RenderTexture (renderer, fontCursor, &sourceRect, &destRect);
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
    clearColor = renderer_hex_to_rgba (cmdline.terminalBackground);
    SDL_SetRenderDrawColor (renderer, clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    SDL_RenderClear (renderer);

    /* draw all the symbols in the terminal matrix */
    for (y = 0; y < TERM_ROWS; y ++)
    {
        for (x = 0; x < TERM_COLUMNS; x ++)
        {
            render_char (buffer[y][x].content, y+1, x+1);
        }
    }

    render_cursor ();

    /* Switch back the render target to the display */
    SDL_SetRenderTarget (renderer, NULL);

    /* profiling */
    /*
    end = SDL_GetPerformanceCounter();
    duration = ((end-start) * 1000) / SDL_GetPerformanceFrequency();

    fprintf (stdout, "render_terminal took: %lld ms\n", duration);
    */
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

    flags = SDL_WINDOW_RESIZABLE;
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

    SDL_SetRenderDrawColor (renderer, 0, 0, 0, 255);
    SDL_SetRenderVSync (renderer, SDL_RENDERER_VSYNC_DISABLED);

    /* create our framebuffer */
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

    /* load font atlas textures for the different font kinds used */
    #ifdef DEBUG
    fprintf (stdout, "DEBUG: terminal foreground=%08X background=%08X\n", cmdline.terminalForeground, cmdline.terminalBackground);
    #endif
    if ((fontNormal = load_fontatlas (cmdline.terminalForeground, cmdline.terminalBackground)) == NULL)
        return false;
    if ((fontInverted = load_fontatlas (cmdline.terminalBackground, cmdline.terminalForeground)) == NULL)
        return false;
    if ((fontCursor = load_fontatlas (cmdline.cursorForeground, cmdline.cursorBackground)) == NULL)
        return false;

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

SDL_Texture * load_fontatlas (uint32_t foreground, uint32_t background)
{
    SDL_Texture *atlas;
    FILE *input;
    uint32_t *pixels;
    int pitch;
    
    uint32_t glyphPixel;
    unsigned char c;

    uint64_t start, end, duration;

    start = SDL_GetPerformanceCounter ();

    atlas = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_XRGB8888, SDL_TEXTUREACCESS_STREAMING, 128, 256);
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
            glyphPixel = background;
        else if (c=='@')
            glyphPixel = foreground;
        else
            continue;

        *pixels = glyphPixel;
        pixels++;
    }

    fclose (input);
    SDL_UnlockTexture (atlas);

    #ifdef DEBUG
    SDL_Surface *s;
    if (!SDL_LockTextureToSurface (atlas, NULL, &s))
    {
        fprintf (stderr, "SDL_LockTextureToSurface failed: %s\n", SDL_GetError());
    }
    SDL_SavePNG (s, "test.png");
    #endif

    /* profiling */
    end = SDL_GetPerformanceCounter ();
    duration = (end - start)  / ( SDL_GetPerformanceFrequency() / 1000);
    
    #ifdef DEBUG
    fprintf (stdout, "DEBUG: loading of the font atlas took: %lld ms\n", duration);
    #endif

    return atlas;
}

/* render the framebuffer to the window */
void render_framebuffer (void)
{
    SDL_RenderClear (renderer);
    SDL_RenderTexture (renderer, framebuffer, NULL, &framebufferRenderRect);
    SDL_RenderPresent (renderer);
}

void render_cleanup (void)
{
    /* clean up */
    SDL_StopTextInput (window);
}
