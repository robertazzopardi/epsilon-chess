#include "piece.h"
#include "board.h"
#include "svgutil.h"
#include "window.h"
#include <SDL_rect.h>
#include <SDL_render.h>

#define PAWN_IMG "./resources/864630-chess/svg/pieces/pawn.svg"
#define ROOK_IMG "./resources/864630-chess/svg/pieces/rook.svg"
#define KNIGHT_IMG "./resources/864630-chess/svg/pieces/horse.svg"
#define BISHOP_IMG "./resources/864630-chess/svg/pieces/bishop.svg"
#define QUEEN_IMG "./resources/864630-chess/svg/pieces/queen.svg"
#define KING_IMG "./resources/864630-chess/svg/pieces/king.svg"

#define PAWN 'P'
#define KING 'K'
#define QUEEN 'Q'
#define BISHOP 'B'
#define KNIGHT 'N'
#define ROOK 'R'
#define EMPTY '_'

#define PLAYER_1 '1'
#define PLAYER_2 '2'

#define PIECE_COUNT 16

// #define MAX(x, y) (((x) > (y)) ? (x) : (y))
// #define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define GET_PLAYER(player, board) (player == PLAYER_1 ? board->p1 : board->p2)
#define GET_OPPOSITION(player, board)                                          \
    (player == PLAYER_1 ? board->p2 : board->p1)

const int X[8] = {2, 1, -1, -2, -2, -1, 1, 2};
const int Y[8] = {1, 2, 2, 1, -1, -2, -2, -1};

void removeArrayElement(Piece *array, int index, int array_length) {
    for (int i = index; i < array_length - 1; i++)
        array[i] = array[i + 1];
}

// char checkPieceOnPiece(const SDL_Point *point, const Player *player) {
//     for (int i = 0; i < player->count; i++)
//         if (SDL_PointInRect(point, player->pieces[i].rect))
//             return 1;

//     return 0;
// }

char checkPieceOnSameTeam(const int x, const int y, const Player *player) {
    for (int j = 0; j < player->count; j++) {
        SDL_Point pos =
            getPos(player->pieces[j].rect->x, player->pieces[j].rect->y);
        if (x == pos.x && y == pos.y)
            return 1;
    }
    return 0;
}

void checkIfFirstMove(MouseEvent *mEvent, Board *board) {
    Player *player = GET_PLAYER(mEvent->piece->player, board);
    for (int i = 0; i < player->count; i++) {
        if (SDL_RectEquals(player->pieces[i].rect, mEvent->piece->rect)) {
            player->pieces[i].firstMove = 0;
            break;
        }
    }
}

static inline int getDistance(const SDL_Point *p1, const SDL_Point *p2) {
    return (p2->x - p1->x) + (p2->y - p1->y);
}

// static inline int getDistanceAbs(const SDL_Point *p1, const SDL_Point *p2) {
//     return abs(p2->x - p1->x) + abs(p2->y - p1->y);
// }

static inline char distanceEqual(const SDL_Point *p1, const SDL_Point *p2) {
    return abs(p2->x - p1->x) == abs(p2->y - p1->y);
}

char canMoveBishop(MoveConditions *mc) {
    for (int i = 1; i <= getDistance(mc->p1, mc->p2) / 2; i++) {
        if (mc->p2->x > mc->p1->x && mc->p2->y > mc->p1->y &&
            checkPieceOnSameTeam(mc->p1->x + i, mc->p1->y + i, mc->player))
            return 0;
        else if (mc->p2->x > mc->p1->x && mc->p2->y < mc->p1->y &&
                 checkPieceOnSameTeam(mc->p1->x + i, mc->p1->y - i, mc->player))
            return 0;
        else if (mc->p2->x < mc->p1->x && mc->p2->y > mc->p1->y &&
                 checkPieceOnSameTeam(mc->p1->x - i, mc->p1->y + i, mc->player))
            return 0;
        else if (mc->p2->x < mc->p1->x && mc->p2->y < mc->p1->y &&
                 checkPieceOnSameTeam(mc->p1->x - i, mc->p1->y + i, mc->player))
            return 0;
    }

    return distanceEqual(mc->p1, mc->p2);
}

char canMoveRook(MoveConditions *mc) {
    if (mc->p1->x != mc->p2->x && mc->p1->y != mc->p2->y)
        return 0;

    for (int i = 1; i <= getDistance(mc->p1, mc->p2); i++) {
        if (mc->p2->x > mc->p1->x &&
            checkPieceOnSameTeam(mc->p1->x + i, mc->p1->y, mc->player))
            return 0;
        else if (mc->p2->x < mc->p1->x &&
                 checkPieceOnSameTeam(mc->p1->x - i, mc->p1->y, mc->player))
            return 0;
        else if (mc->p2->y > mc->p1->y &&
                 checkPieceOnSameTeam(mc->p1->x, mc->p1->y + i, mc->player))
            return 0;
        else if (mc->p2->y < mc->p1->y &&
                 checkPieceOnSameTeam(mc->p1->x, mc->p1->y - i, mc->player))
            return 0;
    }

    return !canMoveBishop(mc);
}

