#include "piece.h"

#define ROW 8
#define COL 8

#define OFFSET100 100
#define OFFSET800 800
#define OFFSET700 700
#define OFFSET200 200

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define SAME_TEAM(n, i) (getFirstDigit(point->x) == getFirstDigit(n.pieces[i].rect->x) && \
                         getFirstDigit(point->y) == getFirstDigit(n.pieces[i].rect->y))

typedef struct
{
    long len;
    char *buffer;
} SVGString;

bool checkPieceOnPiece(SDL_Point *point, char player)
{
    for (size_t i = 0; i < PIECE_COUNT; i++)
    {
        if (player == PLAYER_2 && SDL_PointInRect(point, board.p1.pieces[i].rect))
            return true;
        if (player == PLAYER_1 && SDL_PointInRect(point, board.p2.pieces[i].rect))
            return true;
    }
    return false;
}

bool checkPieceOnSameTeam(SDL_Point *point, char player)
{
    for (size_t i = 0; i < PIECE_COUNT; i++)
    {
        if (player == PLAYER_2)
        {
            if (SAME_TEAM(board.p2, i))
                return true;
        }
        else
        {
            if (SAME_TEAM(board.p1, i))
                return true;
        }
    }
    return false;
}

bool canMoveHV(int mouseX, int mouseY, int oldX, int oldY, int dist)
{
    for (int i = 0; i < dist; i++)
    {
        if (mouseX == oldX && (mouseY + i == oldY || mouseY - i == oldY))
            return true;
        else if (mouseY == oldY && (mouseX + i == oldX || mouseX - i == oldX))
            return true;
    }
    return false;
}

bool canMoveKnight(int mouseX, int mouseY, int oldX, int oldY)
{
    if (mouseX + 1 == oldX && mouseY + 2 == oldY)
        return true;
    else if (mouseX - 1 == oldX && mouseY - 2 == oldY)
        return true;
    else if (mouseX + 1 == oldX && mouseY - 2 == oldY)
        return true;
    else if (mouseX - 1 == oldX && mouseY + 2 == oldY)
        return true;
    else if (mouseX + 2 == oldX && mouseY + 1 == oldY)
        return true;
    else if (mouseX - 2 == oldX && mouseY - 1 == oldY)
        return true;
    else if (mouseX + 2 == oldX && mouseY - 1 == oldY)
        return true;
    else if (mouseX - 2 == oldX && mouseY + 1 == oldY)
        return true;
    return false;
}

bool canMoveDiag(int mouseX, int mouseY, int oldX, int oldY, int dist)
{
    for (int i = 0; i < dist; i++)
    {
        if (mouseX + i == oldX && mouseY + i == oldY)
            return true;
        else if (mouseX - i == oldX && mouseY - i == oldY)
            return true;
        else if (mouseX - i == oldX && mouseY + i == oldY)
            return true;
        else if (mouseX + i == oldX && mouseY - i == oldY)
            return true;
    }
    return false;
}

bool canMove(Piece piece, SDL_Point oldPos, SDL_Point mousePos)
{
    int oldX = getFirstDigit(oldPos.x);
    int oldY = getFirstDigit(oldPos.y);

    int mouseX = getFirstDigit(mousePos.x);
    int mouseY = getFirstDigit(mousePos.y);

    if (checkPieceOnSameTeam(&mousePos, piece.player))
        return false;

    switch (piece.initial)
    {
    case PAWN:
        switch (piece.player)
        {
        case PLAYER_1:
            if (checkPieceOnPiece(&mousePos, piece.player))
            {
                return false;
            }
            else if (mouseY - oldY == 1 && mouseX == oldX)
            {
                return true;
            }
            break;
        case PLAYER_2:
            if (checkPieceOnPiece(&mousePos, piece.player))
            {
                return false;
            }
            else if (oldY - mouseY == 1 && mouseX == oldX)
            {
                return true;
            }
            break;
        default:
            break;
        }
        break;
    case ROOK:
        return canMoveHV(mouseX, mouseY, oldX, oldY, ROW_COUNT);
    case KNIGHT:
        return canMoveKnight(mouseX, mouseY, oldX, oldY);
    case BISHOP:
        return canMoveDiag(mouseX, mouseY, oldX, oldY, ROW_COUNT);
    case QUEEN:
        return canMoveDiag(mouseX, mouseY, oldX, oldY, ROW_COUNT) ||
               canMoveHV(mouseX, mouseY, oldX, oldY, ROW_COUNT);
    case KING:
        return canMoveDiag(mouseX, mouseY, oldX, oldY, 2) ||
               canMoveHV(mouseX, mouseY, oldX, oldY, 2);
    default:
        break;
    }

    return false;
}

