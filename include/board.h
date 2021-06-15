#ifndef _BOARD_H_
#define _BOARD_H_

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
    char count;
} Player;

struct Board {
    Player *p1;
    Player *p2;
    unsigned char moveCount;
    SDL_Texture *texture;
    SDL_Rect *rect;
};

Board *makeBoard(Window *);

SDL_Point getPos(int, int);

void cleanUpBoard(Board *);

#endif
