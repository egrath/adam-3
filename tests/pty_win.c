
#define _WIN32_WINNT 0x0A00

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

DWORD old_input_mode;
DWORD old_output_mode;

int main(void)
{
    HANDLE inputRead, inputWrite;
    HANDLE outputRead, outputWrite;
    HPCON console;
    STARTUPINFOEXW si;
    PROCESS_INFORMATION pi;
    SIZE_T attrSize = 0;
    char buffer[4096];
    DWORD bytesRead, bytesWritten;

    /* Create the two pipes */
    CreatePipe(&inputRead, &inputWrite, NULL, 0);
    CreatePipe(&outputRead, &outputWrite, NULL, 0);

    /* Create the pseudo-console */
    CreatePseudoConsole(
        (COORD){ 80, 24 },
        inputRead,
        outputWrite,
        0,
        &console
    );

    CloseHandle(inputRead);
    CloseHandle(outputWrite);

    /* Prepare process startup information */
    memset(&si, 0, sizeof(si));
    si.StartupInfo.cb = sizeof(si);

    InitializeProcThreadAttributeList(
        NULL, 1, 0, &attrSize
    );

    si.lpAttributeList = malloc(attrSize);

    InitializeProcThreadAttributeList(
        si.lpAttributeList, 1, 0, &attrSize
    );

    UpdateProcThreadAttribute(
        si.lpAttributeList,
        0,
        PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE,
        console,
        sizeof(console),
        NULL,
        NULL
    );

    /* Start cmd.exe */
    wchar_t command[] = L"hec_win.exe";

    CreateProcessW(
        NULL,
        command,
        NULL, NULL,
        FALSE,
        EXTENDED_STARTUPINFO_PRESENT,
        NULL, NULL,
        &si.StartupInfo,
        &pi
    );

    DeleteProcThreadAttributeList(si.lpAttributeList);
    free(si.lpAttributeList);

    /* Send input to the child */
    bool childAlive = true;
    while (! feof(stdin) && childAlive)
    {
        char input[100];
        fgets (input, 100, stdin);

        WriteFile(
            inputWrite,
            input,
            (DWORD)strlen(input),
            &bytesWritten,
            NULL
        );

        /* check if child is still alive */
        if (WaitForSingleObject(pi.hProcess, 0) != WAIT_TIMEOUT)
        {
            childAlive = false;
            continue;
        }

        /* check for data */
        DWORD available;
        DWORD bytesRead;
        char buffer[4096];

        PeekNamedPipe(
            outputRead,
            NULL,
            0,
            NULL,
            &available,
            NULL
        );

        if (available > 0)
        {
            ReadFile(
                outputRead,
                buffer,
                sizeof(buffer) < available ? sizeof(buffer) : available,
                &bytesRead,
                NULL
            );

            fwrite(buffer, 1, bytesRead, stdout);
            fflush(stdout);
        }
    }

    CloseHandle(inputWrite);

    /* wait for the child to exit */
    TerminateProcess (pi.hProcess, 0);
    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    CloseHandle(outputRead);
    ClosePseudoConsole(console);

    return 0;
}
