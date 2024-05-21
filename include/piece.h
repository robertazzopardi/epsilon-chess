#ifndef PIECE_H
#define PIECE_H

#include <SDL_rect.h>
#include <stdbool.h>

#include "engine.h"

typedef struct SDL_Texture SDL_Texture;
typedef struct SDL_Point SDL_Point;
typedef struct SDL_Renderer SDL_Renderer;

typedef struct PieceObject PieceObject;
typedef struct Window Window;
typedef struct State State;

typedef struct PieceObject {
    SDL_Texture *texture;
    SDL_Rect rect;
} PieceObject;

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

void make_pieces(PieceObject *, PieceTextureMap *, State *);

void draw_pieces(Window *, State *, PieceObject *);

void check_if_piece(SDL_Point *, SDL_Point *, State *, PieceObject[],
                    PieceObject **);

void can_move(Move **, State *, SDL_Point *, SDL_Point *);

#endif /* PIECE_H */
