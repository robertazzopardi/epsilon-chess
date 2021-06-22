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

#define PIECE_COUNT 16

#define ROOK_MAX 14
#define BISHOP_MAX 13
#define KNIGHT_MAX 8
#define PAWN_MAX 3
#define QUEEN_MAX 27
#define KING_MAX 10

static int knightX[] = {2, 1, -1, -2, -2, -1, 1, 2};
static int knightY[] = {1, 2, 2, 1, -1, -2, -2, -1};

static int kingX[] = {-1, -1, -1, 0, 0, 1, 1, 1};
static int kingY[] = {-1, 0, 1, -1, 1, -1, 0, 1};

static int bishopX[] = {1, 1, -1, -1};
static int bishopY[] = {1, -1, 1, -1};

static int rookX[] = {1, 0, -1, 0};
static int rookY[] = {0, 1, 0, -1};

static int queenX[] = {1, 1, -1, -1, 1, 0, -1, 0};
static int queenY[] = {1, -1, 1, -1, 0, 1, 0, -1};

static int pawnX[] = {-1, 1};
static int pawn1Y[] = {1, 1};
static int pawn2Y[] = {-1, -1};

// static inline Player *getOpposition(char initial, Board *board) {
//     return initial == PLAYER_1 ? board->p2 : board->p1;
// }

static inline Player *getPlayer(char initial, Board *board) {
    return initial == PLAYER_1 ? board->p1 : board->p2;
}

static bool onSameTeam(const int x, const int y, const Player *player) {
    for (int j = 0; j < player->count; j++) {
        if (x == player->pieces[j].rect->x / 100 &&
            y == player->pieces[j].rect->y / 100)
            return true;
    }
    return false;
}

static Initial checkWhichPiece(const int x, const int y, const Player *player) {
    for (int j = 0; j < player->count; j++) {
        if (x == player->pieces[j].rect->x / 100 &&
            y == player->pieces[j].rect->y / 100)
            return player->pieces[j].initial;
    }
    return empty;
}

static inline void addSquare(MoveConditions *mc, Moves *moves, int x, int y) {
    if (x < ROW_COUNT && x >= 0 && y < ROW_COUNT && y >= 0 &&
        onSameTeam(x, y, mc->opposition)) {
        moves->squares[moves->count++] = (SDL_Point){x, y};
    }
}

static Moves *makeMoves(int possibleMoves) {
    Moves *moves = malloc(1 * sizeof(*moves));
    moves->squares = malloc(possibleMoves * sizeof(*moves->squares));
    moves->count = 0;
    return moves;
}

static void addRookBishopSquares(MoveConditions *mc, int ox, int oy,
                                 Moves *moves) {
    for (int j = 1; j < ROW_COUNT; j++) {
        int nx = mc->p2->x + j * ox;
        int ny = mc->p2->y + j * oy;

        if (onSameTeam(nx, ny, mc->player))
            break;

        if (nx < ROW_COUNT && nx >= 0 && ny < ROW_COUNT && ny >= 0)
            moves->squares[moves->count++] = (SDL_Point){nx, ny};

        if (onSameTeam(nx, ny, mc->opposition))
            break;
    }
}

static Moves *canMoveQueenRookBishop(MoveConditions *mc) {
    Moves *moves = makeMoves(mc->piece->maxMoves);

    for (int i = 0; i < mc->piece->directions; i++) {
        int ox = mc->piece->directionsX[i];
        int oy = mc->piece->directionsY[i];

        addRookBishopSquares(mc, ox, oy, moves);
    }

    return moves;
}

