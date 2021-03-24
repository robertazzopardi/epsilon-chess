#ifndef _BOARD_H_
#define _BOARD_H_

#include "common.h"

typedef struct Square
{
    SDL_Rect rect;
    SDL_Color colour;
    bool selected;
} Square;

typedef struct Piece
{
    char initial;
    SDL_Texture *texture;
    SDL_Rect *rect;
    bool firstMove;
    char player;
} Piece;

typedef struct
{
    Piece *pieces;
    int count;
} Player;

typedef struct
{
    Player p1;
    Player p2;
    unsigned int moveCount;
    Square squares[ROW_COUNT][ROW_COUNT];
} Board;

extern Board board;

void makeBoard();

void drawBoard();

void toggleBoardSquare(const SDL_Point *mousePos, Square **square);

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
