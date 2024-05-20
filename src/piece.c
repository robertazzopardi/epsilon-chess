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

#define PIECE_COUNT 16

static inline SDL_Rect get_piece_rect(int sq) {
    Location loc = get_location(sq);
    return (SDL_Rect){loc.file * SQUARE_SIZE, loc.rank * SQUARE_SIZE,
                      SQUARE_SIZE, SQUARE_SIZE};
}

void check_if_piece(SDL_Point *mouse_pos, SDL_Point *offset, State *game,
                    Piece pieces[], Piece **piece) {
    for (int sq = 0; sq < BOARD_SIZE; sq++) {
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

bool can_move(State *game, SDL_Point *mouse_pos, SDL_Point *old_pos) {
    for (Square sq = 0; sq < MAX_MOVES; sq++) {
        Square from_sq =
            get_square(old_pos->y / SQUARE_SIZE, old_pos->x / SQUARE_SIZE);
        Square to_sq =
            get_square(mouse_pos->y / SQUARE_SIZE, mouse_pos->x / SQUARE_SIZE);

        if (from_sq == game->moves[sq].from && to_sq == game->moves[sq].to) {
            return true;
        }
    }

    return false;
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

void clean_up_texture_map(PieceTextureMap *map) {
    SDL_DestroyTexture(map->pawn->white);
    map->pawn->white = NULL;
    SDL_DestroyTexture(map->pawn->black);
    map->pawn->black = NULL;
    SDL_DestroyTexture(map->rook->white);
    map->rook->white = NULL;
    SDL_DestroyTexture(map->rook->black);
    map->rook->black = NULL;
    SDL_DestroyTexture(map->knight->white);
    map->knight->white = NULL;
    SDL_DestroyTexture(map->knight->black);
    map->knight->black = NULL;
    SDL_DestroyTexture(map->bishop->white);
    map->bishop->white = NULL;
    SDL_DestroyTexture(map->bishop->black);
    map->bishop->black = NULL;
    SDL_DestroyTexture(map->queen->white);
    map->queen->white = NULL;
    SDL_DestroyTexture(map->queen->black);
    map->queen->black = NULL;
    SDL_DestroyTexture(map->king->white);
    map->king->white = NULL;
    SDL_DestroyTexture(map->king->black);
    map->king->black = NULL;

    free(map->pawn);
    map->pawn = NULL;
    free(map->rook);
    map->rook = NULL;
    free(map->king);
    map->king = NULL;
    free(map->knight);
    map->knight = NULL;
    free(map->bishop);
    map->bishop = NULL;
    free(map->queen);
    map->queen = NULL;
}

void make_pieces(Piece *pieces, PieceTextureMap *map, State *game) {
    for (int sq = 0; sq < BOARD_SIZE; sq++) {
        SDL_Rect rect = get_piece_rect(sq);
        if (game->bit_boards[0] & (1ULL << sq)) {
            pieces[sq] = (Piece){map->pawn->white, rect};
        } else if (game->bit_boards[6] & (1ULL << sq)) {
            pieces[sq] = (Piece){map->pawn->black, rect};
        } else if (game->bit_boards[1] & (1ULL << sq)) {
            pieces[sq] = (Piece){map->knight->white, rect};
        } else if (game->bit_boards[7] & (1ULL << sq)) {
            pieces[sq] = (Piece){map->knight->black, rect};
        } else if (game->bit_boards[2] & (1ULL << sq)) {
            pieces[sq] = (Piece){map->bishop->white, rect};
        } else if (game->bit_boards[8] & (1ULL << sq)) {
            pieces[sq] = (Piece){map->bishop->black, rect};
        } else if (game->bit_boards[3] & (1ULL << sq)) {
            pieces[sq] = (Piece){map->rook->white, rect};
        } else if (game->bit_boards[9] & (1ULL << sq)) {
            pieces[sq] = (Piece){map->rook->black, rect};
        } else if (game->bit_boards[4] & (1ULL << sq)) {
            pieces[sq] = (Piece){map->king->white, rect};
        } else if (game->bit_boards[10] & (1ULL << sq)) {
            pieces[sq] = (Piece){map->king->black, rect};
        } else if (game->bit_boards[5] & (1ULL << sq)) {
            pieces[sq] = (Piece){map->queen->white, rect};
        } else if (game->bit_boards[11] & (1ULL << sq)) {
            pieces[sq] = (Piece){map->queen->black, rect};
        }
    }
}

void render_if_occupied(Window *window, State *game, Piece *piece, int sq) {
    for (int i = 0; i < PIECE_TYPE_COUNT; i++) {
        if (game->bit_boards[i] & (1ULL << sq)) {
            SDL_RenderCopy(window->rend, piece->texture, NULL, &piece->rect);
            break;
        }
    }
}

void draw_pieces(Window *window, State *game, Piece *pieces) {
    for (int sq = 0; sq < BOARD_SIZE; sq++) {
        Piece piece = pieces[sq];
        render_if_occupied(window, game, &piece, sq);
    }
}
