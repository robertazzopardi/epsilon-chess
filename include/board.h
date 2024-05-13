#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>

#define SQUARE_SIZE 100
#define WIDTH 800
#define HEIGHT 800
#define ROW_COUNT 8

typedef struct SDL_Rect SDL_Rect;
typedef struct SDL_Texture SDL_Texture;
typedef struct SDL_Point SDL_Point;
typedef struct SDL_Renderer SDL_Renderer;

typedef struct Piece Piece;
typedef struct Player Player;

struct Player {
    Piece *pieces;
    unsigned char piecesRemaining;
    Player *opposition;
};

typedef struct Board {
    Player *p1;
    Player *p2;
    unsigned char moveCount;
    char toMove;
    SDL_Texture *texture;
    SDL_Rect *rect;
    SDL_Rect *selectedRect;
    bool selectedVisible;
} Board;

Board *makeBoard(SDL_Renderer *);

void generateMoves(Board *);

void cleanUpBoard(Board *);

#endif /* BOARD_H */
