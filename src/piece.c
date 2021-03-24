#include "piece.h"

#define OFFSET100 100
#define OFFSET800 800
#define OFFSET700 700
#define OFFSET200 200

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

// #define SAME_TEAM(n, i) (                                            \
//     getFirstDigit(point->x) == getFirstDigit(n.pieces[i].rect->x) && \
//     getFirstDigit(point->y) == getFirstDigit(n.pieces[i].rect->y))

typedef struct
{
    long len;
    char *buffer;
} SVGString;

const int X[8] = {2, 1, -1, -2, -2, -1, 1, 2};
const int Y[8] = {1, 2, 2, 1, -1, -2, -2, -1};

static inline Player *getPlayer(const char player)
{
    return player == PLAYER_1 ? &board.p1 : &board.p2;
}

static inline Player *getOpposition(const char player)
{
    return player == PLAYER_1 ? &board.p2 : &board.p1;
}

inline void remove_element(Piece *array, int index, int array_length)
{
    int i;
    for (i = index; i < array_length - 1; i++)
        array[i] = array[i + 1];
}

void capture(const SDL_Point *point, Player *player)
{
    for (int i = 0; i < player->count; i++)
    {
        if (SDL_PointInRect(point, player->pieces[i].rect))
        {
            free(player->pieces[i].rect);
            remove_element(player->pieces, i, player->count);
            player->count--;
            break;
        }
    }
}

bool checkPieceOnPiece(const SDL_Point *point, const Player *player)
{
    for (int i = 0; i < player->count; i++)
    {
        if (SDL_PointInRect(point, player->pieces[i].rect))
            return true;
    }
    return false;
}

bool checkPieceOnSameTeam(const int x, const int y, const Player *player)
{
    for (int j = 0; j < player->count; j++)
    {
        if (x == getFirstDigit(player->pieces[j].rect->x) &&
            y == getFirstDigit(player->pieces[j].rect->y))
        {
            return true;
        }
    }
    return false;
}

static inline int getDistance(const SDL_Point *p1, const SDL_Point *p2)
{
    return abs(p2->x - p1->x) + abs(p2->y - p1->y);
}

bool canMoveDiag(const SDL_Point *p1, const SDL_Point *p2, const Player *player)
{
    for (int i = 1; i <= getDistance(p1, p2); i++)
    {
        if (p2->x > p1->x && p2->y > p1->y && checkPieceOnSameTeam(p1->x + i, p1->y + i, player))
            return false;
        else if (p2->x > p1->x && p2->y < p1->y && checkPieceOnSameTeam(p1->x + i, p1->y - i, player))
            return false;
        else if (p2->x < p1->x && p2->y > p1->y && checkPieceOnSameTeam(p1->x - i, p1->y + i, player))
            return false;
        else if (p2->x < p1->x && p2->y < p1->y && checkPieceOnSameTeam(p1->x - i, p1->y - i, player))
            return false;

        else if (p2->x + i == p1->x && p2->y + i == p1->y)
            return true;
        else if (p2->x - i == p1->x && p2->y - i == p1->y)
            return true;
        else if (p2->x - i == p1->x && p2->y + i == p1->y)
            return true;
        else if (p2->x + i == p1->x && p2->y - i == p1->y)
            return true;
    }

    return false;
}

bool canMoveHV(const SDL_Point *p1, const SDL_Point *p2, const Player *player)
{
    for (int i = 1; i <= getDistance(p1, p2); i++)
    {
        if (p2->x > p1->x && checkPieceOnSameTeam(p1->x + i, p1->y, player))
            return false;
        else if (p2->x < p1->x && checkPieceOnSameTeam(p1->x - i, p1->y, player))
            return false;
        else if (p2->y > p1->y && checkPieceOnSameTeam(p1->x, p1->y + i, player))
            return false;
        else if (p2->y < p1->y && checkPieceOnSameTeam(p1->x, p1->y - i, player))
            return false;
    }

    return !canMoveDiag(p1, p2, player);
}

bool canMoveKnight(const SDL_Point *p1, const SDL_Point *p2, const Player *player)
{
    for (int i = 0; i < ROW_COUNT; i++)
    {
        if (p2->x + X[i] == p1->x && p2->y + Y[i] == p1->y)
            return !checkPieceOnSameTeam(p2->x, p2->y, player);
    }
    return false;
}

