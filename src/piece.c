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

    return false;
}

static inline SDL_Rect get_piece_rect(int sq) {
    Location loc = get_location(sq);
    return (SDL_Rect){loc.file * SQUARE_SIZE, loc.rank * SQUARE_SIZE,
                      SQUARE_SIZE, SQUARE_SIZE};
}

void check_if_piece(SDL_Point *mouse_pos, __unused SDL_Point *offset,
                    __unused Piece *piece, State *game) {
    for (int sq = 0; sq < BOARD_SIZE; sq++) {
        SDL_Rect rect = get_piece_rect(sq);
        if (SDL_PointInRect(mouse_pos, &rect) &&
            game->all_pieces & (1ULL << sq)) {

            // printf("Found piece\n");

            // piece = &player->pieces[i];
            // offset->x = mouse_pos->x - player->pieces[i].rect->x;
            //
            // offset->y = mouse_pos->y - player->pieces[i].rect->y;

            break;
        }
    }
}

// static Piece makePiece(char initial, SDL_Texture *texture, SDL_Point *pos,
//                        char player, unsigned char maxPossibleMoves,
//                        void *canMoveFunc, char *directionsX, char
//                        *directionsY, char directions) {
//
//     SDL_Rect *rect = malloc(1 * sizeof *rect);
//
//     rect->h = HEIGHT / ROW_COUNT;
//     rect->w = WIDTH / ROW_COUNT;
//
//     rect->x = pos->x;
//     rect->y = pos->y;
//
//     Moves *moves = malloc(1 * sizeof(*moves));
//     moves->squares = malloc(maxPossibleMoves * sizeof(*moves->squares));
//     moves->count = 0;
//
//     return (Piece){
//         .initial = initial,
//         .player = player,
//         .texture = texture,
//         .firstMove = true,
//         .canMove = canMoveFunc,
//         .rect = rect,
//         .maxPossibleMoves = maxPossibleMoves,
//         .directionsX = directionsX,
//         .directionsY = directionsY,
//         .directions = directions,
//         .moves = moves,
//     };
// }

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

void makePieces(__unused Window *window) {
    // PieceTexture *pawnTexture = getTexture(window->rend, PAWN_IMG);
    // PieceTexture *rookTexture = getTexture(window->rend, ROOK_IMG);
    // PieceTexture *knightTexture = getTexture(window->rend, KNIGHT_IMG);
    // PieceTexture *bishopTexture = getTexture(window->rend, BISHOP_IMG);
    // PieceTexture *queenTexture = getTexture(window->rend, QUEEN_IMG);
    // PieceTexture *kingTexture = getTexture(window->rend, KING_IMG);

    // window->board->p1->piecesRemaining = PIECE_COUNT;
    // window->board->p2->piecesRemaining = PIECE_COUNT;

    // window->board->p1->pieces =
    //     malloc(PIECE_COUNT * sizeof(*window->board->p1->pieces));
    // window->board->p2->pieces =
    //     malloc(PIECE_COUNT * sizeof(*window->board->p2->pieces));
    //
    // SDL_Point p1BackRow, p2BackRow;
    //
    // Piece player1, player2;
    //
    // // Init the pieces
    // for (int i = 0; i < ROW_COUNT; i++) {
    //     p1BackRow.x = i * SQUARE_SIZE;
    //     p1BackRow.y = 0;
    //
    //     p2BackRow.x = p1BackRow.x;
    //     p2BackRow.y = HEIGHT - SQUARE_SIZE;
    //
    //     switch (i) {
    //     case 0:
    //     case 7:
    //         player1 =
    //             makePiece(rook, rookTexture->black, &p1BackRow, PLAYER_1,
    //                       ROOK_MAX, canMoveQueenRookBishop, rookX, rookY, 4);
    //         player2 =
    //             makePiece(rook, rookTexture->white, &p2BackRow, PLAYER_2,
    //                       ROOK_MAX, canMoveQueenRookBishop, rookX, rookY, 4);
    //         break;
    //     case 1:
    //     case 6:
    //         player1 = makePiece(knight, knightTexture->black, &p1BackRow,
    //                             PLAYER_1, KNIGHT_MAX, canMoveKingKnight,
    //                             knightX, knightY, ROW_COUNT);
    //         player2 = makePiece(knight, knightTexture->white, &p2BackRow,
    //                             PLAYER_2, KNIGHT_MAX, canMoveKingKnight,
    //                             knightX, knightY, ROW_COUNT);
    //         break;
    //     case 2:
    //     case 5:
    //         player1 = makePiece(bishop, bishopTexture->black, &p1BackRow,
    //                             PLAYER_1, BISHOP_MAX, canMoveQueenRookBishop,
    //                             bishopX, bishopY, 4);
    //         player2 = makePiece(bishop, bishopTexture->white, &p2BackRow,
    //                             PLAYER_2, BISHOP_MAX, canMoveQueenRookBishop,
    //                             bishopX, bishopY, 4);
    //         break;
    //     case 3:
    //         player1 = makePiece(queen, queenTexture->black, &p1BackRow,
    //                             PLAYER_1, QUEEN_MAX, canMoveQueenRookBishop,
    //                             queenX, queenY, ROW_COUNT);
    //         player2 = makePiece(queen, queenTexture->white, &p2BackRow,
    //                             PLAYER_2, QUEEN_MAX, canMoveQueenRookBishop,
    //                             queenX, queenY, ROW_COUNT);
    //         break;
    //     case 4:
    //         player1 = makePiece(king, kingTexture->black, &p1BackRow,
    //         PLAYER_1,
    //                             KNIGHT_MAX, canMoveKingKnight, kingX, kingY,
    //                             ROW_COUNT);
    //         player2 = makePiece(king, kingTexture->white, &p2BackRow,
    //         PLAYER_2,
    //                             KNIGHT_MAX, canMoveKingKnight, kingX, kingY,
    //                             ROW_COUNT);
    //         break;
    //     default:
    //         break;
    //     }
    //
    //     window->board->p1->pieces[i] = player1;
    //     window->board->p2->pieces[i + ROW_COUNT] = player2;
    //
    //     // Make the pawns
    //     window->board->p1->pieces[i + ROW_COUNT] = makePiece(
    //         pawn, pawnTexture->black, &(SDL_Point){p1BackRow.x, HEIGHT -
    //         700}, PLAYER_1, PAWN_MAX, canMovePawn, pawnX, pawn1Y, 2);
    //     window->board->p2->pieces[i] = makePiece(
    //         pawn, pawnTexture->white, &(SDL_Point){p2BackRow.x, HEIGHT -
    //         200}, PLAYER_2, PAWN_MAX, canMovePawn, pawnX, pawn2Y, 2);
    // }
}

