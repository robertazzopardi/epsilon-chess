#include <SDL_rect.h>
#include <SDL_render.h>
#include <stdlib.h>

#include "board.h"
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
    Player *opposition;
    Piece *piece;
};

static char knightX[] = {2, 1, -1, -2, -2, -1, 1, 2};
static char knightY[] = {1, 2, 2, 1, -1, -2, -2, -1};

static char kingX[] = {-1, -1, -1, 0, 0, 1, 1, 1};
static char kingY[] = {-1, 0, 1, -1, 1, -1, 0, 1};

static char bishopX[] = {1, 1, -1, -1};
static char bishopY[] = {1, -1, 1, -1};

static char rookX[] = {1, 0, -1, 0};
static char rookY[] = {0, 1, 0, -1};

static char queenX[] = {1, 1, -1, -1, 1, 0, -1, 0};
static char queenY[] = {1, -1, 1, -1, 0, 1, 0, -1};

static char pawnX[] = {-1, 1};
static char pawn1Y[] = {1, 1};
static char pawn2Y[] = {-1, -1};

static inline Player *getPlayer(char initial, Board *board) {
    return initial == PLAYER_1 ? board->p1 : board->p2;
}

static inline bool checkPointNexToRect(int x, int y, int j,
                                       const Player *player) {
    return x == player->pieces[j].rect->x / SQUARE_SIZE &&
           y == player->pieces[j].rect->y / SQUARE_SIZE;
}

static inline bool pointEqualsPoint(SDL_Point p1, SDL_Point p2) {
    return p1.x == p2.x && p1.y == p2.y;
}

static bool onSameTeam(const int x, const int y, const Player *player) {
    for (int j = 0; j < player->piecesRemaining; j++) {
        if (checkPointNexToRect(x, y, j, player))
            return true;
    }
    return false;
}

