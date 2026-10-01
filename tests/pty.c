#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/errno.h>

#include <string.h>

#define MAX_SLAVE_NAME      64

struct termios old_termios;

int ptyMasterOpen (char *slaveName, size_t len)
{
    int masterFd;
    char *p;

    /* open the master PTY */
    masterFd = posix_openpt (O_RDWR | O_NOCTTY);
    if (masterFd == -1)
        return -1;

    /* Grant us access to the slave PTY */
    if (grantpt (masterFd) == -1)
    {
        close (masterFd);
        return -1;
    }

    /* Release the slave PTY so that some other process can use it */
    if (unlockpt (masterFd) == -1)
    {
        close (masterFd);
        return -1;
    }

    if ((p = ptsname (masterFd)) == NULL)
    {
        close (masterFd);
        return -1;
    }

    if (strlen (p) > len)
    {
        close (masterFd);
        return -1;
    }
    else
        strcpy (slaveName, p);

    return masterFd;
}

pid_t ptyFork (int *masterFd, struct termios *slaveTermios, const struct winsize *slaveWs)
{
    int mfd, slaveFd;
    pid_t childPid;
    char slaveName[MAX_SLAVE_NAME];

    mfd = ptyMasterOpen (slaveName, MAX_SLAVE_NAME);
    if (mfd == -1)
        return -1;

    childPid = fork();
    if (childPid == -1) /* fork has failed */
    {
        close (mfd);
        return -1;
    }

    if (childPid != 0)
    {
        /* Executed by parent - fork() returns 0 for the child and the PID of the child
           to the parent */

        *masterFd = mfd;
        return childPid;
    }

    /* this is now only executed if we are the child */
    if (setsid () == -1)
    {
        fprintf (stderr, "setsid() failed\n");
        exit (1);
    }

    /* the master file descriptor is not needed in the child anymore */
    close (mfd);

    slaveFd = open (slaveName, O_RDWR);
    if (slaveFd == -1)
    {
        fprintf (stderr, "open() for slave PTY file descriptor failed\n");
        exit (1);
    }

    #if defined(TIOCSCTTY) /* needed on the BSD's to acquire controlling TTY */
    if (ioctl (slaveFd, TIOCSCTTY) == -1)
    {
        fprintf (stderr, "ioctl for slave PTY failed while setting TIOCSCTTY\n");
        exit (1);
    }
    #endif

    /* set slave PTY termios parameters if given */
    if (slaveTermios != NULL)
        tcsetattr (slaveFd, TCSANOW, slaveTermios);

    /* set slave PTY window size parameters if given */
    if (slaveWs != NULL)
        ioctl (slaveFd, TIOCSWINSZ, slaveWs);

    /* Duplicate pty slave to become child's stdin, stdout and stderr */
    dup2 (slaveFd, STDIN_FILENO);
    dup2 (slaveFd, STDOUT_FILENO);
    dup2 (slaveFd, STDERR_FILENO);

    if (slaveFd > STDERR_FILENO)
        close (slaveFd);

    return 0;
}

void enableRawMode (void)
{
    struct termios raw;

    /* save old (current) terminal settings */
    tcgetattr (STDIN_FILENO, &old_termios);

    tcgetattr (STDIN_FILENO, &raw);

    /* disable local echo, canonical input processing and the interrupt signal (Ctrl-C, Ctrl-Z) */
    /* disable extended input processing (IEXTEN) for Ctrl-V */
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN );

    /* disable software flow control (Ctrl-S, Ctrl-Q) */
    /* disable newline processing (ICRNL) for Ctrl-M and ENTER key in the input */
    raw.c_iflag &= ~(IXON | ICRNL);

    /* disable newline processing for the output, so when we send a '\n', really
       just send a '\n' and not a '\r\n' */
    raw.c_oflag &= ~(OPOST);

    /* also set some other flags which should not matter, but just to make sure */
    raw.c_iflag &= ~(BRKINT | INPCK | ISTRIP );
    raw.c_cflag |= ~(CS8);

    tcsetattr (STDIN_FILENO, TCSANOW, &raw);
}

void disableRawMode (void)
{
    tcsetattr (STDIN_FILENO, TCSANOW, &old_termios);
}

int main (int argc, char **argv)
{
    struct termios termiosOri;
    struct winsize ws;
    pid_t childPid;
    int childStatus;
    int masterFd;
    fd_set inFds;
    size_t numRead;
    char buffer[256];
    bool finished = false;

    /* save for later use */
    tcgetattr (STDIN_FILENO, &termiosOri);

    /* get current window size */
    ioctl (STDIN_FILENO, TIOCGWINSZ, &ws);

    childPid = ptyFork (&masterFd, &termiosOri, &ws);
    if (childPid == -1)
    {
        fprintf (stderr, "ptyFork() failed\n");
        exit (1);
    }

    if (childPid == 0) /* Am I the child now? */
    {
        execlp ("./hec", "./hec", (char *) NULL);
        exit (1);
    }

    fprintf (stdout, "my child process has the pid %d\n\r", childPid);

    /* only the parent gets here */
    enableRawMode ();

    /* make the PTY and stdin non blocking */
    fcntl (masterFd, F_SETFL, O_NONBLOCK);
    fcntl (STDIN_FILENO, F_SETFL, O_NONBLOCK);

    while (!finished)
    {
        /* check if the child has terminated */
        if (waitpid (childPid, &childStatus, WNOHANG) == childPid)
        {
            fprintf (stdout, "\n\rchild died");
            finished = true;
        }

        /* check if we received any data from the slave */
        numRead = read (masterFd, buffer, 256);
        if (numRead > 0)
            write (STDOUT_FILENO, buffer, numRead);
        else if (numRead < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
            finished = true;

        /* check if we received any data from our stdin to send to the child */
        numRead = read (STDIN_FILENO, buffer, 256);
        if (numRead > 0)
            write (masterFd, buffer, numRead);
        else if (numRead < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
            finished = true;
    }

    disableRawMode ();
    fprintf (stdout, "\n\rmaster is terminating\n");
}