static bool canCastle(MoveConditions *mc, Moves *moves) {
    int y = mc->p2->y;
    int x;
    for (int i = 1; i < ROW_COUNT - mc->p2->x; i++) {
        x = mc->p2->x + i;
        Initial initial = checkWhichPiece(x, y, mc->player);
        if (initial != rook && initial != empty) {
            break;
        } else if (initial == rook) {
            moves->squares[moves->count++] = (SDL_Point){x - 1, y};
        }
    }

    for (int i = 1; i <= mc->p2->x; i++) {
        x = mc->p2->x - i;
        Initial initial = checkWhichPiece(x, y, mc->player);
        if (initial != rook && initial != empty) {
            break;
        } else if (initial == rook) {
            moves->squares[moves->count++] = (SDL_Point){x + 2, y};
        }
    }

    return false;
}

static Moves *canMoveKingKnight(MoveConditions *mc) {
    Moves *moves = NULL;

    moves = makeMoves(mc->piece->maxMoves);

    for (int i = 0; i < mc->piece->directions; i++) {
        int nx = mc->p2->x + mc->piece->directionsX[i];
        int ny = mc->p2->y + mc->piece->directionsY[i];

        if (nx < ROW_COUNT && nx >= 0 && ny < ROW_COUNT && ny >= 0 &&
            !onSameTeam(nx, ny, mc->player)) {
            moves->squares[moves->count++] = (SDL_Point){nx, ny};
        }
    }

    if (mc->piece->initial == king && mc->piece->firstMove) {
        canCastle(mc, moves);
    }

    return moves;
}

static Moves *canMovePawn(MoveConditions *mc) {

    Moves *moves = makeMoves(mc->piece->maxMoves);

    int count = mc->piece->firstMove ? 2 : 1;
    int offset = mc->piece->player == PLAYER_1 ? 1 : -1;

    for (int i = 1; i <= count; i++) {

        if (onSameTeam(mc->p2->x, mc->p2->y + i * offset, mc->opposition) ||
            onSameTeam(mc->p2->x, mc->p2->y + i * offset, mc->player)) {
            break;
        } else {
            moves->squares[moves->count++] =
                (SDL_Point){mc->p2->x, mc->p2->y + i * offset};
        }
    }

    for (int i = 0; i < mc->piece->directions; i++) {
        addSquare(mc, moves, mc->p2->x + mc->piece->directionsX[i],
                  mc->p2->y + mc->piece->directionsY[i]);
    }

    return moves;
}

void generateMoves(MouseEvent *event, Board *board) {
    SDL_Point p2 = {event->mousePos->x / 100, event->mousePos->y / 100};

    Player *player = getPlayer(event->piece->player, board);

    MoveConditions conditions = {.p2 = &p2,
                                 .player = player,
                                 .opposition = player->opposition,
                                 .piece = event->piece};

    event->piece->moves = event->piece->canMove(&conditions);
}

void alignPiece(MouseEvent *event, Board *board) {
    SDL_Point point = {event->mousePos->x, event->mousePos->y};

    int x = (point.x / 100) * 100;
    int y = (point.y / 100) * 100;

    event->piece->rect->x = x;
    event->piece->rect->y = y;

    // Player *opposition = getOpposition(event->piece->player, board);
    Player *player = getPlayer(event->piece->player, board);

    // Castling
    bool direction = ROW_COUNT - (x / 100) < (x / 100) ? true : false;
    if (event->piece->initial == king && event->piece->firstMove) {
        int m = -2, v = 100;
        if (direction) {
            m = 1;
            v = -100;
        }

        for (int i = 0; i < player->count; i++) {
            if ((x / 100) + m == player->pieces[i].rect->x / 100 &&
                (y / 100) == player->pieces[i].rect->y / 100) {
                player->pieces[i].rect->x = x + v;
            }
        }
    }

    // Check if moved piece is on another piece
    for (int i = 0; i < player->opposition->count; i++) {
        // Capture the oppositions piece
        if (player->opposition->pieces[i].rect->x == x &&
            player->opposition->pieces[i].rect->y == y &&
            SDL_PointInRect(&point, player->opposition->pieces[i].rect)) {

            free(player->opposition->pieces[i].rect);

            for (int j = i; j < player->opposition->count - 1; j++)
                player->opposition->pieces[j] =
                    player->opposition->pieces[j + 1];

            player->opposition->count--;

            break;
        }
    }
}

