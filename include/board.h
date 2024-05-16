#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>

#include "consts.h"

#define SQUARE_SIZE 100
#define WIDTH 800
#define HEIGHT 800
#define ROW_COUNT 8

typedef struct SDL_Rect SDL_Rect;
typedef struct SDL_Texture SDL_Texture;
typedef struct SDL_Point SDL_Point;
typedef struct SDL_Renderer SDL_Renderer;

typedef struct Board {
    char toMove;
    SDL_Texture *texture;
    SDL_Rect *rect;
    Square selected_square;
} Board;

Board *make_board(SDL_Renderer *);

void clean_up_board(Board *);

#endif /* BOARD_H */
