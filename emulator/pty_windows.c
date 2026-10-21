#include "pty_windows.h"

/* read and write handles to the child process */
HANDLE output_read_side = NULL;	/* read output from the child */
HANDLE input_write_side = NULL;	/* write input to the child */

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

	if (output_read_side == NULL || output_read_side == INVALID_HANDLE_VALUE)
	{
		fprintf (stdout, "read handle invalid\n");
		return -1;
	}

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
	int num_read;

	if (child_data_available () > 0)
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

	if (input_write_side == NULL || input_write_side == INVALID_HANDLE_VALUE)
		return false;

	WriteFile (
		input_write_side, 
		buffer, 
		buffer_size,
		(LPDWORD) &num_written,
		NULL
	);

	if (num_written != buffer_size)
	{
		fprintf (stdout, "failed to write to child (%d != %d)\n", num_written, buffer_size);
	}

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
	HANDLE child_stdin_read, child_stdout_write;
	char *command;

	setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

	SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;	

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

	/* pipes */
	if (!CreatePipe (&child_stdin_read, &input_write_side, &sa, 0))
	{
		fprintf (stdout, "pty-win: pipe creation failed\n");
		return false;
	}
	SetHandleInformation(input_write_side, HANDLE_FLAG_INHERIT, 0);

	if (!CreatePipe (&output_read_side, &child_stdout_write, &sa, 0))
	{
		fprintf (stdout, "pty-win: pipe creation failed\n");
		return false;
	}
	SetHandleInformation(output_read_side, HANDLE_FLAG_INHERIT, 0);

	/* prepare startup information */
	STARTUPINFO si;
	ZeroMemory (&si, sizeof (si));
	si.cb = sizeof (si);
	si.dwFlags |= STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdInput  = child_stdin_read;   // Child reads from this
    si.hStdOutput = child_stdout_write; // Child writes stdout here
    si.hStdError  = child_stdout_write; // Child writes stderr here too	

	/* start the process */
	ZeroMemory (&process, sizeof (process));

	if (! CreateProcessA(
		NULL,
		cmdline.processName,
		NULL,
		NULL,
		TRUE,
		CREATE_NO_WINDOW,
		NULL,
		cmdline.processWorkingDir,
		&si,
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
	
	CloseHandle(child_stdin_read);
    CloseHandle(child_stdout_write);	

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
