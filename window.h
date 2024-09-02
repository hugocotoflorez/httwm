#ifndef HTTWM_WINDOW_H
#define HTTWM_WINDOW_H

#include "stdint.h"
#include "termstuff.h"
#include <pthread.h> // pthread_t
#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    uint16_t i, j;
} Vec2d;

typedef struct
{
    // Assign by user
    char     *title;
    Vec2d     position;
    Vec2d     dimension;
    // Assign by program
    pthread_t win_thread;
    uint8_t   refresh_rate;
    bool      active;
    int       pipe[2];

} Window;


void window_render(Window win);


#endif
