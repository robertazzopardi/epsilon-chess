#include "piece.h"

#define PAWN_IMG "./resources/864630-chess/svg/008-pawn.svg"
#define ROOK_IMG "./resources/864630-chess/svg/006-rook.svg"
#define KNIGHT_IMG "./resources/864630-chess/svg/010-horse.svg"
#define BISHOP_IMG "./resources/864630-chess/svg/016-bishop.svg"
#define QUEEN_IMG "./resources/864630-chess/svg/007-queen.svg"
#define KING_IMG "./resources/864630-chess/svg/009-king.svg"

#define OFFSET100 100
#define OFFSET800 800
#define OFFSET700 700
#define OFFSET200 200

#define PAWN 'P'
#define KING 'K'
#define QUEEN 'Q'
#define BISHOP 'B'
#define KNIGHT 'N'
#define ROOK 'R'

#define PLAYER_1 '1'
#define PLAYER_2 '2'

#define PIECE_COUNT 16

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

// #define SAME_TEAM(n, i) (                                            \
//     getFirstDigit(point->x) == getFirstDigit(n.pieces[i].rect->x) && \
//     getFirstDigit(point->y) == getFirstDigit(n.pieces[i].rect->y))

typedef struct {
    long len;
    char *buffer;
} SVGString;

const int X[8] = {2, 1, -1, -2, -2, -1, 1, 2};
const int Y[8] = {1, 2, 2, 1, -1, -2, -2, -1};

static inline Player *getPlayer(const char player) {
    return player == PLAYER_1 ? &board.p1 : &board.p2;
}

static inline Player *getOpposition(const char player) {
    return player == PLAYER_1 ? &board.p2 : &board.p1;
}

inline void remove_element(Piece *array, int index, int array_length) {
    int i;
    for (i = index; i < array_length - 1; i++)
        array[i] = array[i + 1];
}

void capture(const SDL_Point *point, Player *player) {
    for (int i = 0; i < player->count; i++) {
        if (SDL_PointInRect(point, player->pieces[i].rect)) {
            free(player->pieces[i].rect);
            remove_element(player->pieces, i, player->count);
            player->count--;
            break;
        }
    }
}

bool checkPieceOnPiece(const SDL_Point *point, const Player *player) {
    for (int i = 0; i < player->count; i++) {
        if (SDL_PointInRect(point, player->pieces[i].rect))
            return true;
    }
    return false;
}

bool checkPieceOnSameTeam(const int x, const int y, const Player *player) {
    for (int j = 0; j < player->count; j++) {
        if (x == getFirstDigit(player->pieces[j].rect->x) &&
            y == getFirstDigit(player->pieces[j].rect->y)) {
            return true;
        }
    }
    return false;
}

void checkIfFirstMove(const MouseEvent *mEvent) {
    const Player *player = getPlayer(mEvent->piece->player);
    for (int i = 0; i < player->count; i++) {
        if (SDL_RectEquals(player->pieces[i].rect, mEvent->piece->rect)) {
            player->pieces[i].firstMove = false;
            break;
        }
    }
}

static inline int getDistance(const SDL_Point *p1, const SDL_Point *p2) {
    return abs(p2->x - p1->x) + abs(p2->y - p1->y);
}

static inline bool distanceEqual(const SDL_Point *p1, const SDL_Point *p2) {
    return abs(p2->x - p1->x) == abs(p2->y - p1->y);
}

bool canMoveBishop(const SDL_Point *p1, const SDL_Point *p2,
                   const Player *player) {
    for (int i = 1; i <= getDistance(p1, p2) / 2; i++) {
        if (p2->x > p1->x && p2->y > p1->y &&
            checkPieceOnSameTeam(p1->x + i, p1->y + i, player))
            return false;
        else if (p2->x > p1->x && p2->y < p1->y &&
                 checkPieceOnSameTeam(p1->x + i, p1->y - i, player))
            return false;
        else if (p2->x < p1->x && p2->y > p1->y &&
                 checkPieceOnSameTeam(p1->x - i, p1->y + i, player))
            return false;
        else if (p2->x < p1->x && p2->y < p1->y &&
                 checkPieceOnSameTeam(p1->x - i, p1->y + i, player))
            return false;
    }

    return distanceEqual(p1, p2);
}

bool canMoveRook(const SDL_Point *p1, const SDL_Point *p2,
                 const Player *player) {
    if (p1->x != p2->x && p1->y != p2->y)
        return false;

    for (int i = 1; i <= getDistance(p1, p2); i++) {
        if (p2->x > p1->x && checkPieceOnSameTeam(p1->x + i, p1->y, player))
            return false;
        else if (p2->x < p1->x && checkPieceOnSameTeam(p1->x - i, p1->y, player))
            return false;
        else if (p2->y > p1->y && checkPieceOnSameTeam(p1->x, p1->y + i, player))
            return false;
        else if (p2->y < p1->y && checkPieceOnSameTeam(p1->x, p1->y - i, player))
            return false;
    }

    return !canMoveBishop(p1, p2, player);
}

