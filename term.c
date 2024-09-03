#include "term.h"
#include "arraylist.h"
#include <assert.h>
#include <poll.h>
#include <pthread.h>
#include <pty.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#ifdef NDEBUG
#undef NDEBUG
#endif

#define SHELL       "zsh"
#define BUFFER_SIZE 1024

static Arraylist term_array = { 0 };

static struct
{
    char *data;
    int   cols;
    int   rows;
} screen;

static int active_term_n = -1;

static struct termios origin_termios[2];

void
set_raw_mode(int fd, struct termios *orig_opts)
{
    struct termios raw_opts;
    tcgetattr(fd, orig_opts);
    raw_opts = *orig_opts;
    cfmakeraw(&raw_opts);
    raw_opts.c_oflag |= (OPOST | ONLCR); // Activa la conversión de '\n' en '\r\n'
    tcsetattr(fd, TCSANOW, &raw_opts);
}


void
term_create()
{
    int   status;
    Term *term;

    if (term_array.data == NULL)
    {
        puts("Creating term before initialize array");
        exit(EXIT_FAILURE);
    }

    term = malloc(sizeof(Term));
    assert(term);

    term->fd     = -1;
    term->active = false;

    term->size.ws_col    = screen.cols;
    term->size.ws_row    = screen.rows;
    term->size.ws_xpixel = 0;
    term->size.ws_ypixel = 0;

    status = forkpty(&term->fd, NULL, &term->opts, &term->size);

    if (status == 0)
    {
        execlp(SHELL, SHELL, NULL);
        perror("execlp failed");
        exit(EXIT_FAILURE);
    }

    assert(status != -1);
    printf("PTY fd: %d\n", term->fd);

    // Establece la terminal creada en modo raw
    set_raw_mode(term->fd, &origin_termios[0]);

    active_term_n = arraylist_append(&term_array, term);
    printf("Active term: %d\n", active_term_n);
}

void
handle_read(int fd)
{
    char    buff[BUFFER_SIZE];
    ssize_t status;

    status = read(fd, buff, sizeof(buff) - 1);

    if (status < 0)
        exit(EXIT_FAILURE);

    if (status)
    {
        buff[status] = '\0';
        printf("%s", buff);
        fflush(stdout);
    }
    else // fd closed
    {
        perror("Fd closed");
    }
}

void *
output_handler()
{
    const int     pty_n = arraylist_length(term_array);
    struct pollfd fds[pty_n + 1];
    int           status;

    for (int i = 0; i < pty_n; i++)
    {
        fds[i].fd     = arraylist_get(term_array, i)->fd;
        fds[i].events = POLLIN | POLLERR | POLLHUP;
    }

    while (1)
    {
        status = poll(fds, pty_n, -1);

        if (status > 0)
            for (int i = 0; i < pty_n; i++)
            {
                if (fds[i].revents & (POLLIN | POLLERR | POLLHUP))
                    handle_read(fds[i].fd);
            }
        else
            perror("poll");
    }
    return NULL;
}

void
destroy_all()
{
    Term *element;
    while ((element = arraylist_remove(&term_array, 0)))
    {
        close(element->fd);
        // delete or free other term struct stuff
    }
}

void
mssleep(int milliseconds)
{
    struct timespec t;
    struct timespec _t;
    t.tv_sec  = milliseconds / 1000;
    t.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&t, &_t);
}

void
term_send_char(Term *term, char *c)
{
    write(term->fd, c, 1);
    fsync(term->fd);
}

void
term_send(Term *term, char *c)
{
    write(term->fd, c, strlen(c));
    fsync(term->fd);
}

void
kbd_handler()
{
    char    buff[BUFFER_SIZE] = { 0 };
    char   *c                 = buff;
    ssize_t status;

    // Configurar stdin en modo raw
    set_raw_mode(STDIN_FILENO, &origin_termios[1]);

    while (1)
    {
        status = read(STDIN_FILENO, c, 1);
        assert(status >= 0);

        if (status == 0)
        {
            mssleep(100);
        }
        else
        {
            if (active_term_n > -1)
                term_send_char(arraylist_get(term_array, active_term_n), c);
        }

        // move c in the circular buffer
        c = (buff + BUFFER_SIZE == c + 1) ? buff : c;
    }

    // Restaurar el modo original de la terminal al salir
    tcsetattr(STDIN_FILENO, 0, &origin_termios[1]);
}


void
screen_init()
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    screen.cols = w.ws_col;
    screen.rows = w.ws_row;
}

int
main()
{
    pthread_t output_thread;

    screen_init();

    term_array = arraylist_create(2);
    assert(term_array.data);

    // Configuración inicial de la terminal
    tcgetattr(STDOUT_FILENO, &origin_termios[0]);

    term_create();

    pthread_create(&output_thread, NULL, output_handler, NULL);
    kbd_handler();


    pthread_cancel(output_thread);
    pthread_join(output_thread, NULL);
    atexit(destroy_all);
    return 0;
}
