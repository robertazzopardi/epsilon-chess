#ifndef _BOARD_H_
#define _BOARD_H_

#include "common.h"

typedef struct
{
    SDL_Rect rect;
    SDL_Color colour;
} Square;

typedef struct
{
    Square squares[SQUARES_SIZE][SQUARES_SIZE];
} Board;

Board makeBoard();

void drawBoard(SDL_Renderer *renderer, Board *board);

#endif
