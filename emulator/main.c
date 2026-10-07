#include <stdio.h>
#include <stdlib.h>

#include <SDL3/SDL.h>

#include "adm3a.h"
#include "commandline.h"
#include "pty_unix.h"

#define FRAMEBUFFER_BORDER_SIZE     4              /* border on every side; it's in client space, so it scales */

#define FONT_WIDTH                  8
#define FONT_HEIGHT                 16

SDL_Renderer *renderer;
SDL_Window *window;
SDL_Texture *framebuffer;
SDL_FRect framebufferRenderRect;

SDL_Texture *fontNormal;        /* this is the font normally used */
SDL_Texture *fontInverted;      /* this is for inverted text */
SDL_Texture *fontCursor;        /* this is just for where to cursor is */

char lastKeyCodeSent;

bool processChildInput (char c);
bool processChildOutput (void);

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

/* #######################################################################
   Keyboard and Input handling
   ####################################################################### */

void send_ascii (char code)
{
    #ifdef DEBUG
    fprintf (stdout, "DEBUG: Sending [0x%02X %03d] (%c) to the slave\n",
        code, code, ((code >= 32 && code <= 126) ? code : ' '));
    #endif

    lastKeyCodeSent = code;

    /* send it to our child */
    processChildInput (code);
}

bool handle_keydown (SDL_KeyboardEvent *event)
{
    /* we have to send CTRL-xxx combinations to the slave, e.g. Ctrl-A = 1, Ctrl-B = 2, ... */
    if ((event->mod & SDL_KMOD_LCTRL) || (event->mod & SDL_KMOD_RCTRL))
    {
        if (event->key >= 'a' && event->key <= 'z')
            send_ascii(event->key - 0x60);
        else if (event->key == SDLK_HOME)
            send_ascii (ASCII_RS);
    }
    else if (event->key == SDLK_DELETE)
        send_ascii (ASCII_DEL);
    else if (event->key == SDLK_BACKSPACE)
        send_ascii (ASCII_BS);
    else if (event->key == SDLK_ESCAPE)
        send_ascii (ASCII_ESC);
    else if (event->key == SDLK_TAB)
        send_ascii (ASCII_HT);
    else if (event->key == SDLK_RETURN)
        send_ascii (ASCII_CR);
    else if ((event->key >= SDLK_A && event->key <= SDLK_Z) ||
             (event->key >= SDLK_0 && event->key <= SDLK_9))
    {
        /* only repeat a-z and 0-9, because for other keys, SDL does it by itself with SDL_TEXTINPUT
           there is also special handling required because SDL_KeyEvents only give use lowercase
           keys, so we have to take that into account when doing our repeat magic */
        if ((char) event->key == tolower (lastKeyCodeSent) && event->repeat)
            send_ascii (lastKeyCodeSent);
    }

    return true;
}

bool handle_textinput (SDL_TextInputEvent *event)
{
    int i, inputLen;
    const bool *keyState;

    /* we only process input text if CTRL is unpressed - this should never happen, but better safe then sorry */
    keyState = SDL_GetKeyboardState (NULL);
    if (keyState[SDL_SCANCODE_LCTRL] || keyState[SDL_SCANCODE_RCTRL])
        return false;

    /* if the key is non-ascii, just refuse */
    if (!(event->text[0] >= 0 && event->text[0] <= 127))
        return false;

    inputLen = strlen (event->text);
    for (i = 0; i < inputLen; i ++)
        send_ascii (event->text[i]);

    return true;
}

/* #######################################################################
   Child process communication
   ####################################################################### */