bool canMoveKnight(const SDL_Point *p1, const SDL_Point *p2,
                   const Player *player) {
    for (int i = 0; i < ROW_COUNT; i++)
        if (p2->x + X[i] == p1->x && p2->y + Y[i] == p1->y)
            return !checkPieceOnSameTeam(p2->x, p2->y, player);
    return false;
}

static inline bool checkDifference(const int mouseY, const int oldY,
                                   const char player) {
    return player == PLAYER_1 ? mouseY - oldY == 1 : oldY - mouseY == 1;
}

bool canMovePawn(const SDL_Point *p1, const SDL_Point *p2, const Piece *piece,
                 const SDL_Point *mousePos) {
    const int d = getDistance(p1, p2);
    if (d > (piece->firstMove ? 2 : 1))
        return distanceEqual(p1, p2);
    else if (checkPieceOnPiece(mousePos, getOpposition(piece->player)))
        return false;
    else if (checkDifference(p2->y, p1->y, piece->player) == piece->firstMove
                 ? 2
                 : 1 && p2->x == p1->x)
        return true;

    return false;
}

bool canMove(MouseEvent *mEvent) {
    SDL_Point p1 = {.x = getFirstDigit(mEvent->oldPos.x),
                    .y = getFirstDigit(mEvent->oldPos.y)};

    SDL_Point p2 = {.x = getFirstDigit(mEvent->mousePos.x),
                    .y = getFirstDigit(mEvent->mousePos.y)};

    const Player *player = getPlayer(mEvent->piece->player);

    switch (mEvent->piece->initial) {
        case PAWN:
            return canMovePawn(&p1, &p2, mEvent->piece, &mEvent->mousePos);
        case ROOK:
            return canMoveRook(&p1, &p2, player);
        case KNIGHT:
            return canMoveKnight(&p1, &p2, player);
        case BISHOP:
            return canMoveBishop(&p1, &p2, player);
        case QUEEN:
            return canMoveBishop(&p1, &p2, player) || canMoveRook(&p1, &p2, player);
        case KING:
            return (canMoveBishop(&p1, &p2, player) && getDistance(&p1, &p2) == 2) ||
                   (canMoveRook(&p1, &p2, player) && getDistance(&p1, &p2) == 1);
        default:
            break;
    }

    return false;
}

void alignPiece(MouseEvent *mEvent) {
    SDL_Point point;
    point.x = mEvent->piece->rect->x + (mEvent->piece->rect->w / 2);
    point.y = mEvent->piece->rect->y + (mEvent->piece->rect->h / 2);

    int x = getFirstDigit(point.x);
    int y = getFirstDigit(point.y);

    if (SDL_PointInRect(&point, &board.squares[x][y].rect)) {
        mEvent->piece->rect->x = board.squares[x][y].rect.x;
        mEvent->piece->rect->y = board.squares[x][y].rect.y;

        //
        Player *opposition = getOpposition(mEvent->piece->player);
        capture(&point, opposition);
    }
}

void checkIfPiece(MouseEvent *mEvent, Player *player) {
    for (int i = 0; i < player->count; i++) {
        if (SDL_PointInRect(&mEvent->mousePos, player->pieces[i].rect)) {
            *mEvent->piece = player->pieces[i];
            mEvent->offset.x = mEvent->mousePos.x - player->pieces[i].rect->x;
            mEvent->offset.y = mEvent->mousePos.y - player->pieces[i].rect->y;

            break;
        }
    }
}

void printPieces() {
    for (int i = 0; i < board.p1.count; i++) {
        printf("%c", board.p1.pieces[i].initial);
    }
    printf("\n");
    for (int i = 0; i < board.p2.count; i++) {
        printf("%c", board.p2.pieces[i].initial);
    }

    printf("\n");
}

SVGString *readSVGFile(const char *inputFilename) {
    SVGString *str;
    str = calloc(1, sizeof(*str));

    FILE *fh = fopen(inputFilename, "rb");
    if (fh != NULL) {
        fseek(fh, 0L, SEEK_END);
        str->len = ftell(fh);
        rewind(fh);
        str->buffer = malloc(str->len);

        if (str->buffer != NULL) {
            fread(str->buffer, str->len, 1, fh);
            // we can now close the file
            fclose(fh);
            fh = NULL;

            // do something, e.g.
            // fwrite(str->buffer, str->len, 1, stdout);

            // free(str->buffer);
        }
        if (fh != NULL)
            fclose(fh);
    }

    return str;
}

static inline SDL_Texture *getTexture(const SVGString *svg) {
    SDL_RWops *rw = SDL_RWFromConstMem(svg->buffer, svg->len);
    SDL_Surface *surface = IMG_Load_RW(rw, 1);
    return SDL_CreateTextureFromSurface(mainWindow.rend, surface);
}

