#ifndef _PIECE_H_
#define _PIECE_H_

#include <SDL.h>
#include <stdbool.h>

#include "svgutil.h"
#include "window.h"

typedef struct Piece Piece;

typedef struct Player Player;

typedef struct MouseEvent MouseEvent;

bool canMove(MouseEvent *mEvent);

void makePieces();

void drawPieces();

void cleanUpPieces();

void checkIfPiece(MouseEvent *mEvent, Player *player);

void alignPiece(MouseEvent *mEvent);

void checkIfFirstMove(const MouseEvent *mEvent);

#endif