bool processChildOutput (void)
{
    ssize_t numRead;
    char inputBuffer[PTY_BUFFER_SIZE];
    int i;

    /* check if we received any data from the slave */
    numRead = read (masterFd, inputBuffer, PTY_BUFFER_SIZE);
    if (numRead > 0)
    {
        #ifdef DEBUG
        fprintf (stdout, "DEBUG: received %zd bytes from the child and passing it on\n", numRead);
        #endif

        /* send the incoming data to the terminal */
        for (i = 0; i < (int) numRead; i ++)
        {
            adm3a_process_character (inputBuffer[i]);
        }
    }
    else if (numRead < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
        return false;

    return true;
}

bool processChildInput (char c)
{
    if (!isChildAlive ())
        return false;

    write (masterFd, &c, 1);

    return true;
}

int main (int argc, char **argv)
{
    SDL_WindowFlags flags;
    SDL_Event event;

    uint64_t start, end;
    double duration, left;

    int fbWidth, fbHeight;

    /* what is the size of our framebuffer? It has to fit 80x24 characters with a size
       of 8x16 pixels and also has some space around for better visibility */
    fbWidth = (FONT_WIDTH * TERM_COLUMNS) + (2 * FRAMEBUFFER_BORDER_SIZE);
    fbHeight = (FONT_HEIGHT * TERM_ROWS) + (2 * FRAMEBUFFER_BORDER_SIZE);

    #ifdef DEBUG
    fprintf (stdout, "DEBUG: framebuffer size will be a total of %d x %d pixels\n", fbWidth, fbHeight);
    #endif

    int run = true;

    if (!parseCommandlineParameters (argc, argv))
        return 1;

    adm3a_initialize ();

    if (SDL_InitSubSystem (SDL_INIT_VIDEO) == 0)
    {
        fprintf (stderr, "SDL_InitSubsystem failed: %s (%s, %d)\n", SDL_GetError(), __FILE__, __LINE__);
        return 1;
    }

    flags = SDL_WINDOW_RESIZABLE;
    if ((window = SDL_CreateWindow ("ADAM-3", fbWidth*2, fbHeight*2, flags)) == NULL)
    {
        fprintf (stderr, "SDL_CreateWindow failed: %s (%s, %d)\n", SDL_GetError(), __FILE__, __LINE__);
        return 1;
    }
    SDL_ShowWindow (window);

    if ((renderer = SDL_CreateGPURenderer (NULL, window)) == NULL)
    {
        fprintf (stderr, "SDL_CreateGPURenderer failed: %s (%s, %d)\n", SDL_GetError(), __FILE__, __LINE__);
        return 1;
    }

    SDL_SetRenderDrawColor (renderer, 0, 0, 0, 255);
    SDL_SetRenderVSync (renderer, SDL_RENDERER_VSYNC_DISABLED);

    /* create our framebuffer */
    framebuffer = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_XRGB8888, SDL_TEXTUREACCESS_TARGET, fbWidth, fbHeight);
    if (framebuffer == NULL)
    {
        fprintf (stderr, "SDL_CreateTexture failed: %s (%s, %d)\n", SDL_GetError(), __FILE__, __LINE__);
        return 1;
    }
    if (SDL_SetTextureScaleMode (framebuffer, SDL_SCALEMODE_PIXELART) == 0)
    {
        fprintf (stderr, "SDL_SetTextureScaleMode failed: %s (%s, %d)\n", SDL_GetError(), __FILE__, __LINE__);
        return 1;
    }

    SDL_StartTextInput (window);

    /* load font atlas textures for the different font kinds used */
    #ifdef DEBUG
    fprintf (stdout, "DEBUG: terminal foreground=%08X background=%08X\n", cmdline.terminalForeground, cmdline.terminalBackground);
    #endif
    if ((fontNormal = load_fontatlas (cmdline.terminalForeground, cmdline.terminalBackground)) == NULL)
        return 1;
    if ((fontInverted = load_fontatlas (cmdline.terminalBackground, cmdline.terminalForeground)) == NULL)
        return 1;
    if ((fontCursor = load_fontatlas (cmdline.cursorForeground, cmdline.cursorBackground)) == NULL)
        return 1;

    /* start the child process running as our terminal client */
    if (!startProcess (TERM_ROWS, TERM_COLUMNS))
    {
        fprintf (stderr, "failed to run child process (%s, %d)\n", __FILE__, __LINE__);
        exit (1);
    }

    /* Main Loop */
    while (run)
    {
        start = SDL_GetPerformanceCounter ();

        /* react to events */
        while (SDL_PollEvent (&event))
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

                case SDL_EVENT_TEXT_INPUT:
                    handle_textinput (&(event.text));

                case SDL_EVENT_KEY_DOWN:
                    handle_keydown (&(event.key));
                    break;

                default:
                    break;
            }
        }

        /* read all incoming data from the started process */
        if (isChildAlive ())
            processChildOutput ();
        else
            run = false;

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

    /* stop the child process */
    stopProcess ();

    /* clean up */
    SDL_StopTextInput (window);
}
