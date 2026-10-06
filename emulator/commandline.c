#include "commandline.h"

Commandline cmdline;

static void printHelp (void)
{
    fprintf (stdout, "ADAM-3 Terminal emulator\n\n");
    fprintf (stdout, "parameters:\n");
    fprintf (stdout, "    -p <process>              Process to start\n");
    fprintf (stdout, "   [-w <directory>]           Working directory for process\n");
    fprintf (stdout, "    -- <param 1 ... param n>  Pass parameter 1 to n to the child\n");
}

static char * buildAbsolutePath (char *relative)
{
    char *absolute;
    struct stat s;

    if ((absolute = (char *) malloc (sizeof (char) * 256)) == NULL)
    {
        fprintf (stderr, "malloc() failed (%s, %d)\n", __FILE__, __LINE__);
        return NULL;
    }

    /* convert from relative to absolute */
    if ((realpath (relative, absolute)) == NULL)
    {
        fprintf (stderr, "realpath() failed (%s, %d)\n", __FILE__, __LINE__);
        return NULL;
    }

    /* check if the file / directory exists */
    if (stat (absolute, &s) != 0)
    {
        fprintf (stderr, "file or directory [%s] does not exist (%s, %d)\n", absolute, __FILE__, __LINE__);
        return NULL;
    }

    return absolute;
}

static void setDefaults (void)
{
    /* terminal colors */
    cmdline.terminalForeground = 0xFF81FF81;
    cmdline.terminalBackground = 0xFF000000;
    cmdline.cursorForeground = 0xFF000000;
    cmdline.cursorBackground = 0xFFFFB003;
}

bool parseCommandlineParameters (int argc, char **argv)
{
    int argIndex, parIndex;
    int childParameterIndex = -1;

    setDefaults();

    if (argc >= 2)
    {
        for (argIndex = 1; argIndex < argc && childParameterIndex == -1; argIndex ++)
        {
            if (argv[argIndex][0] == '-' && childParameterIndex == -1)
            {
                switch (argv[argIndex][1])
                {
                    case 'h':
                        printHelp ();
                        return false;
                        break;
/*
                    case 'f':
                        if (argv[argIndex][1] == '\0')
                        {
                            printHelp ();
                            return false;
                        }
                        else if (argv[argIndex][1] == 't' && argv[argIndex+1] != NULL)
                        {
                            cmdline.terminalForeground = 
                        }
                        break;
*/

                    case 'w': /* working directory for child process */
                        if (argv[argIndex+1] != NULL)
                        {
                            cmdline.processWorkingDir = buildAbsolutePath (argv[argIndex+1]);
                            argIndex ++;
                        }
                        else
                        {
                            fprintf (stderr, "need parameter for option '-w': working directory\n");
                            return false;
                        }
                        break;

                    case 'p':
                        if (argv[argIndex+1] != NULL)
                        {
                            cmdline.processName = buildAbsolutePath (argv[argIndex+1]);
                            argIndex++;
                        }
                        else
                        {
                            fprintf (stderr, "need parameter for option '-p': process name to launch\n");
                            return false;
                        }
                        break;

                    case '-':
                        if (argv[argIndex+1] == NULL)
                        {
                            fprintf (stderr, "no child parameters given, despite indicated\n");
                            return false;
                        }
                        childParameterIndex = argIndex+1;
                        break;

                    default:
                        fprintf (stderr, "unknown parameter '-%c'\n", argv[argIndex][1]);
                        return false;
                }
            }
        }
    }

    /* process the parameters for the child if given */
    if (childParameterIndex >= 0)
    {
        cmdline.processParameters = argv + childParameterIndex;
        cmdline.numProcessParameters = argc - childParameterIndex;
    }

    /* sanity check */
    if (cmdline.processName == NULL)
    {
        printHelp ();
        return false;
    }
    else
    {
        #ifdef DEBUG
        fprintf (stdout, "DEBUG: process to start.        : %s\n", cmdline.processName);
        fprintf (stdout, "DEBUG: process working directory: %s\n", cmdline.processWorkingDir == NULL ? "not specified" : cmdline.processWorkingDir);
        for (parIndex = 0; parIndex < cmdline.numProcessParameters; parIndex ++)
        {
            fprintf (stdout, "DEBUG:             parameter %02d: %s\n", parIndex, *(cmdline.processParameters+parIndex));
        }
        #endif
    }

    return true;
}
