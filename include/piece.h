#ifndef _PIECE_H_
#define _PIECE_H_

#include "common.h"

typedef struct Piece Piece;

bool canMove(Piece piece, SDL_Point oldPos, SDL_Point mousePos);

void makePieces();

void drawPieces();

void cleanUpPieces();

void checkIfPiece(SDL_Point *mousePos, SDL_Point *clickOffset, Piece *piece);

#endif
