#ifndef _WINDOW_h_
#define _WINDOW_h_

#include <SDL.h>
#include <stdbool.h>

typedef struct Piece Piece;

typedef struct Square Square;

typedef struct MouseEvent {
    bool LMBDown;
    SDL_Point mousePos;
    SDL_Point offset;
    Piece *piece;
    SDL_Point oldPos;
    Square *square;
} MouseEvent;

typedef struct {
    SDL_Window *win;
    SDL_Renderer *rend;
} Window;

extern Window mainWindow;

void initialise();

#endif
