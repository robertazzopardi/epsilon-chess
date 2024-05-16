#include <SDL_render.h>

#include "board.h"
#include "piece.h"
#include "svg_util.h"
#include "window.h"

#define CHESS_BOARD "./assets/864630-chess/svg/board/board.svg"

Board *make_board(SDL_Renderer *renderer) {
    Board *board = (Board *)malloc(sizeof(*board));

    // Set up the Boards texture
    long fsize = 0L;
    char *string = openFile(CHESS_BOARD, &fsize);
    board->texture = makeTexture(renderer, string, fsize);
    free(string);
    string = NULL;

    // Set up the boards rect
    board->rect = malloc(1 * sizeof(*board->rect));
    board->rect->w = WIDTH;
    board->rect->h = HEIGHT;
    board->rect->x = 0;
    board->rect->y = 0;

    board->selected_square = EMPTY;

    return board;
}

void clean_up_board(Board *board) {
    SDL_DestroyTexture(board->texture);
    board->texture = NULL;

    free(board->rect);
    board->rect = NULL;

    free(board);
    board = NULL;
}
