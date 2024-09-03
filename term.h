#ifndef HTTWM_TERM_H
#define HTTWM_TERM_H

#include <stdbool.h>
#include <sys/ioctl.h>
#include <termios.h>

typedef struct
{
    int            fd;
    bool           active;
    struct termios opts;
    struct winsize size;
} Term;

#endif // HTTWM_TERM_H
