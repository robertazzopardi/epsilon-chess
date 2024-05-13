#ifndef WINDOW_H
#define WINDOW_H

#include <stdbool.h>

typedef struct SDL_Point SDL_Point;
typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;
typedef union SDL_Event SDL_Event;

typedef struct Piece Piece;
typedef struct Board Board;

typedef struct MouseEvent {
    bool LMBDown;
    SDL_Point *mousePos;
    SDL_Point *offset;
    Piece *piece;
    SDL_Point *oldPos;
} MouseEvent;

typedef struct Window {
    SDL_Window *win;
    SDL_Renderer *rend;
    Board *board;
    bool running;
} Window;

void initialise();

#endif /* WINDOW_H */