void checkIfPiece(SDL_Point *mousePos, SDL_Point *clickOffset, Piece *piece)
{
    for (size_t i = 0; i < PIECE_COUNT; i++)
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

void printPieces(Piece *pieces)
{
    for (size_t i = 0; i < PIECE_COUNT; i++)
    {
        printf("%c", pieces[i].initial);
        if (i == ROW - 1)
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

static inline SDL_Texture *getTexture(SVGString svg)
{
    SDL_RWops *rw = SDL_RWFromConstMem(svg.buffer, svg.len);
    SDL_Surface *surface = IMG_Load_RW(rw, 1);
    return SDL_CreateTextureFromSurface(mainWindow.rend, surface);
}

Piece makePiece(const char initial, SDL_Texture *texture, int x, int y, char player)
{
    int w = 0;
    int h = 0;
    Piece piece = {.initial = initial};

    piece.texture = texture;
    SDL_QueryTexture(piece.texture, NULL, NULL, &w, &h);

    SDL_Rect *rect = NULL;
    rect = calloc(1, sizeof(*rect));
    rect->h = HEIGHT / ROW;
    rect->w = WIDTH / ROW;
    rect->x = x;
    rect->y = y;

    piece.rect = rect;

    // *piece.firstMove = true;
    memset(&piece.firstMove, true, sizeof *piece.firstMove);

    piece.player = player;

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

    for (size_t i = 0; i < ROW; i++)
    {
        switch (i)
        {
        case 0:
        case 7:
            board.p1.pieces[i] = makePiece(ROOK, rookTexture, i * OFFSET100, HEIGHT - OFFSET800, PLAYER_1);
            board.p2.pieces[i + ROW] = makePiece(ROOK, rookTexture, i * OFFSET100, HEIGHT - OFFSET100, PLAYER_2);
            break;
        case 1:
        case 6:
            board.p1.pieces[i] = makePiece(KNIGHT, knightTexture, i * OFFSET100, HEIGHT - OFFSET800, PLAYER_1);
            board.p2.pieces[i + ROW] = makePiece(KNIGHT, knightTexture, i * OFFSET100, HEIGHT - OFFSET100, PLAYER_2);
            break;
        case 2:
        case 5:
            board.p1.pieces[i] = makePiece(BISHOP, bishopTexture, i * OFFSET100, HEIGHT - OFFSET800, PLAYER_1);
            board.p2.pieces[i + ROW] = makePiece(BISHOP, bishopTexture, i * OFFSET100, HEIGHT - OFFSET100, PLAYER_2);
            break;
        case 3:
            board.p1.pieces[i] = makePiece(QUEEN, queenTexture, i * OFFSET100, HEIGHT - OFFSET800, PLAYER_1);
            board.p2.pieces[i + ROW] = makePiece(QUEEN, queenTexture, i * OFFSET100, HEIGHT - OFFSET100, PLAYER_2);
            break;
        case 4:
            board.p1.pieces[i] = makePiece(KING, kingTexture, i * OFFSET100, HEIGHT - OFFSET800, PLAYER_1);
            board.p2.pieces[i + ROW] = makePiece(KING, kingTexture, i * OFFSET100, HEIGHT - OFFSET100, PLAYER_2);
            break;
        default:
            break;
        }

        board.p1.pieces[i + ROW] = makePiece(PAWN, pawnTexture, i * OFFSET100, HEIGHT - OFFSET700, PLAYER_1);
        board.p2.pieces[i] = makePiece(PAWN, pawnTexture, i * OFFSET100, HEIGHT - OFFSET200, PLAYER_2);
    }
}

void drawPieces()
{
    for (size_t i = 0; i < PIECE_COUNT; i++)
    {
        SDL_RenderCopy(mainWindow.rend, board.p1.pieces[i].texture, NULL, board.p1.pieces[i].rect);
        SDL_RenderCopy(mainWindow.rend, board.p2.pieces[i].texture, NULL, board.p2.pieces[i].rect);
    }
}

void cleanUpPieces()
{
    for (size_t i = 0; i < PIECE_COUNT; i++)
    {
        SDL_DestroyTexture(board.p1.pieces[i].texture);
        SDL_DestroyTexture(board.p2.pieces[i].texture);
    }
}