static Initial checkWhichPiece(const int x, const int y, const Player *player) {
    for (int j = 0; j < player->piecesRemaining; j++) {
        if (checkPointNexToRect(x, y, j, player))
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

static inline void resetMoves(Moves *moves) {
    memset(moves->squares, 0, moves->count * sizeof(*moves->squares));
    moves->count = 0;
}

static Moves *canMoveQueenRookBishop(MoveConditions *mc) {

    for (int i = 0; i < mc->piece->directions; i++) {
        int ox = mc->piece->directionsX[i];
        int oy = mc->piece->directionsY[i];

        for (int j = 1; j < ROW_COUNT; j++) {
            int nx = mc->pieceLocation->x + j * ox;
            int ny = mc->pieceLocation->y + j * oy;

            if (onSameTeam(nx, ny, mc->player))
                break;

            if (nx < ROW_COUNT && nx >= 0 && ny < ROW_COUNT && ny >= 0)
                mc->piece->moves->squares[mc->piece->moves->count++] =
                    (SDL_Point){nx, ny};

            if (onSameTeam(nx, ny, mc->opposition))
                break;
        }
    }

    return mc->piece->moves;
}

static bool canCastle(MoveConditions *mc, Moves *moves) {
    int y = mc->pieceLocation->y;
    for (int i = 1; i < ROW_COUNT - mc->pieceLocation->x; i++) {
        int x = mc->pieceLocation->x + i;
        Initial initial = checkWhichPiece(x, y, mc->player);
        if (initial != rook && initial != empty) {
            break;
        } else if (initial == rook) {
            moves->squares[moves->count++] = (SDL_Point){x - 1, y};
        }
    }

    for (int i = 1; i <= mc->pieceLocation->x; i++) {
        int x = mc->pieceLocation->x - i;
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

    for (int i = 0; i < mc->piece->directions; i++) {
        int nx = mc->pieceLocation->x + mc->piece->directionsX[i];
        int ny = mc->pieceLocation->y + mc->piece->directionsY[i];

        if (nx < ROW_COUNT && nx >= 0 && ny < ROW_COUNT && ny >= 0 &&
            !onSameTeam(nx, ny, mc->player)) {

            mc->piece->moves->squares[mc->piece->moves->count++] =
                (SDL_Point){nx, ny};
        }
    }

    // printf("hello\n");

    if (mc->piece->initial == king && mc->piece->firstMove) {
        canCastle(mc, mc->piece->moves);
    }

    return mc->piece->moves;
}

static Moves *canMovePawn(MoveConditions *mc) {

    int count = mc->piece->firstMove ? 2 : 1;
    int offset = mc->piece->player == PLAYER_1 ? 1 : -1;

    for (int i = 1; i <= count; i++) {

        if (onSameTeam(mc->pieceLocation->x, mc->pieceLocation->y + i * offset,
                       mc->opposition) ||
            onSameTeam(mc->pieceLocation->x, mc->pieceLocation->y + i * offset,
                       mc->player)) {
            break;
        } else {
            mc->piece->moves->squares[mc->piece->moves->count++] = (SDL_Point){
                mc->pieceLocation->x, mc->pieceLocation->y + i * offset};
        }
    }

    for (int i = 0; i < mc->piece->directions; i++) {
        addSquare(mc, mc->piece->moves,
                  mc->pieceLocation->x + mc->piece->directionsX[i],
                  mc->pieceLocation->y + mc->piece->directionsY[i]);
    }

    return mc->piece->moves;
}

void calculateIfMovesAreValid(Piece *piece __unused, Player *player) {
    SDL_Point playerKingLocation;
    for (unsigned char i = 0; i < player->piecesRemaining; i++) {
        if (player->pieces[i].initial == king) {
            playerKingLocation =
                (SDL_Point){player->pieces[i].rect->x / SQUARE_SIZE,
                            player->pieces[i].rect->y / SQUARE_SIZE};
            break;
        }
    }

    Player *opposition = player->opposition;
    for (unsigned char i = 0; i < opposition->piecesRemaining; i++) {
        Piece offensivePiece = opposition->pieces[i];

        SDL_Point offensivePieceLocation = {
            offensivePiece.rect->x / SQUARE_SIZE,
            offensivePiece.rect->y / SQUARE_SIZE};

        unsigned char start = 0, end = 0;
        SDL_Point *routeToCheck = NULL;
        int len = 0;
        for (unsigned char l = 0; l < offensivePiece.moves->count; l++) {
            if (floor(hypot(offensivePiece.moves->squares[l].x -
                                offensivePieceLocation.x,
                            offensivePiece.moves->squares[l].y -
                                offensivePieceLocation.y)) == 1) {
                start = l;
            }

            if (pointEqualsPoint(offensivePiece.moves->squares[l],
                                 playerKingLocation)) {
                end = l;

                len = end - start;
                len++;
                routeToCheck = malloc(len * sizeof(*routeToCheck));

                memcpy(routeToCheck, offensivePiece.moves->squares + start,
                       len * sizeof(*routeToCheck));
                routeToCheck[len - 1] = offensivePieceLocation;

                // for (int x = 0; x < len; x++) {
                //     printf("%d %d\n", routeToCheck[x].x, routeToCheck[x].y);
                // }

                break;
            }
        }

        Moves *opPieceMoves = offensivePiece.moves;
        for (unsigned char j = 0; j < opPieceMoves->count; j++) {
            SDL_Point piecePossibleMovePosition = {opPieceMoves->squares[j].x,
                                                   opPieceMoves->squares[j].y};

            if (pointEqualsPoint(playerKingLocation,
                                 piecePossibleMovePosition)) {

                SDL_Point *onlyMoves =
                    malloc(piece->moves->count * sizeof(*onlyMoves));
                int onlyMovesCount = 0;

                for (unsigned char x = 0; x < piece->moves->count; x++) {
                    for (unsigned char y = 0; y < len; y++) {
                        if (pointEqualsPoint(piece->moves->squares[x],
                                             routeToCheck[y]) &&
                            piece->initial != king) {
                            onlyMoves[onlyMovesCount++] =
                                piece->moves->squares[x];
                        }
                    }
                }

                onlyMoves =
                    realloc(onlyMoves, onlyMovesCount * sizeof(*onlyMoves));

                memcpy(piece->moves->squares, onlyMoves,
                       onlyMovesCount * sizeof(*piece->moves->squares));

                piece->moves->count = onlyMovesCount;
            }
        }

        if (routeToCheck != NULL) {
            free(routeToCheck);
            routeToCheck = NULL;
        }
    }
}

void generatePieceMoves(Piece *piece, Player *player) {
    SDL_Point pieceLocation = {piece->rect->x / SQUARE_SIZE,
                               piece->rect->y / SQUARE_SIZE};

    resetMoves(piece->moves);

    MoveConditions conditions = {.pieceLocation = &pieceLocation,
                                 .player = player,
                                 .opposition = player->opposition,
                                 .piece = piece};

    piece->moves = piece->canMove(&conditions);
}

void alignPiece(MouseEvent *event, Board *board) {
    SDL_Point point = {event->mousePos->x, event->mousePos->y};

    int x = (point.x / SQUARE_SIZE) * SQUARE_SIZE;
    int y = (point.y / SQUARE_SIZE) * SQUARE_SIZE;

    event->piece->rect->x = x;
    event->piece->rect->y = y;

    Player *player = getPlayer(event->piece->player, board);

    // Castling
    bool direction =
        ROW_COUNT - (x / SQUARE_SIZE) < (x / SQUARE_SIZE) ? true : false;
    if (event->piece->initial == king && event->piece->firstMove) {
        int m = -2, v = SQUARE_SIZE;
        if (direction) {
            m = 1;
            v = -SQUARE_SIZE;
        }

        for (int i = 0; i < player->piecesRemaining; i++) {
            if ((x / SQUARE_SIZE) + m ==
                    player->pieces[i].rect->x / SQUARE_SIZE &&
                (y / SQUARE_SIZE) == player->pieces[i].rect->y / SQUARE_SIZE) {
                player->pieces[i].rect->x = x + v;
            }
        }
    }

    // Check if moved piece is on another piece
    for (int i = 0; i < player->opposition->piecesRemaining; i++) {
        // Capture the oppositions piece
        if (player->opposition->pieces[i].rect->x == x &&
            player->opposition->pieces[i].rect->y == y &&
            SDL_PointInRect(&point, player->opposition->pieces[i].rect)) {

            free(player->opposition->pieces[i].rect);

            for (int j = i; j < player->opposition->piecesRemaining - 1; j++)
                player->opposition->pieces[j] =
                    player->opposition->pieces[j + 1];

            player->opposition->piecesRemaining--;

            break;
        }
    }
}

bool canMovePiece(const MouseEvent *event) {
    const int mPosX = event->mousePos->x / SQUARE_SIZE;
    const int mPosY = event->mousePos->y / SQUARE_SIZE;

    for (int i = 0; i < event->piece->moves->count; i++) {
        if (mPosX == event->piece->moves->squares[i].x &&
            mPosY == event->piece->moves->squares[i].y)
            return true;
    }

    return false;
}

void checkIfPiece(MouseEvent *event, Player *player) {
    for (int i = 0; i < player->piecesRemaining; i++) {
        if (SDL_PointInRect(event->mousePos, player->pieces[i].rect)) {

            event->piece = &player->pieces[i];
            event->offset->x = event->mousePos->x - player->pieces[i].rect->x;
            event->offset->y = event->mousePos->y - player->pieces[i].rect->y;

            break;
        }
    }
}

static Piece makePiece(char initial, SDL_Texture *texture, SDL_Point *pos,
                       char player, unsigned char maxPossibleMoves,
                       void *canMoveFunc, char *directionsX, char *directionsY,
                       char directions) {

    SDL_Rect *rect = malloc(1 * sizeof *rect);

    rect->h = HEIGHT / ROW_COUNT;
    rect->w = WIDTH / ROW_COUNT;

    rect->x = pos->x;
    rect->y = pos->y;

    Moves *moves = malloc(1 * sizeof(*moves));
    moves->squares = malloc(maxPossibleMoves * sizeof(*moves->squares));
    moves->count = 0;

    return (Piece){
        .initial = initial,
        .player = player,
        .texture = texture,
        .firstMove = true,
        .canMove = canMoveFunc,
        .rect = rect,
        .maxPossibleMoves = maxPossibleMoves,
        .directionsX = directionsX,
        .directionsY = directionsY,
        .directions = directions,
        .moves = moves,
    };
}

void makePieces(Window *mainWindow) {
    TwoToneTexture *pawnTexture = getTexture(mainWindow->rend, PAWN_IMG);
    TwoToneTexture *rookTexture = getTexture(mainWindow->rend, ROOK_IMG);
    TwoToneTexture *knightTexture = getTexture(mainWindow->rend, KNIGHT_IMG);
    TwoToneTexture *bishopTexture = getTexture(mainWindow->rend, BISHOP_IMG);
    TwoToneTexture *queenTexture = getTexture(mainWindow->rend, QUEEN_IMG);
    TwoToneTexture *kingTexture = getTexture(mainWindow->rend, KING_IMG);

    mainWindow->board->p1->piecesRemaining = PIECE_COUNT;
    mainWindow->board->p2->piecesRemaining = PIECE_COUNT;

    mainWindow->board->p1->pieces =
        malloc(PIECE_COUNT * sizeof(*mainWindow->board->p1->pieces));
    mainWindow->board->p2->pieces =
        malloc(PIECE_COUNT * sizeof(*mainWindow->board->p2->pieces));

    SDL_Point p1BackRow, p2BackRow;

    Piece player1, player2;

    // Init the pieces
    for (int i = 0; i < ROW_COUNT; i++) {
        p1BackRow.x = i * SQUARE_SIZE;
        p1BackRow.y = 0;

        p2BackRow.x = p1BackRow.x;
        p2BackRow.y = HEIGHT - SQUARE_SIZE;

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
    pawnTexture = NULL;
    free(rookTexture);
    rookTexture = NULL;
    free(kingTexture);
    kingTexture = NULL;
    free(knightTexture);
    knightTexture = NULL;
    free(bishopTexture);
    bishopTexture = NULL;
    free(queenTexture);
    queenTexture = NULL;
}

void drawPieces(Window *mainWindow, MouseEvent *event) {
    for (int i = 0; i < mainWindow->board->p1->piecesRemaining; i++) {
        if (event->piece &&
            SDL_RectEquals(event->piece->rect,
                           mainWindow->board->p1->pieces[i].rect)) {
            continue;
        }

        SDL_RenderCopy(mainWindow->rend,
                       mainWindow->board->p1->pieces[i].texture, NULL,
                       mainWindow->board->p1->pieces[i].rect);
    }
    for (int i = 0; i < mainWindow->board->p2->piecesRemaining; i++) {
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
    for (int i = 0; i < player->piecesRemaining; i++) {
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
    board->p1->pieces = NULL;
    free(board->p2->pieces);
    board->p2->pieces = NULL;
}
