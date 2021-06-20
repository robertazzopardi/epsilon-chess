#include "board.h"
#include "svgutil.h"
#include "window.h"
#include <SDL_pixels.h>
#include <SDL_render.h>

#define CHESS_BOARD "./assets/864630-chess/svg/board/board.svg"

Board *makeBoard(Window *mainWindow) {
    Board *board = malloc(1 * sizeof(*board));

    // Set up the Boards texture
    long fsize;
    char *string = openFile(CHESS_BOARD, &fsize);
    board->texture = makeTexture(mainWindow->rend, string, fsize);
    free(string);
    string = NULL;

    // Set up the boards rect
    board->rect = malloc(1 * sizeof(*board->rect));
    board->rect->w = WIDTH;
    board->rect->h = HEIGHT;
    board->rect->x = 0;
    board->rect->y = 0;

    // Set up players
    board->p1 = malloc(1 * sizeof(*board->p1));
    board->p2 = malloc(1 * sizeof(*board->p2));

    // Set up the selected rect
    board->selectedRect = malloc(1 * sizeof(*board->selectedRect));
    board->selectedRect->w = WIDTH / ROW_COUNT;
    board->selectedRect->h = WIDTH / ROW_COUNT;
    board->selectedRect->x = 0;
    board->selectedRect->y = 0;
    board->selectedVisible = 0;

    return board;
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

    free(board->selectedRect);
    board->selectedRect = NULL;

    for (int i = 0; i < ROW_COUNT; i++) {
        free(board->pieces[i]);
        board->pieces[i] = NULL;
    }
    free(board->pieces);
    board->pieces = NULL;

    free(board);
    board = NULL;
}
