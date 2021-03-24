#ifndef _PIECE_H_
#define _PIECE_H_

#include "common.h"

typedef struct Piece Piece;

typedef struct MouseEvent MouseEvent;

/**
 * @brief Checks if the piece can move
 *
 * @param mEvent
 * @return true
 * @return false
 */
bool canMove(MouseEvent *mEvent);

void makePieces();

void drawPieces();

void cleanUpPieces();

void checkIfPiece(SDL_Point *mousePos, SDL_Point *clickOffset, Piece *piece);

void alignPiece(SDL_Rect *rect, const char player);

#endif