Piece makePiece(const char initial, SDL_Texture *texture, const SDL_Point *pos,
                const char player) {
    int w = 0;
    int h = 0;

    Piece piece = {.initial = initial, .player = player, .texture = texture};

    SDL_QueryTexture(piece.texture, NULL, NULL, &w, &h);

    SDL_Rect *rect = NULL;
    rect = calloc(1, sizeof(*rect));
    rect->h = (HEIGHT / ROW_COUNT);
    rect->w = (WIDTH / ROW_COUNT);
    rect->x = pos->x;
    rect->y = pos->y;

    piece.rect = rect;
    piece.firstMove = true;

    return piece;
}

void makePieces() {
    SDL_Texture *pawnTexture = getTexture(readSVGFile(PAWN_IMG));
    SDL_Texture *rookTexture = getTexture(readSVGFile(ROOK_IMG));
    SDL_Texture *knightTexture = getTexture(readSVGFile(KNIGHT_IMG));
    SDL_Texture *bishopTexture = getTexture(readSVGFile(BISHOP_IMG));
    SDL_Texture *queenTexture = getTexture(readSVGFile(QUEEN_IMG));
    SDL_Texture *kingTexture = getTexture(readSVGFile(KING_IMG));

    board.p1.count = PIECE_COUNT;
    board.p2.count = PIECE_COUNT;

    board.p1.pieces = calloc(PIECE_COUNT, sizeof(*board.p1.pieces));
    board.p2.pieces = calloc(PIECE_COUNT, sizeof(*board.p2.pieces));

    SDL_Point p1BackRow;
    SDL_Point p2BackRow;

    for (int i = 0; i < ROW_COUNT; i++) {
        p1BackRow.x = i * OFFSET100;
        p1BackRow.y = HEIGHT - OFFSET800;

        p2BackRow.x = i * OFFSET100;
        p2BackRow.y = HEIGHT - OFFSET100;

        switch (i) {
            case 0:
            case 7:
                board.p1.pieces[i] = makePiece(ROOK, rookTexture, &p1BackRow, PLAYER_1);
                board.p2.pieces[i + ROW_COUNT] =
                    makePiece(ROOK, rookTexture, &p2BackRow, PLAYER_2);
                break;
            case 1:
            case 6:
                board.p1.pieces[i] =
                    makePiece(KNIGHT, knightTexture, &p1BackRow, PLAYER_1);
                board.p2.pieces[i + ROW_COUNT] =
                    makePiece(KNIGHT, knightTexture, &p2BackRow, PLAYER_2);
                break;
            case 2:
            case 5:
                board.p1.pieces[i] =
                    makePiece(BISHOP, bishopTexture, &p1BackRow, PLAYER_1);
                board.p2.pieces[i + ROW_COUNT] =
                    makePiece(BISHOP, bishopTexture, &p2BackRow, PLAYER_2);
                break;
            case 3:
                board.p1.pieces[i] = makePiece(QUEEN, queenTexture, &p1BackRow, PLAYER_1);
                board.p2.pieces[i + ROW_COUNT] =
                    makePiece(QUEEN, queenTexture, &p2BackRow, PLAYER_2);
                break;
            case 4:
                board.p1.pieces[i] = makePiece(KING, kingTexture, &p1BackRow, PLAYER_1);
                board.p2.pieces[i + ROW_COUNT] =
                    makePiece(KING, kingTexture, &p2BackRow, PLAYER_2);
                break;
            default:
                break;
        }

        board.p1.pieces[i + ROW_COUNT] = makePiece(
            PAWN, pawnTexture,
            &(SDL_Point){.x = p1BackRow.x, .y = HEIGHT - OFFSET700}, PLAYER_1);
        board.p2.pieces[i] = makePiece(
            PAWN, pawnTexture,
            &(SDL_Point){.x = p2BackRow.x, .y = HEIGHT - OFFSET200}, PLAYER_2);
    }
}

void drawPieces() {
    for (int i = 0; i < board.p1.count; i++)
        SDL_RenderCopy(mainWindow.rend, board.p1.pieces[i].texture, NULL,
                       board.p1.pieces[i].rect);
    for (int i = 0; i < board.p2.count; i++)
        SDL_RenderCopy(mainWindow.rend, board.p2.pieces[i].texture, NULL,
                       board.p2.pieces[i].rect);
}

void cleanUpPlayer(Player *player) {
    for (int i = 0; i < player->count; i++) {
        SDL_DestroyTexture(player->pieces[i].texture);
        player->pieces[i].texture = NULL;
        free(player->pieces[i].rect);
        player->pieces[i].rect = NULL;
    }
}

void cleanUpPieces() {
    cleanUpPlayer(&board.p1);
    cleanUpPlayer(&board.p2);

    free(board.p1.pieces);
    free(board.p2.pieces);
    board.p1.pieces = NULL;
    board.p2.pieces = NULL;
}
