#include <stdio.h>
#include <stdlib.h>

#include <SDL3/SDL.h>

#include "adm3a.h"
#include "commandline.h"
#include "pty_unix.h"
#include "renderer.h"

char lastKeyCodeSent;

bool processChildInput (char c);
bool processChildOutput (void);

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
            adm3a_eat (inputBuffer[i]);
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
    SDL_Event event;

    uint64_t start, end;
    double duration, left;

    int run = true;

    if (!parseCommandlineParameters (argc, argv))
        return 1;

    if (!render_initialize ())
        return 1;

    adm3a_initialize ();
    adm3a_bell = terminal_bell;

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
    stopProcess ();

    render_cleanup ();
}