static inline bool checkDifference(const int mouseY, const int oldY, const char player)
{
    return player == PLAYER_1 ? mouseY - oldY == 1 : oldY - mouseY == 1;
}

bool canMovePawn(const SDL_Point *p1, const SDL_Point *p2, const char player, const SDL_Point *mousePos)
{
    if (getDistance(p1, p2) > 1)
        return false;
    else if (checkPieceOnPiece(mousePos, getOpposition(player)))
        return false;
    else if (checkDifference(p2->y, p1->y, player) == 1 && p2->x == p1->x)
        return true;

    return false;
}

bool canMove(MouseEvent *mEvent)
{
    // printf("%s\n", mEvent->piece->firstMove ? "true" : "false");
    // mEvent->piece->firstMove = false;

    SDL_Point p1 = {.x = getFirstDigit(mEvent->oldPos.x),
                    .y = getFirstDigit(mEvent->oldPos.y)};

    SDL_Point p2 = {.x = getFirstDigit(mEvent->mousePos.x),
                    .y = getFirstDigit(mEvent->mousePos.y)};

    const Player *player = getPlayer(mEvent->piece->player);

    switch (mEvent->piece->initial)
    {
    case PAWN:
        return canMovePawn(&p1, &p2, mEvent->piece->player, &mEvent->mousePos);
    case ROOK:
        return canMoveHV(&p1, &p2, player);
    case KNIGHT:
        return canMoveKnight(&p1, &p2, player);
    case BISHOP:
        return canMoveDiag(&p1, &p2, player);
    case QUEEN:
        return canMoveDiag(&p1, &p2, player) ||
               canMoveHV(&p1, &p2, player);
    case KING:
        return (canMoveDiag(&p1, &p2, player) && getDistance(&p1, &p2) == 2) ||
               (canMoveHV(&p1, &p2, player) && getDistance(&p1, &p2) == 1);
    default:
        break;
    }

    return false;
}

void alignPiece(SDL_Rect *rect, const char player)
{
    SDL_Point point;
    point.x = rect->x + (rect->w / 2);
    point.y = rect->y + (rect->h / 2);

    int x = getFirstDigit(point.x);
    int y = getFirstDigit(point.y);

    if (SDL_PointInRect(&point, &board.squares[x][y].rect))
    {
        rect->x = board.squares[x][y].rect.x;
        rect->y = board.squares[x][y].rect.y;

        //
        Player *opposition = getOpposition(player);
        capture(&point, opposition);
    }
}

void checkIfPiece(SDL_Point *mousePos, SDL_Point *clickOffset, Piece *piece)
{
    for (int i = 0; i < board.p1.count; i++)
    {
        if (SDL_PointInRect(mousePos, board.p1.pieces[i].rect))
        {
            *piece = board.p1.pieces[i];
            clickOffset->x = mousePos->x - board.p1.pieces[i].rect->x;
            clickOffset->y = mousePos->y - board.p1.pieces[i].rect->y;

            break;
        }

        if (SDL_PointInRect(mousePos, board.p2.pieces[i].rect))
        {
            *piece = board.p2.pieces[i];
            clickOffset->x = mousePos->x - board.p2.pieces[i].rect->x;
            clickOffset->y = mousePos->y - board.p2.pieces[i].rect->y;

            break;
        }
    }
}

void printPieces(const Piece *pieces)
{
    for (size_t i = 0; i < PIECE_COUNT; i++)
    {
        printf("%c", pieces[i].initial);
        if (i == ROW_COUNT - 1)
        {
            printf("\n");
        }
    }
    printf("\n");
}

SVGString readSVGFile(const char *inputFilename)
{
    SVGString str;
    memset(&str, 0, sizeof(str));

    FILE *fh = fopen(inputFilename, "rb");
    if (fh != NULL)
    {
        fseek(fh, 0L, SEEK_END);
        str.len = ftell(fh);
        rewind(fh);
        str.buffer = malloc(str.len);
        if (str.buffer != NULL)
        {
            fread(str.buffer, str.len, 1, fh);
            // we can now close the file
            fclose(fh);
            fh = NULL;

            // do something, e.g.
            // fwrite(str.buffer, str.len, 1, stdout);

            // free(str.buffer);
        }
        if (fh != NULL)
            fclose(fh);
    }

    return str;
}

