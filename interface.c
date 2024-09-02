#include "interface.h"
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>


void
command_to_file(const char *filename, char *command[const])
{
    pid_t out;
    pid_t save_out;
    pid_t proc;
    int   errcode;

    assert(command && command[0]);

    out = open(filename, O_RDWR | O_CREAT, 0600);
    assert(out >= 0);

    save_out = dup(fileno(stdout));
    errcode  = dup2(out, fileno(stdout));
    assert(errcode >= 0);

    proc = fork();
    assert(proc >= 0);

    if (!proc) // child
        execvp(command[0], command);

    fflush(stdout);
    close(out);
    dup2(save_out, fileno(stdout));
    close(save_out);
}

int
main()
{
    char *args[] = { "nvim", "." };
    command_to_file("ls.txt", args);
    return 0;
}
