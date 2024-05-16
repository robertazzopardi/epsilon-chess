#include <SDL_rect.h>
#include <SDL_render.h>
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

#define ROOK_MAX 14
#define BISHOP_MAX 13
#define KNIGHT_MAX 8
#define PAWN_MAX 3
#define QUEEN_MAX 27
#define KING_MAX 10

struct MoveConditions {
    SDL_Point *pieceLocation;
    Player *player;
    Piece *piece;
};

// static bool canCastle(MoveConditions *mc, Moves *moves) {
//     int y = mc->pieceLocation->y;
//     for (int i = 1; i < ROW_COUNT - mc->pieceLocation->x; i++) {
//         int x = mc->pieceLocation->x + i;
//         Initial initial = checkWhichPiece(x, y, mc->player);
//         if (initial != rook && initial != empty) {
//             break;
//         } else if (initial == rook) {
//             moves->squares[moves->count++] = (SDL_Point){x - 1, y};
//         }
//     }
//
//     for (int i = 1; i <= mc->pieceLocation->x; i++) {
//         int x = mc->pieceLocation->x - i;
//         Initial initial = checkWhichPiece(x, y, mc->player);
//         if (initial != rook && initial != empty) {
//             break;
//         } else if (initial == rook) {
//             moves->squares[moves->count++] = (SDL_Point){x + 2, y};
//         }
//     }
//
//     return false;
// }

void align_piece(__unused Board *board) {
    // SDL_Point point = {event->mousePos->x, event->mousePos->y};
    //
    // int x = (point.x / SQUARE_SIZE) * SQUARE_SIZE;
    // int y = (point.y / SQUARE_SIZE) * SQUARE_SIZE;
    //
    // event->piece->rect->x = x;
    // event->piece->rect->y = y;
    //
    // Player *player = getPlayer(event->piece->player, board);
    //
    // // Castling
    // bool direction =
    //     ROW_COUNT - (x / SQUARE_SIZE) < (x / SQUARE_SIZE) ? true : false;
    // if (event->piece->initial == king && event->piece->firstMove) {
    //     int m = -2, v = SQUARE_SIZE;
    //     if (direction) {
    //         m = 1;
    //         v = -SQUARE_SIZE;
    //     }
    //
    //     for (int i = 0; i < player->piecesRemaining; i++) {
    //         if ((x / SQUARE_SIZE) + m ==
    //                 player->pieces[i].rect->x / SQUARE_SIZE &&
    //             (y / SQUARE_SIZE) == player->pieces[i].rect->y / SQUARE_SIZE)
    //             { player->pieces[i].rect->x = x + v;
    //         }
    //     }
    // }
    //
    // // Check if moved piece is on another piece
    // for (int i = 0; i < player->opposition->piecesRemaining; i++) {
    //     // Capture the oppositions piece
    //     if (player->opposition->pieces[i].rect->x == x &&
    //         player->opposition->pieces[i].rect->y == y &&
    //         SDL_PointInRect(&point, player->opposition->pieces[i].rect)) {
    //
    //         free(player->opposition->pieces[i].rect);
    //
    //         for (int j = i; j < player->opposition->piecesRemaining - 1; j++)
    //             player->opposition->pieces[j] =
    //                 player->opposition->pieces[j + 1];
    //
    //         player->opposition->piecesRemaining--;
    //
    //         break;
    //     }
    // }
}

bool canMovePiece() {
    // const int mPosX = event->mousePos->x / SQUARE_SIZE;
    // const int mPosY = event->mousePos->y / SQUARE_SIZE;
    //
    // for (int i = 0; i < event->piece->moves->count; i++) {
    //     if (mPosX == event->piece->moves->squares[i].x &&
    //         mPosY == event->piece->moves->squares[i].y)
    //         return true;
    // }

    return true;
}

static inline SDL_Rect get_piece_rect(int sq) {
    Location loc = get_location(sq);
    return (SDL_Rect){loc.file * SQUARE_SIZE, loc.rank * SQUARE_SIZE,
                      SQUARE_SIZE, SQUARE_SIZE};
}

Piece *check_if_piece(SDL_Point *mouse_pos, SDL_Point *offset, State *game,
                      Piece pieces[]) {
    for (int sq = 0; sq < BOARD_SIZE; sq++) {
        SDL_Rect rect = get_piece_rect(sq);
        if (SDL_PointInRect(mouse_pos, &rect) &&
            game->all_pieces & (1ULL << sq)) {

            printf("Found piece\n");

            // assign the piece to the pointer
            // piece = &pieces[sq];

            offset->x = mouse_pos->x - rect.x;

            offset->y = mouse_pos->y - rect.y;

            return &pieces[sq];
            // break;
        }
    }

    return NULL;
}

PieceTextureMap new_texture_map(SDL_Renderer *renderer) {
    PieceTexture *pawn = getTexture(renderer, PAWN_IMG);
    PieceTexture *rook = getTexture(renderer, ROOK_IMG);
    PieceTexture *knight = getTexture(renderer, KNIGHT_IMG);
    PieceTexture *bishop = getTexture(renderer, BISHOP_IMG);
    PieceTexture *queen = getTexture(renderer, QUEEN_IMG);
    PieceTexture *king = getTexture(renderer, KING_IMG);

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
