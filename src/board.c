#include "board.h"

const SDL_Color black = {0, 0, 0, 0};
const SDL_Color white = {255, 255, 255, 0};
const SDL_Color selectedColour = {150, 150, 150, 0};

Board board;

void makeBoard()
{
    const int squareWidth = WIDTH / ROW_COUNT;

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
            square.colour = (i + j) % 2 == 0 ? white : black;

            board.squares[i][j] = square;
        }
        // Could swap pieces or sort to have equal halfs
    }
}

void drawBoard()
{
    for (size_t i = 0; i < ROW_COUNT; i++)
    {
        for (size_t j = 0; j < ROW_COUNT; j++)
        {
            SDL_Color c = board.squares[i][j].selected ? selectedColour : board.squares[i][j].colour;
            SDL_SetRenderDrawColor(mainWindow.rend, c.r, c.g, c.b, c.a);
            SDL_RenderFillRect(mainWindow.rend, &board.squares[i][j].rect);
        }
    }
}

void toggleBoardSquare(const SDL_Point *mousePos, Square **square)
{
    int x = getFirstDigit(mousePos->x);
    int y = getFirstDigit(mousePos->y);

    if (SDL_PointInRect(mousePos, &board.squares[x][y].rect))
    {
        if (*square != NULL)
            (*square)->selected = !(*square)->selected;

        *square = &board.squares[x][y];
        board.squares[x][y].selected = !board.squares[x][y].selected;
    }
}
