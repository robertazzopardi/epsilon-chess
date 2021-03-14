#ifndef _BOARD_H_
#define _BOARD_H_

#include "common.h"

typedef struct
{
    SDL_Rect rect;
    SDL_Color colour;
    bool selected;
} Square;

void makeBoard();

void drawBoard();

void toggleBoardSquare(SDL_Point *mousePos, SDL_Point *clickOffset, Square **square);

void alignPiece(SDL_Rect *rect);

static inline int getFirstDigit(int num)
{
    int len = floor(log10(num)) + 1;
    if (len <= 2)
    {
        return 0;
    }
    return num / (int)pow(10, len - 1);
}

#endif
