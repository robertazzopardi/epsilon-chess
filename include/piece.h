#ifndef _PIECE_H_
#define _PIECE_H_

#include <stdbool.h>

typedef struct SDL_Rect SDL_Rect;
typedef struct SDL_Texture SDL_Texture;
typedef struct SDL_Point SDL_Point;

typedef struct Piece Piece;
typedef struct Player Player;
typedef struct Board Board;
typedef struct Window Window;
typedef struct MouseEvent MouseEvent;

typedef struct {
    int x;
    int y;
} Square;

typedef struct {
    Square *squares;
    int count;
} Moves;

typedef struct {
    SDL_Point *p2;
    Player *player;
    Player *opposition;
    Piece *piece;
} MoveConditions;

typedef struct Piece {
    char initial;
    SDL_Texture *texture;
    SDL_Rect *rect;
    bool firstMove;
    char player;
    Moves *(*canMove)(MoveConditions *);
    Moves *moves;
} Piece;

void generateMoves(MouseEvent *, Board *);

bool canMovePiece(MouseEvent *);

// void printPieces(Board *);

void makePieces(Window *, Board *);

void drawPieces(Window *, Board *);

void cleanUpPieces(Board *);

void checkIfPiece(MouseEvent *, Player *);

void alignPiece(MouseEvent *, Board *);

#endif