char canMoveKnight(MoveConditions *mc) {
    for (int i = 0; i < ROW_COUNT; i++)
        if (mc->p2->x + X[i] == mc->p1->x && mc->p2->y + Y[i] == mc->p1->y)
            return !checkPieceOnSameTeam(mc->p2->x, mc->p2->y, mc->player);
    return 0;
}

// static inline char checkDifference(const int mouseY, const int oldY,
//                                    const char player) {
//     return player == PLAYER_1 ? mouseY - oldY == 1 : oldY - mouseY == 1;
// }

char canMovePawn(MoveConditions *mc) {
    const int d = getDistance(mc->p1, mc->p2);
    char canMove = 0;

    switch (*mc->initial) {
    case PLAYER_1:
        if (d == 1 || (d == 2 && mc->firstMove))
            canMove = 1;
        break;
    case PLAYER_2:
        if (d == -1 || (d == -2 && mc->firstMove))
            canMove = 1;
        break;
    default:
        break;
    }

    // Check if piece is on the same position as another
    Player *player = GET_OPPOSITION(*mc->initial, mc->board);
    for (int i = 0; i < player->count; i++)
        if (SDL_PointInRect(mc->mousePos, player->pieces[i].rect))
            canMove = 0;

    // printf("%d %d\n", d, canMove);
    return canMove;
}

char canMoveQueen(MoveConditions *mc) {
    return canMoveBishop(mc) || canMoveRook(mc);
}

char canMoveKing(MoveConditions *mc) {
    return (canMoveBishop(mc) && getDistance(mc->p1, mc->p2) == 2) ||
           (canMoveRook(mc) && getDistance(mc->p1, mc->p2) == 1);
}

char canMove(MouseEvent *mEvent, struct Board *board) {
    SDL_Point p1 = getPos(mEvent->oldPos->x, mEvent->oldPos->y);
    SDL_Point p2 = getPos(mEvent->mousePos->x, mEvent->mousePos->y);

    Player *player = GET_PLAYER(mEvent->piece->player, board);

    // printf("%c\n", mEvent->piece->initial);

    // return mEvent->piece->canMove(&p1, &p2, player);
    return mEvent->piece->canMove(
        &(MoveConditions){&p1, &p2, player, mEvent->mousePos, board,
                          &mEvent->piece->firstMove, &mEvent->piece->player});
}

void alignPiece(MouseEvent *mEvent, Board *board) {
    SDL_Point point = {mEvent->piece->rect->x + (mEvent->piece->rect->w / 2),
                       mEvent->piece->rect->y + (mEvent->piece->rect->h / 2)};

    // printf("%d %d\n", mEvent->mousePos->x, mEvent->mousePos->y);

    int x = (point.x / 100) * 100;
    int y = (point.y / 100) * 100;

    mEvent->piece->rect->x = x;
    mEvent->piece->rect->y = y;

    Player *opposition = GET_OPPOSITION(mEvent->piece->player, board);

    // Check if moved piece is on another piece
    for (int i = 0; i < opposition->count; i++) {
        if (opposition->pieces[i].rect->x == x &&
            opposition->pieces[i].rect->y == y) {

            // Capture the oppositions piece
            if (SDL_PointInRect(&point, opposition->pieces[i].rect)) {
                free(opposition->pieces[i].rect);
                removeArrayElement(opposition->pieces, i, opposition->count);
                opposition->count--;
            }
            break;
        }
    }
}

void checkIfPiece(MouseEvent *mEvent, Player *player) {
    for (int i = 0; i < player->count; i++) {
        if (SDL_PointInRect(mEvent->mousePos, player->pieces[i].rect)) {
            *mEvent->piece = player->pieces[i];
            mEvent->offset->x = mEvent->mousePos->x - player->pieces[i].rect->x;
            mEvent->offset->y = mEvent->mousePos->y - player->pieces[i].rect->y;

            break;
        }
    }
}

// void printPieces(Board board) {
//     for (int i = 0; i < board.p1.count; i++) {
//         printf("%c", board.p1.pieces[i].initial);
//     }
//     printf("\n");
//     for (int i = 0; i < board.p2.count; i++) {
//         printf("%c", board.p2.pieces[i].initial);
//     }

//     printf("\n");
// }

Piece makePiece(const char initial, SDL_Texture *texture, const SDL_Point *pos,
                const char player, void *canMoveFunc) {
    int w = 0;
    int h = 0;

    Piece piece = {.initial = initial, .player = player, .texture = texture};

    SDL_QueryTexture(piece.texture, NULL, NULL, &w, &h);

    SDL_Rect *rect = NULL;
    rect = calloc(1, sizeof *rect);
    rect->h = HEIGHT / ROW_COUNT;
    rect->w = WIDTH / ROW_COUNT;
    rect->x = pos->x;
    rect->y = pos->y;

    piece.rect = rect;
    piece.firstMove = 1;

    piece.canMove = canMoveFunc;

    return piece;
}

