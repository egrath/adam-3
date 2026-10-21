#include "pty_windows.h"

/* read and write handles to the child process */
HANDLE output_read_side;	/* read output from the child */
HANDLE input_write_side;	/* write input to the child */

PROCESS_INFORMATION process;
HPCON console;

/* private functions */
static char * get_error_message (DWORD error)
{
	char *message_buffer = NULL;

	DWORD size = FormatMessage (
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		error,
		MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR) &message_buffer,
		0,
		NULL
	);

	if (size == 0)
		return NULL;
	else
	{
		int pos = strcspn (message_buffer, "\r\n");
		if (pos >= 0)
			message_buffer[pos] = '\0';
		return message_buffer;
	}
}

/* how much data is available for us to read from the child? */
int		child_data_available (void)
{
	DWORD available;
	DWORD bytes_read;
	char buffer[4096];

	PeekNamedPipe(
		output_read_side,
		NULL,
		0,
		NULL,
		&available,
		NULL
	);

	return (int) available;
}

/* public functions */
bool    child_has_data (void)
{
	if (child_data_available () > 0)
		return true;
	
	return false;
}

int     child_read (uint8_t *buffer, int buffer_size)
{
	int available, num_read;

	if ((available = child_data_available ()) > 0)
	{
		ReadFile (output_read_side,
			buffer,
			buffer_size,
			(LPDWORD) &num_read,
			NULL
		);

		for (int i = 0; i < num_read; i++)
		{
			fprintf (stdout, "%02X ", buffer[i]);
		}

		return num_read;
	}
	else
		return -1;
}

bool    child_write (uint8_t *buffer, int buffer_size)
{
	int num_written;

	WriteFile (
		input_write_side, 
		buffer, 
		buffer_size,
		(LPDWORD) &num_written,
		NULL
	);

	return true;
}

bool    child_alive (void)
{
	int exit_code;

	if (WaitForSingleObject(process.hProcess, 0) != WAIT_TIMEOUT)
	{
		GetExitCodeProcess (process.hProcess, (LPDWORD) &exit_code);
		fprintf (stdout, "pty-win: child no longer alive (exit code %d)\n", exit_code);		
		return false;
	}
	return true;
}

bool    start_process (void)
{
	HRESULT hr;
	HANDLE inputReadSide, outputWriteSide;
	char *command;

	/* build the commandline to execute - CreateProcess expects a complete commandline unlike Linux/Mac */
	command = (char *) malloc (sizeof (char) * 256);
	command[0] = '\0';
	strcat (command, cmdline.processName);
	strcat (command, " ");
	for (int i = 0; i < cmdline.numProcessParameters; i ++)
	{
		strcat (command, cmdline.processParameters[i]);
		strcat (command, " ");
	}

	/* pipes and pseudoconsole */
	if (!CreatePipe (&inputReadSide, &input_write_side, NULL, 0))
	{
		fprintf (stdout, "pty-win: pipe creation failed\n");
		return false;
	}

	if (!CreatePipe (&output_read_side, &outputWriteSide, NULL, 0))
	{
		fprintf (stdout, "pty-win: pipe creation failed\n");
		return false;
	}

	hr = CreatePseudoConsole ((COORD) { 80, 24 }, inputReadSide, outputWriteSide, 0, &console);
	if (FAILED (hr))
	{
		fprintf (stdout, "pty-win: pseudoconsole creation failed\n");
		return false;
	}

	CloseHandle (inputReadSide);
	CloseHandle (outputWriteSide);

	/* prepare startup information */
	STARTUPINFOEX si;
	ZeroMemory (&si, sizeof (si));
	si.StartupInfo.cb = sizeof (STARTUPINFOEX);

	size_t bytesRequired;
	InitializeProcThreadAttributeList (NULL, 1, 0, &bytesRequired);
	si.lpAttributeList = (PPROC_THREAD_ATTRIBUTE_LIST) HeapAlloc (GetProcessHeap (), 0, bytesRequired);
	InitializeProcThreadAttributeList (si.lpAttributeList, 1, 0, &bytesRequired);
	UpdateProcThreadAttribute (si.lpAttributeList,
		0, PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE, console, sizeof (console), NULL, NULL);
	
	/* start the process */
	ZeroMemory (&process, sizeof (process));

	if (! CreateProcessA(
		NULL,
		cmdline.processName,
		NULL,
		NULL,
		FALSE,
		EXTENDED_STARTUPINFO_PRESENT,
		NULL,
		cmdline.processWorkingDir,
		&si.StartupInfo,
		&process
	))
	{
		fprintf (stdout, "pty-win: process creation failed\n");
		return false;
	}
	else
	{
		fprintf (stdout, "pty-win: process created, pid = %ld\n", process.dwProcessId);
	}

	return true;
}

void    stop_process (void)
{
	fprintf (stdout, "pty-win: stopping child process (if still alive)\n");

	TerminateProcess (process.hProcess, 0);
	WaitForSingleObject (process.hProcess, 5000);
	CloseHandle (process.hProcess);
	CloseHandle (process.hThread);

	ClosePseudoConsole (console);
	CloseHandle (input_write_side);
	CloseHandle (output_read_side);

	return;
}
