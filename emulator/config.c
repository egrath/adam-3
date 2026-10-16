#include "config.h"

configuration config;

/* set default parameters in case we don't have a config file */
void config_set_default (void)
{
    config.auto_newline = true;
    config.fullscreen = true;
    config.terminal_foreground = uint32_to_rgba (0xFF81FF81);
    config.terminal_background = uint32_to_rgba (0xFF000000);
    config.cursor_foreground = uint32_to_rgba (0xFF000000);
    config.cursor_background = uint32_to_rgba (0xFFFFB003);
}

/* print current configuration */
void print_config (void)
{
    fprintf (stdout, "configuration:\n");

    fprintf (stdout, "    terminal foreground: r=%d, g=%d, b=%d\n", 
        config.terminal_foreground.r, 
        config.terminal_foreground.g,
        config.terminal_foreground.b
    );

    fprintf (stdout, "    terminal background: r=%d, g=%d, b=%d\n", 
        config.terminal_background.r, 
        config.terminal_background.g,
        config.terminal_background.b
    );

    fprintf (stdout, "      cursor foreground: r=%d, g=%d, b=%d\n", 
        config.cursor_foreground.r, 
        config.cursor_foreground.g,
        config.cursor_foreground.b
    );

    fprintf (stdout, "      cursor background: r=%d, g=%d, b=%d\n", 
        config.cursor_background.r, 
        config.cursor_background.g,
        config.cursor_background.b
    );

    fprintf (stdout, "           auto newline: %s\n", config.auto_newline ? "true" : "false");
    fprintf (stdout, "             fullscreen: %s\n", config.fullscreen ? "true" : "false");
}

/* read configuration file */
bool config_init (void)
{
    FILE *configFile;
    char line[256];
    char *key, *value;

    #ifdef DEBUG
    fprintf (stdout, "DEBUG: reading configuration file\n");
    #endif

    config_set_default ();

    if ((configFile = fopen ("config.txt", "r")) == NULL)
        return false;

    while (!feof (configFile))
    {
        fgets (line, 256, configFile);
        key=strtok (line, "=");
        value=strtok (NULL, "=");

        if (key != NULL && value != NULL)
        {
            /* replace newline */
            key[strcspn (key, "\r\n")] = '\0';
            value[strcspn (value, "\r\n")] = '\0';

            /* process key / value */
            if (strcmp (key, "terminal_foreground") == 0)
                config.terminal_foreground = uint32_to_rgba (str_to_uint32 (value));
            if (strcmp (key, "terminal_background") == 0)
                config.terminal_background = uint32_to_rgba (str_to_uint32 (value));
            if (strcmp (key, "cursor_foreground") == 0)
                config.cursor_foreground = uint32_to_rgba (str_to_uint32 (value));
            if (strcmp (key, "cursor_background") == 0)
                config.cursor_background = uint32_to_rgba (str_to_uint32 (value));
            
            if (strcmp (key, "auto_newline") == 0)
                config.auto_newline = str_to_bool (value);
            
            if (strcmp (key, "fullscreen") == 0)
                config.fullscreen = str_to_bool (value);
        }
    }
    
    print_config();
    
    fclose (configFile);

    return true;
}
