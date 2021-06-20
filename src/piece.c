#include "piece.h"
#include "board.h"
#include "svgutil.h"
#include "window.h"
#include <SDL_rect.h>
#include <SDL_render.h>
#include <stdlib.h>

#define PAWN_IMG "./assets/864630-chess/svg/pieces/pawn.svg"
#define ROOK_IMG "./assets/864630-chess/svg/pieces/rook.svg"
#define KNIGHT_IMG "./assets/864630-chess/svg/pieces/horse.svg"
#define BISHOP_IMG "./assets/864630-chess/svg/pieces/bishop.svg"
#define QUEEN_IMG "./assets/864630-chess/svg/pieces/queen.svg"
#define KING_IMG "./assets/864630-chess/svg/pieces/king.svg"

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

#define ROOK_MAX 14
#define BISHOP_MAX 13
#define KNIGHT_MAX 8
#define PAWN_MAX 3
#define QUEEN_MAX 27
#define KING_MAX 8

#define size_of(v, t) (sizeof(v) / sizeof(t))

const int knightX[] = {2, 1, -1, -2, -2, -1, 1, 2};
const int knightY[] = {1, 2, 2, 1, -1, -2, -2, -1};

const int kingX[] = {-1, -1, -1, 0, 0, 1, 1, 1};
const int kingY[] = {-1, 0, 1, -1, 1, -1, 0, 1};

const int bishopX[] = {1, 1, -1, -1};
const int bishopY[] = {1, -1, 1, -1};

const int rookX[] = {1, 0, -1, 0};
const int rookY[] = {0, 1, 0, -1};

static inline Player *getOpposition(char initial, Board *board) {
    return initial == PLAYER_1 ? board->p2 : board->p1;
}

static inline Player *getPlayer(char initial, Board *board) {
    return initial == PLAYER_1 ? board->p1 : board->p2;
}

static bool checkPieceOnSameTeam(const int x, const int y,
                                 const Player *player) {
    for (int j = 0; j < player->count; j++) {
        if (x == player->pieces[j].rect->x / 100 &&
            y == player->pieces[j].rect->y / 100)
            return true;
    }

    return false;
}

static Moves *canMoveBishop(MoveConditions *mc) {

    Moves *moves = malloc(1 * sizeof(*moves));
    moves->squares = malloc(BISHOP_MAX * sizeof(*moves->squares));
    moves->count = 0;

    for (int i = 0; i < 4; i++) {
        int ox = bishopX[i];
        int oy = bishopY[i];

        for (int j = 1; j < ROW_COUNT; j++) {
            int nx = mc->p2->x + j * ox;
            int ny = mc->p2->y + j * oy;

            if (checkPieceOnSameTeam(nx, ny, mc->player))
                break;

            if (nx < ROW_COUNT && nx >= 0 && ny < ROW_COUNT && ny >= 0)
                moves->squares[moves->count++] = (Square){nx, ny};

            if (checkPieceOnSameTeam(nx, ny, mc->opposition))
                break;
        }
    }

    return moves;
}

static Moves *canMoveRook(MoveConditions *mc) {

    Moves *moves = malloc(1 * sizeof(*moves));
    moves->squares = malloc(ROOK_MAX * sizeof(*moves->squares));
    moves->count = 0;

    for (int i = 0; i < 4; i++) {
        int ox = rookX[i];
        int oy = rookY[i];

        for (int j = 1; j < ROW_COUNT; j++) {
            int nx = mc->p2->x + j * ox;
            int ny = mc->p2->y + j * oy;

            if (checkPieceOnSameTeam(nx, ny, mc->player))
                break;

            if (nx < ROW_COUNT && nx >= 0 && ny < ROW_COUNT && ny >= 0)
                moves->squares[moves->count++] = (Square){nx, ny};

            if (checkPieceOnSameTeam(nx, ny, mc->opposition))
                break;
        }
    }

    return moves;
}

static Moves *canMoveKnight(MoveConditions *mc) {

    Moves *moves = malloc(1 * sizeof(*moves));
    moves->squares = malloc(KNIGHT_MAX * sizeof(*moves->squares));
    moves->count = 0;

    for (int i = 0; i < ROW_COUNT; i++) {
        int nx = mc->p2->x + knightX[i];
        int ny = mc->p2->y + knightY[i];

        if (nx < ROW_COUNT && nx >= 0 && ny < ROW_COUNT && ny >= 0 &&
            !checkPieceOnSameTeam(nx, ny, mc->player)) {
            moves->squares[moves->count++] = (Square){nx, ny};
        }
    }

    return moves;
}

