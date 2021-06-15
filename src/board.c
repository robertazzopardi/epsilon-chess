#include "board.h"
#include "svgutil.h"
#include "window.h"
#include <SDL_pixels.h>
#include <SDL_render.h>

#define CHESS_BOARD "./resources/864630-chess/svg/board/board.svg"

Board *makeBoard(Window *mainWindow) {
    Board *board = malloc(1 * sizeof(*board));
    long fsize;
    char *string = openFile(CHESS_BOARD, &fsize);
    board->texture = makeTexture(mainWindow->rend, string, fsize);
    free(string);
    string = NULL;

    int w = 0;
    int h = 0;
    SDL_QueryTexture(board->texture, NULL, NULL, &w, &h);

    board->rect = malloc(1 * sizeof(*board->rect));
    board->rect->h = HEIGHT;
    board->rect->w = WIDTH;
    board->rect->x = 0;
    board->rect->y = 0;

    board->p1 = malloc(1 * sizeof(*board->p1));
    board->p2 = malloc(1 * sizeof(*board->p2));

    //

    // const int squareWidth = WIDTH / ROW_COUNT;

    // for (size_t i = 0; i < ROW_COUNT; i++) {
    //     for (size_t j = 0; j < ROW_COUNT; j++) {
    //         Square square;

    //         square.selected = false;

    //         // dimentions
    //         square.rect->w = squareWidth;
    //         square.rect->h = squareWidth;

    //         // location
    //         square.rect->x = squareWidth * i;
    //         square.rect->y = squareWidth * j;

    //         // colour
    //         // square.colour = (i + j) % 2 == 0 ? white : black;

    //         board.squares[i][j] = square;
    //     }
    // }
    return board;
}

// void drawBoard(Window *mainWindow, Board *board) {
//     // for (size_t i = 0; i < ROW_COUNT; i++) {
//     //     for (size_t j = 0; j < ROW_COUNT; j++) {
//     //         SDL_Color c = board.squares[i][j].selected ? selectedColour
//     //                                                    :
//     // board.squares[i][j].colour;
//     //         SDL_SetRenderDrawColor(mainWindow.rend, c.r, c.g, c.b, c.a);
//     //         SDL_RenderFillRect(mainWindow.rend,
//     &board.squares[i][j].rect);
//     //     }
//     // }

//     SDL_RenderCopy(mainWindow->rend, board->texture, NULL, board->rect);
// }

// void toggleBoardSquare(const SDL_Point *mousePos, Square **square) {
//     // SDL_Point pos = getPos(mousePos->x, mousePos->y);

//     printf("%d, %d", mousePos->x, (*square)->selected);

//     // if (SDL_PointInRect(mousePos, board.squares[pos.x][pos.y].rect)) {
//     //     if (*square != NULL)
//     //         (*square)->selected = !(*square)->selected;

//     //     *square = &board.squares[pos.x][pos.y];
//     //     board.squares[pos.x][pos.y].selected =
//     !board.squares[pos.x][pos.y].selected;
//     // }
// }

SDL_Point getPos(int x, int y) {
    int lenX = floor(log10(x)) + 1;
    int lenY = floor(log10(y)) + 1;

    int px = x / (int)pow(10, lenX - 1);
    int py = y / (int)pow(10, lenY - 1);

    if (lenX <= 2)
        px = 0;
    if (lenY <= 2)
        py = 0;

    return (SDL_Point){.x = px, .y = py};
}

void cleanUpBoard(Board *board) {
    SDL_DestroyTexture(board->texture);
    board->texture = NULL;

    free(board->rect);
    board->rect = NULL;

    free(board->p1);
    free(board->p2);
    board->p1 = NULL;
    board->p2 = NULL;

    free(board);
    board = NULL;
}