bool canMovePiece(const MouseEvent *event) {
    const int mPosX = event->mousePos->x / 100;
    const int mPosY = event->mousePos->y / 100;

    for (int i = 0; i < event->piece->moves->count; i++) {
        if (mPosX == event->piece->moves->squares[i].x &&
            mPosY == event->piece->moves->squares[i].y)
            return true;
    }

    return false;
}

void checkIfPiece(MouseEvent *event, Player *player) {
    for (int i = 0; i < player->count; i++) {
        if (SDL_PointInRect(event->mousePos, player->pieces[i].rect)) {

            event->piece = &player->pieces[i];
            event->offset->x = event->mousePos->x - player->pieces[i].rect->x;
            event->offset->y = event->mousePos->y - player->pieces[i].rect->y;
            // event->pieceSelected = true;

            // printf("%d %d\n", event->offset->x, event->offset->y);

            break;
        }
    }
}

// void printPieces(Board *board) {
//     printf("\n");
//     for (int i = 0; i < ROW_COUNT; i++) {
//         for (int j = 0; j < ROW_COUNT; j++)
//             printf("%c ", board->pieces[i][j].initial);
//         printf("\n");
//     }
//     printf("\n");
// }

static Piece makePiece(char initial, SDL_Texture *texture, SDL_Point *pos,
                       char player, int maxMoves, void *canMoveFunc,
                       int *directionsX, int *directionsY, int directions) {
    // int w;
    // int h;
    // SDL_QueryTexture(texture, NULL, NULL, &w, &h);

    SDL_Rect *rect = malloc(1 * sizeof *rect);

    rect->h = HEIGHT / ROW_COUNT;
    rect->w = WIDTH / ROW_COUNT;

    // rect->h = h;
    // rect->w = w;

    rect->x = pos->x;
    rect->y = pos->y;

    return (Piece){.initial = initial,
                   .player = player,
                   .texture = texture,
                   .firstMove = true,
                   .canMove = canMoveFunc,
                   .rect = rect,
                   .maxMoves = maxMoves,
                   .directionsX = directionsX,
                   .directionsY = directionsY,
                   .directions = directions};
}

