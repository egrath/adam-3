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
} Commandline;

extern Commandline cmdline;

bool parseCommandlineParameters (int argc, char **argv);

#endif