static Moves *canMovePawn(MoveConditions *mc) {

    Moves *moves = malloc(1 * sizeof(*moves));
    moves->squares = malloc(PAWN_MAX * sizeof(*moves->squares));
    moves->count = 0;

    int row = 1;
    int x, y;

    if (mc->piece->player == PLAYER_1) {
        // 1
        if (mc->piece->firstMove)
            row = 2;

        for (int i = 1; i <= row; i++)
            if (!checkPieceOnSameTeam(mc->p2->x, mc->p2->y + i, mc->opposition))
                moves->squares[moves->count++] =
                    (Square){mc->p2->x, mc->p2->y + i};

        x = mc->p2->x - 1;
        y = mc->p2->y + 1;
        if (x < ROW_COUNT && x >= 0 && y < ROW_COUNT && y >= 0) {
            if (checkPieceOnSameTeam(x, y, mc->opposition)) {
                moves->squares[moves->count++] = (Square){x, y};
            }
        }

        x = mc->p2->x + 1;
        y = mc->p2->y + 1;
        if (x < ROW_COUNT && x >= 0 && y < ROW_COUNT && y >= 0) {
            if (checkPieceOnSameTeam(x, y, mc->opposition)) {
                moves->squares[moves->count++] = (Square){x, y};
            }
        }
    } else if (mc->piece->player == PLAYER_2) {
        // -1
        if (mc->piece->firstMove)
            row = 2;

        for (int i = 1; i <= row; i++)
            if (!checkPieceOnSameTeam(mc->p2->x, mc->p2->y - i, mc->opposition))
                moves->squares[moves->count++] =
                    (Square){mc->p2->x, mc->p2->y - i};

        x = mc->p2->x - 1;
        y = mc->p2->y - 1;
        if (x < ROW_COUNT && x >= 0 && y < ROW_COUNT && y >= 0) {
            if (checkPieceOnSameTeam(x, y, mc->opposition)) {
                moves->squares[moves->count++] = (Square){x, y};
            }
        }

        x = mc->p2->x + 1;
        y = mc->p2->y - 1;
        if (x < ROW_COUNT && x >= 0 && y < ROW_COUNT && y >= 0) {
            if (checkPieceOnSameTeam(x, y, mc->opposition)) {
                moves->squares[moves->count++] = (Square){x, y};
            }
        }
    }

    return moves;
}

static Moves *canMoveQueen(MoveConditions *mc) {

    Moves *diagMoves = canMoveBishop(mc);
    Moves *horiMoves = canMoveRook(mc);

    const int count = diagMoves->count + horiMoves->count;

    Moves *moves = malloc(1 * sizeof(*moves));
    moves->squares = malloc(count * sizeof(*moves->squares));

    memcpy(moves->squares, diagMoves->squares,
           diagMoves->count * sizeof(*diagMoves->squares));
    memcpy(moves->squares + diagMoves->count, horiMoves->squares,
           horiMoves->count * sizeof(*horiMoves->squares));

    moves->count = count;

    return moves;
}

static Moves *canMoveKing(MoveConditions *mc) {

    Moves *moves = malloc(1 * sizeof(*moves));
    moves->squares = malloc(KING_MAX * sizeof(*moves->squares));
    moves->count = 0;

    for (int i = 0; i < ROW_COUNT; i++) {
        int nx = mc->p2->x + kingX[i];
        int ny = mc->p2->y + kingY[i];

        if (nx < ROW_COUNT && nx >= 0 && ny < ROW_COUNT && ny >= 0 &&
            !checkPieceOnSameTeam(nx, ny, mc->player)) {
            moves->squares[moves->count++] = (Square){nx, ny};
        }
    }

    return moves;
}

void generateMoves(MouseEvent *mEvent, Board *board) {
    // SDL_Point p1 = {mEvent->oldPos->x / 100, mEvent->oldPos->y / 100};
    SDL_Point p2 = {mEvent->mousePos->x / 100, mEvent->mousePos->y / 100};

    Player *player = getPlayer(mEvent->piece->player, board);
    Player *opposition = getOpposition(mEvent->piece->player, board);

    MoveConditions conditions = {// .p1 = &p1,
                                 .p2 = &p2,
                                 .player = player,
                                 //  .mousePos = mEvent->mousePos,
                                 //  .board = board,
                                 .opposition = opposition,
                                 .piece = mEvent->piece};

    Moves *moves = mEvent->piece->canMove(&conditions);

    mEvent->piece->moves = moves;
}