void makePieces(Window *mainWindow) {
    TwoToneTexture *pawnTexture = getTexture(mainWindow->rend, PAWN_IMG);
    TwoToneTexture *rookTexture = getTexture(mainWindow->rend, ROOK_IMG);
    TwoToneTexture *knightTexture = getTexture(mainWindow->rend, KNIGHT_IMG);
    TwoToneTexture *bishopTexture = getTexture(mainWindow->rend, BISHOP_IMG);
    TwoToneTexture *queenTexture = getTexture(mainWindow->rend, QUEEN_IMG);
    TwoToneTexture *kingTexture = getTexture(mainWindow->rend, KING_IMG);

    mainWindow->board->p1->count = PIECE_COUNT;
    mainWindow->board->p2->count = PIECE_COUNT;

    mainWindow->board->p1->pieces =
        malloc(PIECE_COUNT * sizeof(*mainWindow->board->p1->pieces));
    mainWindow->board->p2->pieces =
        malloc(PIECE_COUNT * sizeof(*mainWindow->board->p2->pieces));

    SDL_Point p1BackRow, p2BackRow;

    Piece player1, player2;

    // Init the pieces
    for (int i = 0; i < ROW_COUNT; i++) {
        p1BackRow.x = i * 100;
        p1BackRow.y = HEIGHT - 800;

        p2BackRow.x = p1BackRow.x;
        p2BackRow.y = HEIGHT - 100;

        switch (i) {
        case 0:
        case 7:
            player1 =
                makePiece(rook, rookTexture->black, &p1BackRow, PLAYER_1,
                          ROOK_MAX, canMoveQueenRookBishop, rookX, rookY, 4);
            player2 =
                makePiece(rook, rookTexture->white, &p2BackRow, PLAYER_2,
                          ROOK_MAX, canMoveQueenRookBishop, rookX, rookY, 4);
            break;
        case 1:
        case 6:
            player1 = makePiece(knight, knightTexture->black, &p1BackRow,
                                PLAYER_1, KNIGHT_MAX, canMoveKingKnight,
                                knightX, knightY, ROW_COUNT);
            player2 = makePiece(knight, knightTexture->white, &p2BackRow,
                                PLAYER_2, KNIGHT_MAX, canMoveKingKnight,
                                knightX, knightY, ROW_COUNT);
            break;
        case 2:
        case 5:
            player1 = makePiece(bishop, bishopTexture->black, &p1BackRow,
                                PLAYER_1, BISHOP_MAX, canMoveQueenRookBishop,
                                bishopX, bishopY, 4);
            player2 = makePiece(bishop, bishopTexture->white, &p2BackRow,
                                PLAYER_2, BISHOP_MAX, canMoveQueenRookBishop,
                                bishopX, bishopY, 4);
            break;
        case 3:
            player1 = makePiece(queen, queenTexture->black, &p1BackRow,
                                PLAYER_1, QUEEN_MAX, canMoveQueenRookBishop,
                                queenX, queenY, ROW_COUNT);
            player2 = makePiece(queen, queenTexture->white, &p2BackRow,
                                PLAYER_2, QUEEN_MAX, canMoveQueenRookBishop,
                                queenX, queenY, ROW_COUNT);
            break;
        case 4:
            player1 = makePiece(king, kingTexture->black, &p1BackRow, PLAYER_1,
                                KNIGHT_MAX, canMoveKingKnight, kingX, kingY,
                                ROW_COUNT);
            player2 = makePiece(king, kingTexture->white, &p2BackRow, PLAYER_2,
                                KNIGHT_MAX, canMoveKingKnight, kingX, kingY,
                                ROW_COUNT);
            break;
        default:
            break;
        }

        mainWindow->board->p1->pieces[i] = player1;
        mainWindow->board->p2->pieces[i + ROW_COUNT] = player2;

        // Make the pawns
        mainWindow->board->p1->pieces[i + ROW_COUNT] = makePiece(
            pawn, pawnTexture->black, &(SDL_Point){p1BackRow.x, HEIGHT - 700},
            PLAYER_1, PAWN_MAX, canMovePawn, pawnX, pawn1Y, 2);
        mainWindow->board->p2->pieces[i] = makePiece(
            pawn, pawnTexture->white, &(SDL_Point){p2BackRow.x, HEIGHT - 200},
            PLAYER_2, PAWN_MAX, canMovePawn, pawnX, pawn2Y, 2);
    }

    // printPieces(mainWindow->board);

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

void drawPieces(Window *mainWindow, MouseEvent *event) {
    for (int i = 0; i < mainWindow->board->p1->count; i++) {
        if (event->piece &&
            SDL_RectEquals(event->piece->rect,
                           mainWindow->board->p1->pieces[i].rect)) {
            continue;
        }

        SDL_RenderCopy(mainWindow->rend,
                       mainWindow->board->p1->pieces[i].texture, NULL,
                       mainWindow->board->p1->pieces[i].rect);
    }
    for (int i = 0; i < mainWindow->board->p2->count; i++) {
        if (event->piece &&
            SDL_RectEquals(event->piece->rect,
                           mainWindow->board->p2->pieces[i].rect)) {
            continue;
        }

        SDL_RenderCopy(mainWindow->rend,
                       mainWindow->board->p2->pieces[i].texture, NULL,
                       mainWindow->board->p2->pieces[i].rect);
    }
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
