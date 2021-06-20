#include "window.h"
#include "board.h"
#include "piece.h"
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_events.h>
#include <SDL_image.h>

#define SQUARE_COUNT 64
#define TITLE "Chess"
#define FRAME_DELAY 1000 / 60
#define CELL_RADIUS ((WIDTH / ROW_COUNT) / 2)

static void handleEvents(bool *running, MouseEvent *mEvent, Board *board) {
    while (SDL_PollEvent(mEvent->event)) {
        switch (mEvent->event->type) {
        case SDL_QUIT:
            *running = false;
            break;
        case SDL_MOUSEBUTTONUP:
            if (mEvent->LMBDown &&
                mEvent->event->button.button == SDL_BUTTON_LEFT) {

                if (mEvent->pieceSelected) {

                    // if (canMoveFunc(mEvent, board)) {
                    if (canMovePiece(mEvent)) {

                        board->moveCount++;

                        // Align the pieces
                        alignPiece(mEvent, board);

                        // Swap the boards virtual pieces
                        Piece p = board->pieces[mEvent->piece->rect->y / 100]
                                               [mEvent->piece->rect->x / 100];
                        board->pieces[mEvent->piece->rect->y / 100]
                                     [mEvent->piece->rect->x / 100] =
                            board->pieces[mEvent->oldPos->y / 100]
                                         [mEvent->oldPos->x / 100];
                        board->pieces[mEvent->oldPos->y / 100]
                                     [mEvent->oldPos->x / 100] = p;

                        if (mEvent->piece->firstMove)
                            mEvent->piece->firstMove = false;

                        mEvent->piece = NULL;

                        // printPieces(board);
                    } else {
                        mEvent->piece->rect->x = mEvent->oldPos->x;
                        mEvent->piece->rect->y = mEvent->oldPos->y;
                    }
                }
                mEvent->LMBDown = false;
                mEvent->pieceSelected = false;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (!mEvent->LMBDown &&
                mEvent->event->button.button == SDL_BUTTON_LEFT) {
                mEvent->LMBDown = true;

                mEvent->piece = NULL;
                checkIfPiece(mEvent, board->p1);
                checkIfPiece(mEvent, board->p2);

                if (mEvent->piece)
                    generateMoves(mEvent, board);

                // Save old position
                if (mEvent->pieceSelected) {
                    mEvent->oldPos->x = mEvent->piece->rect->x;
                    mEvent->oldPos->y = mEvent->piece->rect->y;
                }

                // Toggle the colour of a square on the board
                SDL_Point point = {mEvent->mousePos->x, mEvent->mousePos->y};
                if (SDL_PointInRect(&point, board->selectedRect) ||
                    !board->selectedVisible) {
                    board->selectedVisible = !board->selectedVisible;
                }

                board->selectedRect->x = (mEvent->mousePos->x / 100) * 100;
                board->selectedRect->y = (mEvent->mousePos->y / 100) * 100;
            }
            break;
        case SDL_MOUSEMOTION: {
            mEvent->mousePos->x = mEvent->event->motion.x;
            mEvent->mousePos->y = mEvent->event->motion.y;

            if (mEvent->LMBDown && mEvent->pieceSelected) {
                board->selectedVisible = false;
                mEvent->piece->rect->x =
                    mEvent->mousePos->x - mEvent->offset->x;
                mEvent->piece->rect->y =
                    mEvent->mousePos->y - mEvent->offset->y;
            }
        } break;
        }
    }
}

static void gameLoop(Window *mainWindow, Board *board) {
    // controls annimation loop
    bool running = true;

    MouseEvent mEvent = {0};
    mEvent.mousePos = malloc(1 * sizeof(*mEvent.mousePos));
    mEvent.offset = malloc(1 * sizeof(*mEvent.offset));
    // mEvent.piece = malloc(1 * sizeof(*mEvent.piece));
    mEvent.piece = NULL;
    mEvent.pieceSelected = false;
    mEvent.oldPos = malloc(1 * sizeof(*mEvent.oldPos));
    mEvent.event = malloc(1 * sizeof(*mEvent.event));

    // Main loop
    while (running) {
        handleEvents(&running, &mEvent, board);

        SDL_SetRenderDrawColor(mainWindow->rend, 0, 0, 0, 255);

        // Clear the screen
        SDL_RenderClear(mainWindow->rend);

        // Render board
        SDL_RenderCopy(mainWindow->rend, board->texture, NULL, board->rect);

        // Render selected square
        if (board->selectedVisible) {
            SDL_SetRenderDrawColor(mainWindow->rend, 100, 100, 100, 100);
            SDL_RenderFillRect(mainWindow->rend, board->selectedRect);
        }

        // Render possible moves
        if (mEvent.piece && mEvent.piece->moves->count > 0) {
            for (int i = 0; i < mEvent.piece->moves->count; i++) {
                int mX =
                    (mEvent.piece->moves->squares[i].x * 100) + CELL_RADIUS;
                int mY =
                    (mEvent.piece->moves->squares[i].y * 100) + CELL_RADIUS;

                filledCircleRGBA(mainWindow->rend, mX, mY, 30, 100, 100, 100,
                                 100);
            }
        }

        // Render the pieces
        drawPieces(mainWindow, board);

        // triggers the double buffers for multiple rendering
        SDL_RenderPresent(mainWindow->rend);

        // calculates to 60 fps
        SDL_Delay(FRAME_DELAY);
    }

    // free(mEvent.piece);
    free(mEvent.mousePos);
    free(mEvent.offset);
    free(mEvent.oldPos);
    free(mEvent.event);

    // mEvent.piece = NULL;
    mEvent.mousePos = NULL;
    mEvent.offset = NULL;
    mEvent.oldPos = NULL;
    mEvent.event = NULL;
}

void initialise() {
    // retutns zero on success else non-zero
    if (SDL_Init(SDL_INIT_VIDEO)) {
        // printf("error initializing SDL: %s\n", SDL_GetError());
        exit(0);
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    Window mainWindow = {.win = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED,
                                                 SDL_WINDOWPOS_CENTERED, WIDTH,
                                                 HEIGHT, 0)};

    mainWindow.rend =
        SDL_CreateRenderer(mainWindow.win, -1, SDL_RENDERER_ACCELERATED);

    Board *board = makeBoard(&mainWindow);

    makePieces(&mainWindow, board);

    gameLoop(&mainWindow, board);

    cleanUpPieces(board);
    cleanUpBoard(board);

    // destroy renderer
    SDL_DestroyRenderer(mainWindow.rend);

    // destroy window
    SDL_DestroyWindow(mainWindow.win);

    mainWindow.win = NULL;
    mainWindow.rend = NULL;

    IMG_Quit();
    // close SDL
    SDL_Quit();
}
