#include "config.h"

configuration config;

/* set default parameters in case we don't have a config file */
void config_set_default (void)
{
    config.auto_newline = true;
    config.terminal_foreground = uint32_to_rgba (0xFF81FF81);
    config.terminal_background = uint32_to_rgba (0xFF000000);
    config.cursor_foreground = uint32_to_rgba (0xFF000000);
    config.cursor_background = uint32_to_rgba (0xFFFFB003);
}

/* read configuration file */
bool config_init (void)
{
    #ifdef DEBUG
    fprintf (stdout, "DEBUG: reading configuration file\n");
    #endif

    config_set_default ();

    return true;
}
