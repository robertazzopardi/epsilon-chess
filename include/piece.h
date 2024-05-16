#ifndef PIECE_H
#define PIECE_H

#include <SDL_rect.h>

typedef struct SDL_Texture SDL_Texture;
typedef struct SDL_Point SDL_Point;
typedef struct SDL_Renderer SDL_Renderer;

typedef struct Piece Piece;
typedef struct Window Window;
typedef struct State State;

typedef struct Piece {
    SDL_Texture *texture;
    SDL_Rect rect;
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

void make_pieces(Piece *, PieceTextureMap *, State *);

void draw_pieces(Window *, State *, Piece *);

void check_if_piece(SDL_Point *, SDL_Point *, State *, Piece[], Piece **);

#endif /* PIECE_H */
