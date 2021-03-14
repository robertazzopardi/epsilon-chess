#include "piece.h"

#define PIECE_COUNT 16
#define ROW 8
#define COL 8

#define OFFSET100 100
#define OFFSET800 800
#define OFFSET700 700
#define OFFSET200 200

typedef struct
{
    long len;
    char *buffer;
} SVGString;

typedef struct
{
    Piece pieces[PIECE_COUNT];
    int count;
} PlayerOnePieces;

typedef struct
{
    Piece pieces[PIECE_COUNT];
    int count;
} PlayerTwoPieces;

PlayerOnePieces p1;
PlayerTwoPieces p2;

bool canMove(Piece piece, SDL_Point oldPos)
{
    int x = getFirstDigit(piece.rect->x);
    int y = getFirstDigit(piece.rect->y);

    int oldX = getFirstDigit(oldPos.x);
    int oldY = getFirstDigit(oldPos.y);

    switch (piece.initial)
    {
    case PAWN:
        if (piece.firstMove)
        {
            if (piece.player == PLAYER_2 && oldY - y <= 3 && oldY - y > 0)
            {
                piece.firstMove = false;
                return true;
            }
            else if (piece.player == PLAYER_1 && oldY + y <= 3 && oldY + y > 0)
            {
                piece.firstMove = false;
                return true;
            }
        }
        break;
    case ROOK:
        break;
    case KNIGHT:
        break;
    case BISHOP:
        break;
    case QUEEN:
        break;
    case KING:
        break;
    default:
        break;
    }

    return false;
}

void checkIfPiece(SDL_Point *mousePos, SDL_Point *clickOffset, Piece *piece)
{
    for (size_t i = 0; i < PIECE_COUNT; i++)
    {
        if (SDL_PointInRect(mousePos, p1.pieces[i].rect))
        {
            *piece = p1.pieces[i];
            clickOffset->x = mousePos->x - p1.pieces[i].rect->x;
            clickOffset->y = mousePos->y - p1.pieces[i].rect->y;

            break;
        }

        if (SDL_PointInRect(mousePos, p2.pieces[i].rect))
        {
            *piece = p2.pieces[i];
            clickOffset->x = mousePos->x - p2.pieces[i].rect->x;
            clickOffset->y = mousePos->y - p2.pieces[i].rect->y;

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

    piece.firstMove = true;

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

    p1.count = PIECE_COUNT;
    p2.count = PIECE_COUNT;

    for (size_t i = 0; i < ROW; i++)
    {
        switch (i)
        {
        case 0:
        case 7:
            p1.pieces[i] = makePiece(ROOK, rookTexture, i * OFFSET100, HEIGHT - OFFSET800, PLAYER_1);
            p2.pieces[i + ROW] = makePiece(ROOK, rookTexture, i * OFFSET100, HEIGHT - OFFSET100, PLAYER_2);
            break;
        case 1:
        case 6:
            p1.pieces[i] = makePiece(KNIGHT, knightTexture, i * OFFSET100, HEIGHT - OFFSET800, PLAYER_1);
            p2.pieces[i + ROW] = makePiece(KNIGHT, knightTexture, i * OFFSET100, HEIGHT - OFFSET100, PLAYER_2);
            break;
        case 2:
        case 5:
            p1.pieces[i] = makePiece(BISHOP, bishopTexture, i * OFFSET100, HEIGHT - OFFSET800, PLAYER_1);
            p2.pieces[i + ROW] = makePiece(BISHOP, bishopTexture, i * OFFSET100, HEIGHT - OFFSET100, PLAYER_2);
            break;
        case 3:
            p1.pieces[i] = makePiece(QUEEN, queenTexture, i * OFFSET100, HEIGHT - OFFSET800, PLAYER_1);
            p2.pieces[i + ROW] = makePiece(QUEEN, queenTexture, i * OFFSET100, HEIGHT - OFFSET100, PLAYER_2);
            break;
        case 4:
            p1.pieces[i] = makePiece(KING, kingTexture, i * OFFSET100, HEIGHT - OFFSET800, PLAYER_1);
            p2.pieces[i + ROW] = makePiece(KING, kingTexture, i * OFFSET100, HEIGHT - OFFSET100, PLAYER_2);
            break;
        default:
            break;
        }

        p1.pieces[i + ROW] = makePiece(PAWN, pawnTexture, i * OFFSET100, HEIGHT - OFFSET700, PLAYER_1);
        p2.pieces[i] = makePiece(PAWN, pawnTexture, i * OFFSET100, HEIGHT - OFFSET200, PLAYER_2);
    }
}

void drawPieces()
{
    for (size_t i = 0; i < PIECE_COUNT; i++)
    {
        SDL_RenderCopy(mainWindow.rend, p1.pieces[i].texture, NULL, p1.pieces[i].rect);
        SDL_RenderCopy(mainWindow.rend, p2.pieces[i].texture, NULL, p2.pieces[i].rect);
    }
}

void cleanUpPieces()
{
    for (size_t i = 0; i < PIECE_COUNT; i++)
    {
        SDL_DestroyTexture(p1.pieces[i].texture);
        SDL_DestroyTexture(p2.pieces[i].texture);
    }
}
