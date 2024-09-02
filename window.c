#include "window.h"
#include "arraylist.h"
#include "termstuff.h"
#include <assert.h>
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

/* Array with all the windows. Elements are pointers to Window*/
Arraylist windows_array;

/* Only active in the window where user cursor is */
pthread_mutex_t main_lock;

/* Lock for render */
pthread_mutex_t render_lock;

/* lock for arraylist */
pthread_mutex_t arraylist_lock;


__attribute__((constructor)) static void
init()
{
    windows_array = arraylist_create(2);
    pthread_mutex_init(&main_lock, NULL);
    pthread_mutex_init(&render_lock, NULL);
    pthread_mutex_init(&arraylist_lock, NULL);
}

void
window_resize(Window win)
{
}

void
window_render(Window win)
{
    pthread_mutex_lock(&render_lock);
    term_cursor_save_current_possition();
    term_cursor_position(win.position.i, win.position.j);
    /* TODO */
    term_cursor_restore_saved_position();
    pthread_mutex_unlock(&render_lock);
}

/* Reload window data, do not update on screen data (call window_render for that)*/
bool
window_update_data(Window *win)
{
}

static void
sleepms(const int milliseconds)
{
    struct timespec t;
    struct timespec _t;
    t.tv_sec  = milliseconds / 1000;
    t.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&t, &_t);
}

void *
window_loop(void *args)
{
    Window *win = args;
    while (win->active)
    {
        if (window_update_data(win))
            window_render(*win);

        sleepms(win->refresh_rate);
    }
    return NULL;
}

void
window_create(Window win)
{
    int     status;
    Window *winptr;

    winptr = malloc(sizeof(win));
    assert(winptr);

    *winptr = win;

    pthread_create(&winptr->win_thread, NULL, window_loop, winptr);
    pthread_mutex_lock(&arraylist_lock);
    arraylist_append(&windows_array, winptr);
    pthread_mutex_unlock(&arraylist_lock);

    status = pipe(win.pipe);
    assert(!status);

    win.active = true;
}
