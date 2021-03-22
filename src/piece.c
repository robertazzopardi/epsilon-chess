#include "piece.h"

#define OFFSET100 100
#define OFFSET800 800
#define OFFSET700 700
#define OFFSET200 200

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define SAME_TEAM(n, i) (                                            \
    getFirstDigit(point->x) == getFirstDigit(n.pieces[i].rect->x) && \
    getFirstDigit(point->y) == getFirstDigit(n.pieces[i].rect->y))

typedef struct
{
    long len;
    char *buffer;
} SVGString;

static inline Player getPlayer(char player)
{
    return player == PLAYER_1 ? board.p1 : board.p2;
}

static inline Player getOpposition(char player)
{
    return player == PLAYER_1 ? board.p2 : board.p1;
}

bool checkPieceOnPiece(SDL_Point *point, Player player)
{
    for (int i = 0; i < player.count; i++)
    {
        if (SDL_PointInRect(point, player.pieces[i].rect))
            return true;
    }
    return false;
}

bool checkPieceOnSameTeam(int x, int y, Player player)
{
    for (int j = 0; j < player.count; j++)
    {
        if (x == getFirstDigit(player.pieces[j].rect->x) &&
            y == getFirstDigit(player.pieces[j].rect->y))
        {
            return false;
        }
    }
    return true;
}

bool canMoveDiag(int mouseX, int mouseY, int oldX, int oldY)
{
    int squaresMoved = abs(mouseX - oldX) + abs(mouseY - oldY);
    for (int i = 0; i < squaresMoved; i++)
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

bool canMoveHV(int mouseX, int mouseY, int oldX, int oldY, Player player)
{
    int squaresMoved = abs(mouseX - oldX) + abs(mouseY - oldY);

    for (int i = 1; i <= squaresMoved; i++)
    {
        if (mouseX > oldX && !checkPieceOnSameTeam(oldX + i, oldY, player))
            return false;
        else if (mouseX < oldX && !checkPieceOnSameTeam(oldX - i, oldY, player))
            return false;
        else if (mouseY > oldY && !checkPieceOnSameTeam(oldX, oldY + i, player))
            return false;
        else if (mouseY < oldY && !checkPieceOnSameTeam(oldX, oldY - i, player))
            return false;
    }

    return !canMoveDiag(mouseX, mouseY, oldX, oldY);
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

static inline bool checkDifference(int mouseY, int oldY, char player)
{
    return player == PLAYER_1 ? mouseY - oldY == 1 : oldY - mouseY == 1;
}

bool canMovePawn(int mouseX, int mouseY, int oldX, int oldY, char player, SDL_Point mousePos)
{
    if (checkPieceOnPiece(&mousePos, getOpposition(player)))
        return false;
    else if (checkDifference(mouseY, oldY, player) == 1 && mouseX == oldX)
        return true;

    return false;
}

bool canMove(Piece piece, SDL_Point oldPos, SDL_Point mousePos)
{
    int oldX = getFirstDigit(oldPos.x);
    int oldY = getFirstDigit(oldPos.y);

    int mouseX = getFirstDigit(mousePos.x);
    int mouseY = getFirstDigit(mousePos.y);

    // if (checkPieceOnSameTeam(&mousePos, piece.player))
    //     return false;

    switch (piece.initial)
    {
    case PAWN:
        return canMovePawn(mouseX, mouseY, oldX, oldY, piece.player, mousePos);
    case ROOK:
        return canMoveHV(mouseX, mouseY, oldX, oldY, getPlayer(piece.player));
    case KNIGHT:
        return canMoveKnight(mouseX, mouseY, oldX, oldY);
    case BISHOP:
        return canMoveDiag(mouseX, mouseY, oldX, oldY);
    case QUEEN:
        return canMoveDiag(mouseX, mouseY, oldX, oldY) ||
               canMoveHV(mouseX, mouseY, oldX, oldY, getPlayer(piece.player));
    case KING:
        return canMoveDiag(mouseX, mouseY, oldX, oldY) ||
               canMoveHV(mouseX, mouseY, oldX, oldY, getPlayer(piece.player));
    default:
        break;
    }

    return false;
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
    }

    for (int i = 0; i < board.p2.count; i++)
    {
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
    Piece piece = {.initial = initial, .player = player};

    piece.texture = texture;
    SDL_QueryTexture(piece.texture, NULL, NULL, &w, &h);

    SDL_Rect *rect = NULL;
    rect = calloc(1, sizeof(*rect));
    rect->h = HEIGHT / ROW_COUNT;
    rect->w = WIDTH / ROW_COUNT;
    rect->x = x;
    rect->y = y;

    piece.rect = rect;

    memset(&piece.firstMove, true, sizeof *piece.firstMove);

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

    for (size_t i = 0; i < ROW_COUNT; i++)
    {
        switch (i)
        {
        case 0:
        case 7:
            board.p1.pieces[i] = makePiece(ROOK, rookTexture, i * OFFSET100, HEIGHT - OFFSET800, PLAYER_1);
            board.p2.pieces[i + ROW_COUNT] = makePiece(ROOK, rookTexture, i * OFFSET100, HEIGHT - OFFSET100, PLAYER_2);
            break;
        case 1:
        case 6:
            board.p1.pieces[i] = makePiece(KNIGHT, knightTexture, i * OFFSET100, HEIGHT - OFFSET800, PLAYER_1);
            board.p2.pieces[i + ROW_COUNT] = makePiece(KNIGHT, knightTexture, i * OFFSET100, HEIGHT - OFFSET100, PLAYER_2);
            break;
        case 2:
        case 5:
            board.p1.pieces[i] = makePiece(BISHOP, bishopTexture, i * OFFSET100, HEIGHT - OFFSET800, PLAYER_1);
            board.p2.pieces[i + ROW_COUNT] = makePiece(BISHOP, bishopTexture, i * OFFSET100, HEIGHT - OFFSET100, PLAYER_2);
            break;
        case 3:
            board.p1.pieces[i] = makePiece(QUEEN, queenTexture, i * OFFSET100, HEIGHT - OFFSET800, PLAYER_1);
            board.p2.pieces[i + ROW_COUNT] = makePiece(QUEEN, queenTexture, i * OFFSET100, HEIGHT - OFFSET100, PLAYER_2);
            break;
        case 4:
            board.p1.pieces[i] = makePiece(KING, kingTexture, i * OFFSET100, HEIGHT - OFFSET800, PLAYER_1);
            board.p2.pieces[i + ROW_COUNT] = makePiece(KING, kingTexture, i * OFFSET100, HEIGHT - OFFSET100, PLAYER_2);
            break;
        default:
            break;
        }

        board.p1.pieces[i + ROW_COUNT] = makePiece(PAWN, pawnTexture, i * OFFSET100, HEIGHT - OFFSET700, PLAYER_1);
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
    free(board.p1.pieces);
    free(board.p2.pieces);
}
