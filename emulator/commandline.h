#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/stat.h>

typedef struct
{
    char *processName;
    char *processWorkingDir;
    char **processParameters;
    int numProcessParameters;

    bool fullscreen;
} Commandline;

extern Commandline cmdline;

static void print_help (void);
static char * build_absolute_path (char *relative);
bool parse_commandline (int argc, char **argv);

#endif
