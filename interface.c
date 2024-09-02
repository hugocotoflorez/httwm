#include "interface.h"
#include "window.h"
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

static struct termios origin_termios;

void
disableRawMode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &origin_termios);
}

void
enableRawMode()
{
    tcgetattr(STDIN_FILENO, &origin_termios);
    struct termios raw;
    raw.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP);
    raw.c_iflag &= ~(INLCR | IGNCR | ICRNL | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    raw.c_cflag &= ~(CSIZE | PARENB);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void
command_to_file(int fd, char *command[const])
{
    if (dup2(fd, STDOUT_FILENO) >= 0)
    {
        close(fd);
        execvp(command[0], command);
    }
}

void
run_command(Window win, char *command[const])
{
    if (!fork()) // child
    {
        command_to_file(win.pipe[0], command);
        return;
    }
}

int
main(int argc, char **argv)
{
    enableRawMode();

    int    fd = open("out.txt", O_RDWR | O_CREAT | O_APPEND, 0600);
    Window win;
    win.pipe[0] = fd;

    if (argc > 1)
    {
        run_command(win, argv + 1);
    }


    disableRawMode();
    return 0;
}
