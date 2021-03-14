#ifndef _PIECE_H_
#define _PIECE_H_

#include "common.h"

#define PAWN 'P'
#define KING 'K'
#define QUEEN 'Q'
#define BISHOP 'B'
#define KNIGHT 'N'
#define ROOK 'R'

#define PLAYER_1 '1'
#define PLAYER_2 '2'

typedef struct
{
    char initial;
    SDL_Texture *texture;
    SDL_Rect *rect;
    bool firstMove;
    char player;
} Piece;

bool canMove(Piece piece, SDL_Point oldPos);

void makePieces();

void drawPieces();

void cleanUpPieces();

void checkIfPiece(SDL_Point *mousePos, SDL_Point *clickOffset, Piece *piece);

#endif
