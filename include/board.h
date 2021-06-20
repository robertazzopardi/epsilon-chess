#ifndef _BOARD_H_
#define _BOARD_H_

#include <stdbool.h>

#define WIDTH 800
#define HEIGHT 800
#define ROW_COUNT 8

typedef struct SDL_Rect SDL_Rect;
typedef struct SDL_Texture SDL_Texture;
typedef struct SDL_Point SDL_Point;

typedef struct Piece Piece;
typedef struct Board Board;
typedef struct Window Window;

typedef struct Player {
    Piece *pieces;
    int count;
} Player;

struct Board {
    Player *p1;
    Player *p2;
    unsigned moveCount;
    char toMove;
    SDL_Texture *texture;
    SDL_Rect *rect;
    SDL_Rect *selectedRect;
    bool selectedVisible;
    Piece **pieces;
};

Board *makeBoard(Window *);

void cleanUpBoard(Board *);

#endif
