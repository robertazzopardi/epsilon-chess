#ifndef PIECE_H
#define PIECE_H

#include <stdbool.h>

#define PLAYER_1 1
#define PLAYER_2 2

typedef struct SDL_Rect SDL_Rect;
typedef struct SDL_Texture SDL_Texture;
typedef struct SDL_Point SDL_Point;
typedef struct SDL_Renderer SDL_Renderer;

typedef struct Piece Piece;
typedef struct Player Player;
typedef struct Board Board;
typedef struct Window Window;
typedef struct MoveConditions MoveConditions;
typedef struct State State;

typedef struct Moves {
    SDL_Point *squares;
    unsigned char count;
} Moves;

typedef struct Piece {
    SDL_Texture *texture;
    SDL_Rect *rect;
} Piece;

typedef struct PieceTexture PieceTexture;

typedef struct {
    PieceTexture *pawn;
    PieceTexture *rook;
    PieceTexture *knight;
    PieceTexture *bishop;
    PieceTexture *queen;
    PieceTexture *king;
} PieceTextureMap;

PieceTextureMap new_texture_map(SDL_Renderer *);

void clean_up_texture_map(PieceTextureMap *);

void calculateIfMovesAreValid(Piece *, Player *);

void generatePieceMoves(Piece *, Player *);

bool canMovePiece();

void make_pieces(Piece *, PieceTextureMap *, State *);

typedef struct State State;

void draw_pieces(Window *, State *, PieceTextureMap *, Piece *);

void check_if_piece(SDL_Point *, SDL_Point *, Piece *, State *);

void align_piece(Board *);

#endif /* PIECE_H */
