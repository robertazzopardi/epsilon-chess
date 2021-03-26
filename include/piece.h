#ifndef _PIECE_H_
#define _PIECE_H_

#include <SDL.h>
#include <stdbool.h>

#include "window.h"

/**
 * @brief Piece forward declaration
 *
 */
typedef struct Piece Piece;

/**
 * @brief Player forward declaration
 *
 */
typedef struct Player Player;

/**
 * @brief MouseEvent forward declaration
 *
 */
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

void checkIfPiece(MouseEvent *mEvent, Player *player);

void alignPiece(MouseEvent *mEvent);

void checkIfFirstMove(const MouseEvent *mEvent);

#endif
