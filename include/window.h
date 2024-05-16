#ifndef WINDOW_H
#define WINDOW_H

typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;

typedef struct Board Board;

typedef struct Window {
    SDL_Window *win;
    SDL_Renderer *rend;
    Board *board;
} Window;

void initialise();

#endif /* WINDOW_H */
