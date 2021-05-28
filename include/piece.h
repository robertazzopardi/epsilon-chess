#ifndef _PIECE_H_
#define _PIECE_H_

#include <stdbool.h>

typedef struct Piece Piece;

typedef struct Player Player;

typedef struct MouseEvent MouseEvent;

bool canMove(MouseEvent *);

void makePieces();

void drawPieces();

void cleanUpPieces();

void checkIfPiece(MouseEvent *, Player *);

void alignPiece(MouseEvent *);

void checkIfFirstMove(const MouseEvent *);

#endif
