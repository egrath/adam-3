#include <stdio.h>
#include <stdlib.h>

#include <SDL3/SDL.h>

#include "adm3a.h"

SDL_Renderer *renderer;
SDL_Window *window;
SDL_Texture *framebuffer;
SDL_FRect framebufferRenderRect;

SDL_Texture *fontNormal;

SDL_Rect cursorPosition;

void update_framebuffer_rect (void)
{
    int window_w, window_h;
    float texture_w, texture_h;
    float scale_w, scale_h, scale;

    fprintf (stdout, "update_framebuffer_rect ()\n");

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

SDL_Texture * load_fontatlas (void)
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
        fprintf (stderr, "SDL_CreateTexture failed: %s\n", SDL_GetError());
        exit (1);
    }

    if ((input = fopen("font.dat","rb")) == NULL)
    {
        fprintf (stderr, "failed to load font atlas file\n");
        exit (1);
    }

    SDL_LockTexture (atlas, NULL, (void *) &pixels, &pitch);

    while (!feof(input))
    {
        fread (&c, 1, 1, input);
        if (c=='.') 
            glyphPixel = 0x00000000;
        else if (c=='@')
            glyphPixel = 0xFF81FF81;
        else
            continue;

        *pixels = glyphPixel;
        pixels++;
    }

    fclose (input);
    SDL_UnlockTexture (atlas);

#if defined(DEBUG)
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
    fprintf (stdout, "loading of the font atlas took: %lld ms\n", duration);

    return atlas;
}

/* render a single char to the proper position in our framebuffer using ADM-3A
   coordinates: row from 1 to 25 and column from 1 to 80 */ 
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
    destRect.x = (float) ((x-1) * 8);
    destRect.y = (float) ((y-1) * 16);

    /* Blit the character to the screen */
    SDL_RenderTexture (renderer, fontNormal, &sourceRect, &destRect);
}

/* render the content of the whole terminal buffer to the framebuffer */
void render_terminal (void)
{
    int x, y;
    /* uint64_t start, end, duration;

    start = SDL_GetPerformanceCounter(); */

    /* We only set the render target once before rendering a whopping
       1920 cells to the framebuffer. Otherwise we have a performance
       issue */
    SDL_SetRenderTarget (renderer, framebuffer);

    for (y = 0; y < TERM_ROWS; y ++)
    {
        for (x = 0; x < TERM_COLUMNS; x ++)
        {
            render_char (buffer[y][x].content, y+1, x+1);
        }
    }

    /* Switch back the render target to the display */
    SDL_SetRenderTarget (renderer, NULL);

    /* profiling */
    /*
    end = SDL_GetPerformanceCounter();
    duration = ((end-start) * 1000) / SDL_GetPerformanceFrequency();

    fprintf (stdout, "render_terminal took: %lld ms\n", duration);
    */
}

void parseCommandlineParameters (int argc, char **argv)
{
    int argIndex;

    if (argc >= 2)
    {
        for (argIndex = 1; argIndex < argc; argIndex ++)
        {
            if (argv[argIndex][0] == '-' || argv[argIndex][0] == '/')
            {
                switch (argv[argIndex][1])
                {
                    case 'h':
                        fprintf (stdout, "ADAM-3 help\n");
                        exit (0);
                        break;

                    default:
                        fprintf (stdout, "unknown argument \'%c\' given", argv[argIndex][1]);
                        exit (1);
                }
            }
        }
    }
}

int main (int argc, char **argv)
{
    SDL_WindowFlags flags;
    SDL_Event event;

    uint64_t start, end;
    double duration, left;

    int run = true;

    cursorPosition.x = 1;
    cursorPosition.y = 1;

    parseCommandlineParameters (argc, argv);

    adm3a_initialize ();

    if (SDL_InitSubSystem (SDL_INIT_VIDEO) == 0)
    {
        fprintf (stderr, "SDL_InitSubsystem failed: %s\n", SDL_GetError());
        return 1;
    }

    flags = SDL_WINDOW_RESIZABLE;
    if ((window = SDL_CreateWindow ("ADAM-3", 1280, 800, flags)) == NULL)
    {
        fprintf (stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        return 1;
    }
    SDL_ShowWindow (window);

    if ((renderer = SDL_CreateGPURenderer (NULL, window)) == NULL)
    {
        fprintf (stderr, "SDL_CreateGPURenderer failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_SetRenderDrawColor (renderer, 0, 0, 0, 255);
    SDL_SetRenderVSync (renderer, SDL_RENDERER_VSYNC_DISABLED);

    /* create our framebuffer */
    framebuffer = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_XRGB8888, SDL_TEXTUREACCESS_TARGET, 640, 400);
    if (framebuffer == NULL)
    {
        fprintf (stderr, "SDL_CreateTexture failed: %s\n", SDL_GetError());
        return 1;
    }
    if (SDL_SetTextureScaleMode (framebuffer, SDL_SCALEMODE_PIXELART) == 0)
    {
        fprintf (stderr, "SDL_SetTextureScaleMode failed: %s\n", SDL_GetError());
        return 1;
    }

    /* load font atlas texture */
    if ((fontNormal = load_fontatlas()) == NULL)
        return 1;

    /* Main Loop */
    while (run)
    {
        start = SDL_GetPerformanceCounter ();

        /* react to events */
        if (SDL_PollEvent (&event))
        {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                    run = false;
                    break;

                case SDL_EVENT_WINDOW_RESIZED:
                case SDL_EVENT_WINDOW_SHOWN:
                    update_framebuffer_rect ();
                    break;

                default:
                    break;
            }
        }

        /* render the terminal the framebuffer */
        render_terminal();

        /* render framebuffer into the window */
        SDL_RenderClear (renderer);
        SDL_RenderTexture (renderer, framebuffer, NULL, &framebufferRenderRect);
        SDL_RenderPresent (renderer);

        /* cap to 60 fps */
        end = SDL_GetPerformanceCounter ();
        duration = (double) (end - start) * 1000.0 / (double) SDL_GetPerformanceFrequency();
        left = 16.67 - duration;
        if (left>0.0)
        {
            SDL_Delay (left);
        }
    }
}
