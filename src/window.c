#include "window.h"
#include "board.h"
#include "piece.h"
#include <SDL.h>
#include <SDL_events.h>
#include <SDL_image.h>

#define SQUARE_COUNT 64
#define TITLE "Chess"
#define FRAME_DELAY 1000 / 60

void handleEvents(char *running, MouseEvent *mEvent, Board *board) {
    while (SDL_PollEvent(mEvent->event)) {
        switch (mEvent->event->type) {
        case SDL_QUIT:
            *running = !running;
            break;
        case SDL_MOUSEBUTTONUP:
            if (mEvent->LMBDown && mEvent->event->button.button == SDL_BUTTON_LEFT) {
                if (mEvent->piece->rect != NULL) {
                    if (canMove(mEvent, board)) {
                        checkIfFirstMove(mEvent, board);
                        board->moveCount++;

                        alignPiece(mEvent);
                    } else {
                        mEvent->piece->rect->x = mEvent->oldPos->x;
                        mEvent->piece->rect->y = mEvent->oldPos->y;
                    }
                }
                mEvent->LMBDown = 0;
                mEvent->piece->rect = NULL;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (!mEvent->LMBDown && mEvent->event->button.button == SDL_BUTTON_LEFT) {
                mEvent->LMBDown = 1;
                checkIfPiece(mEvent, &board->p1);
                checkIfPiece(mEvent, &board->p2);

                // save old position
                if (mEvent->piece->rect) {
                    mEvent->oldPos->x = mEvent->piece->rect->x;
                    mEvent->oldPos->y = mEvent->piece->rect->y;
                }

                // toggleBoardSquare(mEvent->mousePos, &mEvent->square);
            }
            break;
        case SDL_MOUSEMOTION: {
            mEvent->mousePos->x = mEvent->event->motion.x;
            mEvent->mousePos->y = mEvent->event->motion.y;

            if (mEvent->LMBDown && mEvent->piece->rect != NULL) {
                mEvent->piece->rect->x = mEvent->mousePos->x - mEvent->offset->x;
                mEvent->piece->rect->y = mEvent->mousePos->y - mEvent->offset->y;
            }
        } break;
        }
    }
}

void gameLoop(Window *mainWindow, Board *board) {
    // controls annimation loop
    char running = 1;

    MouseEvent mEvent = {0};
    mEvent.piece = malloc(1 * sizeof(*mEvent.piece));
    mEvent.mousePos = malloc(1 * sizeof(*mEvent.mousePos));
    mEvent.offset = malloc(1 * sizeof(*mEvent.offset));
    mEvent.oldPos = malloc(1 * sizeof(*mEvent.oldPos));
    mEvent.event = malloc(1 * sizeof(*mEvent.event));

    // annimation loop
    while (running) {
        handleEvents(&running, &mEvent, board);

        SDL_SetRenderDrawColor(mainWindow->rend, 0, 0, 0, 255);

        // clears the screen
        SDL_RenderClear(mainWindow->rend);

        drawBoard(mainWindow, board);

        drawPieces(mainWindow, board);

        // triggers the double buffers for multiple rendering
        SDL_RenderPresent(mainWindow->rend);

        // calculates to 60 fps
        SDL_Delay(FRAME_DELAY);
    }

    free(mEvent.piece);
    free(mEvent.mousePos);
    free(mEvent.offset);
    free(mEvent.oldPos);
    free(mEvent.event);

    mEvent.piece = NULL;
    mEvent.mousePos = NULL;
    mEvent.offset = NULL;
    mEvent.oldPos = NULL;
    mEvent.oldPos = NULL;
}

void initialise() {
    // retutns zero on success else non-zero
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        // printf("error initializing SDL: %s\n", SDL_GetError());
        exit(0);
    }

    Window *mainWindow = malloc(1 * sizeof(*mainWindow));
    mainWindow->win = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

    mainWindow->rend = SDL_CreateRenderer(mainWindow->win, -1, SDL_RENDERER_ACCELERATED);

    Board *board = makeBoard(mainWindow);

    makePieces(mainWindow, board);

    gameLoop(mainWindow, board);

    cleanUpPieces(board);
    cleanUpBoard(board);

    // destroy renderer
    SDL_DestroyRenderer(mainWindow->rend);

    // destroy window
    SDL_DestroyWindow(mainWindow->win);

    mainWindow->win = NULL;
    mainWindow->rend = NULL;

    IMG_Quit();
    // close SDL
    SDL_Quit();
}
