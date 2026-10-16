#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "helper.h"

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
bool parse_commandline (int argc, char **argv);

#endif