static inline SDL_Texture *getTexture(const SVGString svg)
{
    SDL_RWops *rw = SDL_RWFromConstMem(svg.buffer, svg.len);
    SDL_Surface *surface = IMG_Load_RW(rw, 1);
    return SDL_CreateTextureFromSurface(mainWindow.rend, surface);
}

Piece makePiece(const char initial, SDL_Texture *texture, const SDL_Point *pos, const char player)
{
    int w = 0;
    int h = 0;
    Piece piece = {.initial = initial, .player = player};

    piece.texture = texture;
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

void makePieces()
{
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

    for (int i = 0; i < ROW_COUNT; i++)
    {
        p1BackRow.x = i * OFFSET100;
        p1BackRow.y = HEIGHT - OFFSET800;

        p2BackRow.x = i * OFFSET100;
        p2BackRow.y = HEIGHT - OFFSET100;

        switch (i)
        {
        case 0:
        case 7:
            board.p1.pieces[i] = makePiece(ROOK, rookTexture, &p1BackRow, PLAYER_1);
            board.p2.pieces[i + ROW_COUNT] = makePiece(ROOK, rookTexture, &p2BackRow, PLAYER_2);
            break;
        case 1:
        case 6:
            board.p1.pieces[i] = makePiece(KNIGHT, knightTexture, &p1BackRow, PLAYER_1);
            board.p2.pieces[i + ROW_COUNT] = makePiece(KNIGHT, knightTexture, &p2BackRow, PLAYER_2);
            break;
        case 2:
        case 5:
            board.p1.pieces[i] = makePiece(BISHOP, bishopTexture, &p1BackRow, PLAYER_1);
            board.p2.pieces[i + ROW_COUNT] = makePiece(BISHOP, bishopTexture, &p2BackRow, PLAYER_2);
            break;
        case 3:
            board.p1.pieces[i] = makePiece(QUEEN, queenTexture, &p1BackRow, PLAYER_1);
            board.p2.pieces[i + ROW_COUNT] = makePiece(QUEEN, queenTexture, &p2BackRow, PLAYER_2);
            break;
        case 4:
            board.p1.pieces[i] = makePiece(KING, kingTexture, &p1BackRow, PLAYER_1);
            board.p2.pieces[i + ROW_COUNT] = makePiece(KING, kingTexture, &p2BackRow, PLAYER_2);
            break;
        default:
            break;
        }

        board.p1.pieces[i + ROW_COUNT] =
            makePiece(PAWN, pawnTexture, &(SDL_Point){.x = p1BackRow.x, .y = HEIGHT - OFFSET700}, PLAYER_1);
        board.p2.pieces[i] =
            makePiece(PAWN, pawnTexture, &(SDL_Point){.x = p2BackRow.x, .y = HEIGHT - OFFSET200}, PLAYER_2);
    }
}

void drawPieces()
{
    for (int i = 0; i < board.p1.count; i++)
    {
        SDL_RenderCopy(mainWindow.rend, board.p1.pieces[i].texture, NULL, board.p1.pieces[i].rect);
    }
    for (int i = 0; i < board.p2.count; i++)
    {
        SDL_RenderCopy(mainWindow.rend, board.p2.pieces[i].texture, NULL, board.p2.pieces[i].rect);
    }
}

void cleanUpPieces()
{
    for (int i = 0; i < board.p1.count; i++)
    {
        SDL_DestroyTexture(board.p1.pieces[i].texture);
        board.p1.pieces[i].texture = NULL;
        free(board.p1.pieces[i].rect);
        board.p1.pieces[i].rect = NULL;
    }
    for (int i = 0; i < board.p2.count; i++)
    {
        SDL_DestroyTexture(board.p2.pieces[i].texture);
        board.p2.pieces[i].texture = NULL;
        free(board.p2.pieces[i].rect);
        board.p2.pieces[i].rect = NULL;
    }
    free(board.p1.pieces);
    free(board.p2.pieces);
    board.p1.pieces = NULL;
    board.p2.pieces = NULL;
}
