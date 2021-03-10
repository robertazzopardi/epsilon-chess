#include "board.h"

Board makeBoard()
{
    const int squareWidth = WIDTH / SQUARES_SIZE;

    Board board;

    memset(board.squares, 0, sizeof board.squares);

    bool bw = true;

    for (size_t i = 0; i < SQUARES_SIZE; i++)
    {
        for (size_t j = 0; j < SQUARES_SIZE; j++)
        {
            Square square;
            // dimentions
            square.rect.w = squareWidth;
            square.rect.h = squareWidth;

            // location
            square.rect.x = squareWidth * i;
            square.rect.y = squareWidth * j;

            // colour
            square.colour.a = 255;

            square.colour.r = bw ? 255 : 0;
            square.colour.g = bw ? 255 : 0;
            square.colour.b = bw ? 255 : 0;

            board.squares[i][j] = square;
            bw = !bw;
        }
        bw = !bw;
    }

    return board;
}

void drawSquare(SDL_Renderer *renderer, Square square)
{

    SDL_Color c = square.colour;
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderFillRect(renderer, &square.rect);
}

void drawBoard(SDL_Renderer *renderer, Board *board)
{
    SDL_RenderClear(renderer);
    for (size_t i = 0; i < SQUARES_SIZE; i++)
    {
        for (size_t j = 0; j < SQUARES_SIZE; j++)
        {
            drawSquare(renderer, board->squares[i][j]);
        }
    }
}
