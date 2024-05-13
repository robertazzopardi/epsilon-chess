#include <SDL_render.h>

#include "board.h"
#include "piece.h"
#include "svg_util.h"
#include "window.h"

#define CHESS_BOARD "./assets/864630-chess/svg/board/board.svg"

Board *makeBoard(SDL_Renderer *renderer) {
    Board *board = malloc(1 * sizeof(*board));

    board->toMove = PLAYER_2;

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

    // Set up players
    board->p1 = malloc(1 * sizeof(*board->p1));
    board->p2 = malloc(1 * sizeof(*board->p2));
    board->p1->opposition = board->p2;
    board->p2->opposition = board->p1;

    // Set up the selected rect
    board->selectedRect = malloc(1 * sizeof(*board->selectedRect));
    board->selectedRect->w = WIDTH / ROW_COUNT;
    board->selectedRect->h = WIDTH / ROW_COUNT;
    board->selectedRect->x = 0;
    board->selectedRect->y = 0;
    board->selectedVisible = 0;

    return board;
}

void generateMoves(Board *board) {
    for (unsigned char i = 0; i < board->p1->piecesRemaining; i++) {
        generatePieceMoves(&board->p1->pieces[i], board->p1);
    }

    for (unsigned char i = 0; i < board->p2->piecesRemaining; i++) {
        generatePieceMoves(&board->p2->pieces[i], board->p2);
    }

    for (unsigned char i = 0; i < board->p1->piecesRemaining; i++) {
        calculateIfMovesAreValid(&board->p1->pieces[i], board->p1);
    }

    for (unsigned char i = 0; i < board->p2->piecesRemaining; i++) {
        calculateIfMovesAreValid(&board->p2->pieces[i], board->p2);
    }
}

void cleanUpBoard(Board *board) {
    SDL_DestroyTexture(board->texture);
    board->texture = NULL;

    free(board->rect);
    board->rect = NULL;

    free(board->p1);
    board->p1 = NULL;
    free(board->p2);
    board->p2 = NULL;

    free(board->selectedRect);
    board->selectedRect = NULL;

    free(board);
    board = NULL;
}
