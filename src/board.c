#include "board.h"

Square board[ROW_COUNT][ROW_COUNT];

unsigned int moves = 0;

const SDL_Color black = {0, 0, 0, 0};
const SDL_Color white = {255, 255, 255, 0};
const SDL_Color selectedColour = {255, 0, 0, 0};

void makeBoard()
{
    const int squareWidth = WIDTH / ROW_COUNT;

    bool bw = true;

    for (size_t i = 0; i < ROW_COUNT; i++)
    {
        for (size_t j = 0; j < ROW_COUNT; j++)
        {
            Square square;

            square.selected = false;

            // dimentions
            square.rect.w = squareWidth;
            square.rect.h = squareWidth;

            // location
            square.rect.x = squareWidth * i;
            square.rect.y = squareWidth * j;

            // colour
            square.colour = bw ? white : black;

            board[i][j] = square;
            bw = !bw;
        }
        bw = !bw;
    }
}

static inline void drawSquare(Square square)
{
    SDL_Color c = square.selected ? selectedColour : square.colour;
    SDL_SetRenderDrawColor(mainWindow.rend, c.r, c.g, c.b, c.a);
    SDL_RenderFillRect(mainWindow.rend, &square.rect);
}

void drawBoard()
{
    for (size_t i = 0; i < ROW_COUNT; i++)
    {
        drawSquare(board[0][i]);
        drawSquare(board[1][i]);
        drawSquare(board[2][i]);
        drawSquare(board[3][i]);
        drawSquare(board[4][i]);
        drawSquare(board[5][i]);
        drawSquare(board[6][i]);
        drawSquare(board[7][i]);
    }
}

void toggleBoardSquare(SDL_Point *mousePos, SDL_Point *clickOffset, Square **square)
{
    int x = getFirstDigit(mousePos->x);
    int y = getFirstDigit(mousePos->y);

    if (SDL_PointInRect(mousePos, &board[x][y].rect))
    {
        if (*square != NULL)
        {
            (*square)->selected = !(*square)->selected;
        }

        *square = &board[x][y];
        board[x][y].selected = !board[x][y].selected;
    }
}

void alignPiece(SDL_Rect *rect)
{
    SDL_Point point;
    point.x = rect->x + (rect->w / 2);
    point.y = rect->y + (rect->h / 2);

    int x = getFirstDigit(point.x);
    int y = getFirstDigit(point.y);

    if (SDL_PointInRect(&point, &board[x][y].rect))
    {
        rect->x = board[x][y].rect.x;
        rect->y = board[x][y].rect.y;

        return;
    }
}
