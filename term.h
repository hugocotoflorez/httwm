#ifndef HTTWM_TERM_H
#define HTTWM_TERM_H

#include <stdbool.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>


#define BORDER_CHAR '#'
#define BORDER_FG   YELLOW
#define BORDER_BG   WHITE
/* Border size. Avaliable sizes: 1 */
#define BORDER_SIZE 1
/* Make vertical borders look as wide as horizontal.
 * Values: 0(false), 1(true) */
#define BORDER_DOUBLE_VERTICAL 0

#define BG_COLOR BLACK
#define FG_COLOR WHITE

#define SHELL       "zsh"
//#define SHELL       "bash"
#define BUFFER_SIZE 1024


typedef struct
{
    int i, j;
} vec2;

typedef struct
{
    int            fd;
    bool           active;
    struct termios opts;
    struct winsize size;
    vec2           position;
    vec2           cursor;
} Term;


// screen.c
void screen_print_term(Term *term);

#endif // HTTWM_TERM_H
