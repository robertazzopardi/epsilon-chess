#ifndef _WINDOW_h_
#define _WINDOW_h_

#include "common.h"

typedef struct
{
    SDL_Window *win;
    SDL_Renderer *rend;
} Window;

extern Window mainWindow;

void initialise();

#endif