void alignPiece(MouseEvent *mEvent, Board *board) {
    SDL_Point point = {mEvent->mousePos->x, mEvent->mousePos->y};

    int x = (point.x / 100) * 100;
    int y = (point.y / 100) * 100;

    mEvent->piece->rect->x = x;
    mEvent->piece->rect->y = y;

    Player *opposition = getOpposition(mEvent->piece->player, board);

    // Check if moved piece is on another piece
    for (int i = 0; i < opposition->count; i++) {
        // Capture the oppositions piece
        if (opposition->pieces[i].rect->x == x &&
            opposition->pieces[i].rect->y == y &&
            SDL_PointInRect(&point, opposition->pieces[i].rect)) {

            free(opposition->pieces[i].rect);

            for (int j = i; j < opposition->count - 1; j++)
                opposition->pieces[j] = opposition->pieces[j + 1];

            opposition->count--;

            // Update boards virtual pieces
            board
                ->pieces[mEvent->mousePos->y / 100][mEvent->mousePos->x / 100] =
                (Piece){0};
            board->pieces[mEvent->mousePos->y / 100][mEvent->mousePos->x / 100]
                .initial = EMPTY;

            break;
        }
    }
}

bool canMovePiece(MouseEvent *event) {
    const int mPosX = event->mousePos->x / 100;
    const int mPosY = event->mousePos->y / 100;

    for (int i = 0; i < event->piece->moves->count; i++) {
        const int possX = event->piece->moves->squares[i].x;
        const int possY = event->piece->moves->squares[i].y;

        if (mPosX == possX && mPosY == possY)
            return true;
    }

    return false;
}

void checkIfPiece(MouseEvent *mEvent, Player *player) {
    for (int i = 0; i < player->count; i++) {
        if (SDL_PointInRect(mEvent->mousePos, player->pieces[i].rect)) {

            mEvent->piece = &player->pieces[i];
            mEvent->offset->x = mEvent->mousePos->x - player->pieces[i].rect->x;
            mEvent->offset->y = mEvent->mousePos->y - player->pieces[i].rect->y;
            mEvent->pieceSelected = true;

            break;
        }
    }
}

// void printPieces(Board *board) {
//     printf("\n");
//     for (int i = 0; i < ROW_COUNT; i++) {
//         for (int j = 0; j < ROW_COUNT; j++) {
//             printf("%c ", board->pieces[i][j].initial);
//         }
//         printf("\n");
//     }
//     printf("\n");
// }

static Piece makePiece(const char initial, SDL_Texture *texture,
                       const SDL_Point *pos, const char player,
                       void *canMoveFunc) {
    int w;
    int h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);

    SDL_Rect *rect = NULL;
    rect = malloc(1 * sizeof *rect);

    // rect->h = HEIGHT / ROW_COUNT;
    // rect->w = WIDTH / ROW_COUNT;

    rect->h = h;
    rect->w = w;

    rect->x = pos->x;
    rect->y = pos->y;

    return (Piece){.initial = initial,
                   .player = player,
                   .texture = texture,
                   .firstMove = true,
                   .canMove = canMoveFunc,
                   .rect = rect};
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

    board->p1->pieces = malloc(PIECE_COUNT * sizeof(*board->p1->pieces));
    board->p2->pieces = malloc(PIECE_COUNT * sizeof(*board->p2->pieces));

    SDL_Point p1BackRow;
    SDL_Point p2BackRow;

    Piece empty = {0};
    empty.initial = EMPTY;

    // Init the board representation
    board->pieces = malloc(ROW_COUNT * sizeof(*board->pieces));
    for (int i = 0; i < ROW_COUNT; i++)
        board->pieces[i] = malloc(ROW_COUNT * sizeof(*board->pieces[i]));

    // Init the pieces
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

        board->pieces[0][i] = board->p1->pieces[i];
        board->pieces[7][i] = board->p2->pieces[i + ROW_COUNT];

        // Make the pawns
        board->p1->pieces[i + ROW_COUNT] =
            makePiece(PAWN, pawnTexture->black,
                      &(SDL_Point){.x = p1BackRow.x, .y = HEIGHT - 700},
                      PLAYER_1, canMovePawn);
        board->p2->pieces[i] =
            makePiece(PAWN, pawnTexture->white,
                      &(SDL_Point){.x = p2BackRow.x, .y = HEIGHT - 200},
                      PLAYER_2, canMovePawn);

        board->pieces[1][i] = board->p1->pieces[i + ROW_COUNT];
        board->pieces[6][i] = board->p2->pieces[i];

        board->pieces[2][i] = empty;
        board->pieces[3][i] = empty;
        board->pieces[4][i] = empty;
        board->pieces[5][i] = empty;
    }

    // printPieces(board);

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

static void cleanUpPlayer(Player *player) {
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