void draw_pieces(Window *window, State *game, PieceTextureMap *map) {
    for (int sq = 0; sq < BOARD_SIZE; sq++) {
        SDL_Rect rect = get_piece_rect(sq);
        if (game->bit_boards[0] & (1ULL << sq)) {
            SDL_RenderCopy(window->rend, map->pawn->white, NULL, &rect);
        } else if (game->bit_boards[6] & (1ULL << sq)) {
            SDL_RenderCopy(window->rend, map->pawn->black, NULL, &rect);
        } else if (game->bit_boards[1] & (1ULL << sq)) {
            SDL_RenderCopy(window->rend, map->knight->white, NULL, &rect);
        } else if (game->bit_boards[7] & (1ULL << sq)) {
            SDL_RenderCopy(window->rend, map->knight->black, NULL, &rect);
        } else if (game->bit_boards[2] & (1ULL << sq)) {
            SDL_RenderCopy(window->rend, map->bishop->white, NULL, &rect);
        } else if (game->bit_boards[8] & (1ULL << sq)) {
            SDL_RenderCopy(window->rend, map->bishop->black, NULL, &rect);
        } else if (game->bit_boards[3] & (1ULL << sq)) {
            SDL_RenderCopy(window->rend, map->rook->white, NULL, &rect);
        } else if (game->bit_boards[9] & (1ULL << sq)) {
            SDL_RenderCopy(window->rend, map->rook->black, NULL, &rect);
        } else if (game->bit_boards[4] & (1ULL << sq)) {
            SDL_RenderCopy(window->rend, map->king->white, NULL, &rect);
        } else if (game->bit_boards[10] & (1ULL << sq)) {
            SDL_RenderCopy(window->rend, map->king->black, NULL, &rect);
        } else if (game->bit_boards[5] & (1ULL << sq)) {
            SDL_RenderCopy(window->rend, map->queen->white, NULL, &rect);
        } else if (game->bit_boards[11] & (1ULL << sq)) {
            SDL_RenderCopy(window->rend, map->queen->black, NULL, &rect);
        }
    }
}