void makePieces(Window *mainWindow, Board *board) {
    TwoToneTexture *pawnTexture = getTexture(mainWindow->rend, PAWN_IMG);
    TwoToneTexture *rookTexture = getTexture(mainWindow->rend, ROOK_IMG);
    TwoToneTexture *knightTexture = getTexture(mainWindow->rend, KNIGHT_IMG);
    TwoToneTexture *bishopTexture = getTexture(mainWindow->rend, BISHOP_IMG);
    TwoToneTexture *queenTexture = getTexture(mainWindow->rend, QUEEN_IMG);
    TwoToneTexture *kingTexture = getTexture(mainWindow->rend, KING_IMG);

    board->p1->count = PIECE_COUNT;
    board->p2->count = PIECE_COUNT;

    board->p1->pieces = calloc(PIECE_COUNT, sizeof *board->p1->pieces);
    board->p2->pieces = calloc(PIECE_COUNT, sizeof *board->p2->pieces);

    SDL_Point p1BackRow;
    SDL_Point p2BackRow;

    for (int i = 0; i < ROW_COUNT; i++) {
        p1BackRow.x = i * 100;
        p1BackRow.y = HEIGHT - 800;

        p2BackRow.x = i * 100;
        p2BackRow.y = HEIGHT - 100;

        switch (i) {
        case 0:
        case 7:
            board->p1->pieces[i] = makePiece(ROOK, rookTexture->black,
                                             &p1BackRow, PLAYER_1, canMoveRook);
            board->p2->pieces[i + ROW_COUNT] = makePiece(
                ROOK, rookTexture->white, &p2BackRow, PLAYER_2, canMoveRook);
            break;
        case 1:
        case 6:
            board->p1->pieces[i] =
                makePiece(KNIGHT, knightTexture->black, &p1BackRow, PLAYER_1,
                          canMoveKnight);
            board->p2->pieces[i + ROW_COUNT] =
                makePiece(KNIGHT, knightTexture->white, &p2BackRow, PLAYER_2,
                          canMoveKnight);
            break;
        case 2:
        case 5:
            board->p1->pieces[i] =
                makePiece(BISHOP, bishopTexture->black, &p1BackRow, PLAYER_1,
                          canMoveBishop);
            board->p2->pieces[i + ROW_COUNT] =
                makePiece(BISHOP, bishopTexture->white, &p2BackRow, PLAYER_2,
                          canMoveBishop);
            break;
        case 3:
            board->p1->pieces[i] = makePiece(
                QUEEN, queenTexture->black, &p1BackRow, PLAYER_1, canMoveQueen);
            board->p2->pieces[i + ROW_COUNT] = makePiece(
                QUEEN, queenTexture->white, &p2BackRow, PLAYER_2, canMoveQueen);
            break;
        case 4:
            board->p1->pieces[i] = makePiece(KING, kingTexture->black,
                                             &p1BackRow, PLAYER_1, canMoveKing);
            board->p2->pieces[i + ROW_COUNT] = makePiece(
                KING, kingTexture->white, &p2BackRow, PLAYER_2, canMoveKing);
            break;
        default:
            break;
        }

        board->p1->pieces[i + ROW_COUNT] =
            makePiece(PAWN, pawnTexture->black,
                      &(SDL_Point){.x = p1BackRow.x, .y = HEIGHT - 700},
                      PLAYER_1, canMovePawn);
        board->p2->pieces[i] =
            makePiece(PAWN, pawnTexture->white,
                      &(SDL_Point){.x = p2BackRow.x, .y = HEIGHT - 200},
                      PLAYER_2, canMovePawn);
    }

    free(pawnTexture);
    free(rookTexture);
    free(kingTexture);
    free(knightTexture);
    free(bishopTexture);
    free(queenTexture);
    pawnTexture = NULL;
    rookTexture = NULL;
    knightTexture = NULL;
    bishopTexture = NULL;
    queenTexture = NULL;
    kingTexture = NULL;
}

void drawPieces(Window *mainWindow, Board *board) {
    for (int i = 0; i < board->p1->count; i++)
        SDL_RenderCopy(mainWindow->rend, board->p1->pieces[i].texture, NULL,
                       board->p1->pieces[i].rect);
    for (int i = 0; i < board->p2->count; i++)
        SDL_RenderCopy(mainWindow->rend, board->p2->pieces[i].texture, NULL,
                       board->p2->pieces[i].rect);
}

void cleanUpPlayer(Player *player) {
    printf("%d\n", player->count);
    for (int i = 0; i < player->count; i++) {
        SDL_DestroyTexture(player->pieces[i].texture);
        player->pieces[i].texture = NULL;

        free(player->pieces[i].rect);
        player->pieces[i].rect = NULL;
    }
}

void cleanUpPieces(Board *board) {
    cleanUpPlayer(board->p1);
    cleanUpPlayer(board->p2);

    free(board->p1->pieces);
    free(board->p2->pieces);
    board->p1->pieces = NULL;
    board->p2->pieces = NULL;
}
