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
    char     *title;
    Vec2d     position;
    Vec2d     dimension;
    pthread_t win_thread;
    bool      active;
    uint8_t   refresh_rate;

} Window;


void window_render(Window win);


#endif
