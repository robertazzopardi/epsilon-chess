/**
 * @file piece.h
 * @author Robert Azzopardi-Yashi (robertazzopardi@icloud.com)
 * @brief
 * @version 0.1
 * @date 2021-06-21
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef _PIECE_H_
#define _PIECE_H_

#include <stdbool.h>

#define PLAYER_1 1
#define PLAYER_2 2

typedef struct SDL_Rect SDL_Rect;
typedef struct SDL_Texture SDL_Texture;
typedef struct SDL_Point SDL_Point;
typedef struct Piece Piece;
typedef struct Player Player;
typedef struct Board Board;
typedef struct Window Window;
typedef struct MouseEvent MouseEvent;

typedef struct {
    SDL_Point *squares;
    int count;
} Moves;

typedef struct {
    SDL_Point *p2;
    Player *player;
    Player *opposition;
    Piece *piece;
} MoveConditions;

typedef enum {
    pawn = 'P',
    king = 'K',
    queen = 'Q',
    bishop = 'B',
    knight = 'N',
    rook = 'R',
    empty = '_'
} Initial;

typedef struct Piece {
    Initial initial;
    SDL_Texture *texture;
    SDL_Rect *rect;
    bool firstMove;
    char player;
    Moves *(*canMove)(MoveConditions *);
    Moves *moves;
    int maxMoves;
    int *directionsX;
    int *directionsY;
    int directions;
} Piece;

void generateMoves(MouseEvent *, Board *);

bool canMovePiece(const MouseEvent *);

// void printPieces(Board *);

void makePieces(Window *);

void drawPieces(Window *, MouseEvent *);

void cleanUpPieces(Board *);

void checkIfPiece(MouseEvent *, Player *);

void alignPiece(MouseEvent *, Board *);

#endif
