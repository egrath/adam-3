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
    uint32_t terminalForeground;
    uint32_t terminalBackground;
    uint32_t cursorForeground;
    uint32_t cursorBackground;
} Commandline;

extern Commandline cmdline;

static void printHelp (void);
static char * buildAbsolutePath (char *relative);
static void setDefaults (void);
bool parseCommandlineParameters (int argc, char **argv);

#endif
