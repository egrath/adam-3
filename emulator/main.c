#include <stdio.h>
#include <stdlib.h>

#include <SDL3/SDL.h>

#include "adm3a.h"
#include "commandline.h"
#include "pty_unix.h"
#include "renderer.h"
#include "config.h"

char lastKeyCodeSent;

bool process_child_input (char c);
bool process_child_output (void);

static int run = true;

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
    process_child_input (code);
}

bool handle_keydown (SDL_KeyboardEvent *event)
{
    #ifdef DEBUG
    fprintf (stdout, "handle_keydown for [%02X] called\n", event->key);
    #endif

    /* we use the F-Keys for our control and also for some convenience stuff */
    switch (event->key)
    {
        case SDLK_F5:
            cmdline.fullscreen = !cmdline.fullscreen;
            render_set_fullscreen (cmdline.fullscreen);
            break;

        case SDLK_F11:
            stop_process ();
            start_process ();
            break;

        case SDLK_F12:
            run = false;
            break;

        /* cursor control sends WordStar Diamond */
        case SDLK_UP:
            /* Ctrl-E */
            send_ascii (0x5);
            break;

        case SDLK_DOWN:
            /* Ctrl-X */
            send_ascii (0x18);
            break;

        case SDLK_LEFT:
            /* Ctrl-S */
            send_ascii (0x13);
            break;

        case SDLK_RIGHT:
            /* Ctrl-D */
            send_ascii (0x4);
            break;
    }

    /* we have to send CTRL-xxx combinations to the slave, e.g. Ctrl-A = 1, Ctrl-B = 2, ... */
    if ((event->mod & SDL_KMOD_LCTRL) || (event->mod & SDL_KMOD_RCTRL))
    {
        #ifdef DEBUG
        fprintf (stdout, "handle_keydown: control modifier pressed, handling\n");
        #endif

        if (event->key >= 'a' && event->key <= 'z')
            send_ascii(event->key - 0x60);
        else if (event->key == SDLK_HOME)
            send_ascii (ASCII_RS);
    }
    else if (event->key == SDLK_DELETE)
    {
        #ifdef DEBUG
        fprintf (stdout, "handle_keydown: DELETE key pressed\n");
        #endif
        send_ascii (ASCII_DEL);
    }
    else if (event->key == SDLK_BACKSPACE)
    {
        #ifdef DEBUG
        fprintf (stdout, "handle_keydown: BACKSPACE key pressed\n");
        #endif
        send_ascii (ASCII_BS);
    }
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
        {
            #ifdef DEBUG
            fprintf (stdout, "handle_keydown: repeating last key\n");
            #endif
            send_ascii (lastKeyCodeSent);
        }
    }

    #ifdef DEBUG
    fprintf (stdout, "handle_keydown: returning\n");
    #endif

    return true;
}

bool handle_textinput (SDL_TextInputEvent *event)
{
    int i, inputLen;
    const bool *keyState;

    #ifdef DEBUG
    fprintf (stdout, "handle_textinput called\n");
    #endif

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

    #ifdef DEBUG
    fprintf (stdout, "handle_textinput return\n");
    #endif

    return true;
}

/* #######################################################################
   Terminal bell 
   ####################################################################### */

void terminal_bell (void)
{
    /* Ring the bell, but not implemented yet */
    fprintf (stdout, "Terminal bell!\n");
}

/* #######################################################################
   Child process communication
   ####################################################################### */

/* data from the child process to the terminal */
bool process_child_output (void)
{
    ssize_t numRead;
    char inputBuffer[PTY_BUFFER_SIZE];
    int i;

    /* check if we received any data from the slave */
    numRead = read_child (inputBuffer, PTY_BUFFER_SIZE);

    if (numRead > 0)
    {
        #ifdef DEBUG
        fprintf (stdout, "DEBUG: received %zd bytes from the child and passing it on\n", numRead);
        #endif

        /* send the incoming data to the terminal */
        for (i = 0; i < (int) numRead; i ++)
        {
            adm3a_eat (inputBuffer[i]);
        }
    }
    else if (numRead < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
        return false;

    return true;
}

/* from the terminal to the child process */
bool process_child_input (char c)
{
    return write_child (&c, 1);
}

int main (int argc, char **argv)
{
    SDL_Event event;

    uint64_t start, end;
    double duration, left;

    if (!config_init ())
        fprintf (stdout, "config.txt is missing, using defaults\n");

    if (!parse_commandline (argc, argv))
        return 1;

    if (!render_initialize ())
        return 1;

    adm3a_initialize ();
    adm3a_bell = terminal_bell;

    /* start the child process running as our terminal client */
    if (!start_process ())
    {
        fprintf (stderr, "failed to run child process (%s, %d)\n", __FILE__, __LINE__);
        exit (1);
    }

    /* Register the callback for the blinky state of the cell buffer */
    SDL_AddTimer (500, blinky_callback, NULL);

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
                    break;

                case SDL_EVENT_KEY_DOWN:
                    handle_keydown (&(event.key));
                    break;

                /* Drag and drop support for XMODEM transfers */
                case SDL_EVENT_DROP_FILE:
                    fprintf (stdout, "drop file: %s\n", event.drop.data);
                    break;

                default:
                    break;
            }
        }

        /* read all incoming data from the started process */
        if (is_child_alive ())
            process_child_output ();
        else
            run = false;

        /* render the terminal the framebuffer */
        render_terminal ();

        /* render framebuffer into the window */
        render_framebuffer ();

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
    stop_process ();

    render_cleanup ();
}
