#ifndef _BOARD_H_
#define _BOARD_H_

#include <SDL.h>
#include <stdbool.h>

#define WIDTH 800
#define HEIGHT 800
#define ROW_COUNT 8

typedef struct Square {
    SDL_Rect rect;
    bool selected;
} Square;

typedef struct Piece {
    char initial;
    SDL_Texture *texture;
    SDL_Rect *rect;
    bool firstMove;
    char player;
} Piece;

typedef struct Player {
    Piece *pieces;
    int count;
} Player;

typedef struct {
    Player p1;
    Player p2;
    unsigned int moveCount;
    Square squares[ROW_COUNT][ROW_COUNT];
    SDL_Texture *texture;
    SDL_Rect *rect;
} Board;

extern Board board;

void makeBoard();

void drawBoard();

void toggleBoardSquare(const SDL_Point *, Square **);

SDL_Point getPos(int, int);

void cleanUpBoard();

#endif
