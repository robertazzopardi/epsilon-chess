#include <SDL_rect.h>
#include <SDL_render.h>
#include <stdbool.h>
#include <stdlib.h>

#include "board.h"
#include "engine.h"
#include "piece.h"
#include "svg_util.h"
#include "window.h"

#define PAWN_IMG "./assets/864630-chess/svg/pieces/pawn.svg"
#define ROOK_IMG "./assets/864630-chess/svg/pieces/rook.svg"
#define KNIGHT_IMG "./assets/864630-chess/svg/pieces/horse.svg"
#define BISHOP_IMG "./assets/864630-chess/svg/pieces/bishop.svg"
#define QUEEN_IMG "./assets/864630-chess/svg/pieces/queen.svg"
#define KING_IMG "./assets/864630-chess/svg/pieces/king.svg"

static inline SDL_Rect get_piece_rect(int sq) {
    Location loc = get_location(sq);
    return (SDL_Rect){loc.file * SQUARE_SIZE, loc.rank * SQUARE_SIZE,
                      SQUARE_SIZE, SQUARE_SIZE};
}

void check_if_piece(SDL_Point *mouse_pos, SDL_Point *offset, State *game,
                    PieceObject pieces[], PieceObject **piece) {
    for (Square sq = A1; sq <= H8; sq++) {
        SDL_Rect rect = get_piece_rect(sq);
        if (SDL_PointInRect(mouse_pos, &rect) &&
            game->all_pieces & (1ULL << sq)) {

            offset->x = mouse_pos->x - rect.x;
            offset->y = mouse_pos->y - rect.y;

            *piece = &pieces[sq];

            break;
        }
    }
}

void can_move(Move **move, State *game, SDL_Point *mouse_pos,
              SDL_Point *old_pos) {
    for (int sq = 0; sq < MAX_MOVES; sq++) {
        Square from_sq =
            get_square(old_pos->y / SQUARE_SIZE, old_pos->x / SQUARE_SIZE);
        Square to_sq =
            get_square(mouse_pos->y / SQUARE_SIZE, mouse_pos->x / SQUARE_SIZE);

        if (from_sq == game->moves[sq].from && to_sq == game->moves[sq].to) {
            *move = &game->moves[sq];
            return;
        }
    }
}

PieceTextureMap new_texture_map(SDL_Renderer *renderer) {
    PieceTexture *pawn = get_texture(renderer, PAWN_IMG);
    PieceTexture *rook = get_texture(renderer, ROOK_IMG);
    PieceTexture *knight = get_texture(renderer, KNIGHT_IMG);
    PieceTexture *bishop = get_texture(renderer, BISHOP_IMG);
    PieceTexture *queen = get_texture(renderer, QUEEN_IMG);
    PieceTexture *king = get_texture(renderer, KING_IMG);

    return (PieceTextureMap){pawn, rook, knight, bishop, queen, king};
}

static inline void free_texture(SDL_Texture *texture) {
    SDL_DestroyTexture(texture);
    texture = NULL;
}

static inline void free_map(PieceTexture *texture) {
    free(texture);
    texture = NULL;
}

void clean_up_texture_map(PieceTextureMap *map) {
    free_texture(map->pawn->white);
    free_texture(map->pawn->black);
    free_texture(map->rook->white);
    free_texture(map->rook->black);
    free_texture(map->knight->white);
    free_texture(map->knight->black);
    free_texture(map->bishop->white);
    free_texture(map->bishop->black);
    free_texture(map->queen->white);
    free_texture(map->queen->black);
    free_texture(map->king->white);
    free_texture(map->king->black);

    free_map(map->pawn);
    free_map(map->rook);
    free_map(map->king);
    free_map(map->knight);
    free_map(map->bishop);
    free_map(map->queen);
}

void make_pieces(PieceObject *pieces, PieceTextureMap *map, State *game) {
    for (Square sq = A1; sq <= H8; sq++) {
        SDL_Rect rect = get_piece_rect(sq);
        if (game->bit_boards[0] & (1ULL << sq)) {
            pieces[sq] = (PieceObject){map->pawn->white, rect};
        } else if (game->bit_boards[6] & (1ULL << sq)) {
            pieces[sq] = (PieceObject){map->pawn->black, rect};
        } else if (game->bit_boards[1] & (1ULL << sq)) {
            pieces[sq] = (PieceObject){map->knight->white, rect};
        } else if (game->bit_boards[7] & (1ULL << sq)) {
            pieces[sq] = (PieceObject){map->knight->black, rect};
        } else if (game->bit_boards[2] & (1ULL << sq)) {
            pieces[sq] = (PieceObject){map->bishop->white, rect};
        } else if (game->bit_boards[8] & (1ULL << sq)) {
            pieces[sq] = (PieceObject){map->bishop->black, rect};
        } else if (game->bit_boards[3] & (1ULL << sq)) {
            pieces[sq] = (PieceObject){map->rook->white, rect};
        } else if (game->bit_boards[9] & (1ULL << sq)) {
            pieces[sq] = (PieceObject){map->rook->black, rect};
        } else if (game->bit_boards[4] & (1ULL << sq)) {
            pieces[sq] = (PieceObject){map->king->white, rect};
        } else if (game->bit_boards[10] & (1ULL << sq)) {
            pieces[sq] = (PieceObject){map->king->black, rect};
        } else if (game->bit_boards[5] & (1ULL << sq)) {
            pieces[sq] = (PieceObject){map->queen->white, rect};
        } else if (game->bit_boards[11] & (1ULL << sq)) {
            pieces[sq] = (PieceObject){map->queen->black, rect};
        }
    }
}

void render_if_occupied(Window *window, State *game, PieceObject *piece,
                        int sq) {
    for (int i = 0; i < PIECE_TYPE_COUNT; i++) {
        if (game->bit_boards[i] & (1ULL << sq)) {
            SDL_RenderCopy(window->rend, piece->texture, NULL, &piece->rect);
            break;
        }
    }
}

void draw_pieces(Window *window, State *game, PieceObject *pieces) {
    for (Square sq = A1; sq <= H8; sq++) {
        PieceObject piece = pieces[sq];
        render_if_occupied(window, game, &piece, sq);
    }
}
