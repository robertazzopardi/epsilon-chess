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
#define WHITE_TO_MOVE "White to move!"
#define BLACK_TO_MOVE "Black to move!"

#define PLAYER_TO_MOVE(toMove)                                                 \
    (toMove == PLAYER_2 ? WHITE_TO_MOVE : BLACK_TO_MOVE)

static void handleEvents(MouseEvent *event, Window *window,
                         SDL_Event *sdlEvent) {
    while (SDL_PollEvent(sdlEvent)) {
        switch (sdlEvent->type) {
        case SDL_QUIT:
            window->running = false;
            break;
        case SDL_MOUSEBUTTONUP:
            if (event->LMBDown && sdlEvent->button.button == SDL_BUTTON_LEFT) {

                // if (event->pieceSelected) {
                if (event->piece) {

                    if (canMovePiece(event) &&
                        window->board->toMove == event->piece->player) {

                        window->board->toMove =
                            window->board->toMove == PLAYER_1 ? PLAYER_2
                                                              : PLAYER_1;

                        SDL_SetWindowTitle(
                            window->win, PLAYER_TO_MOVE(window->board->toMove));

                        window->board->moveCount++;

                        // Align the pieces
                        alignPiece(event, window->board);

                        if (event->piece->firstMove)
                            event->piece->firstMove = false;

                        event->piece = NULL;

                        // printPieces(window->board);
                    } else {
                        event->piece->rect->x = event->oldPos->x;
                        event->piece->rect->y = event->oldPos->y;
                    }
                }
                event->LMBDown = false;
                // event->pieceSelected = false;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (!event->LMBDown && sdlEvent->button.button == SDL_BUTTON_LEFT) {
                event->LMBDown = true;

                event->piece = NULL;
                checkIfPiece(event, window->board->p1);
                checkIfPiece(event, window->board->p2);

                if (event->piece)
                    generateMoves(event, window->board);

                // Save old position
                // if (event->pieceSelected) {
                if (event->piece) {
                    event->oldPos->x = event->piece->rect->x;
                    event->oldPos->y = event->piece->rect->y;
                }

                // Toggle the colour of a square on the window->board
                SDL_Point point = {event->mousePos->x, event->mousePos->y};
                if (SDL_PointInRect(&point, window->board->selectedRect) ||
                    !window->board->selectedVisible) {
                    window->board->selectedVisible =
                        !window->board->selectedVisible;
                }

                window->board->selectedRect->x =
                    (event->mousePos->x / 100) * 100;
                window->board->selectedRect->y =
                    (event->mousePos->y / 100) * 100;
            }
            break;
        case SDL_MOUSEMOTION: {
            event->mousePos->x = sdlEvent->motion.x;
            event->mousePos->y = sdlEvent->motion.y;

            // if (event->LMBDown && event->pieceSelected) {
            if (event->LMBDown && event->piece) {
                window->board->selectedVisible = false;
                event->piece->rect->x = event->mousePos->x - event->offset->x;
                event->piece->rect->y = event->mousePos->y - event->offset->y;
            }
        } break;
        }
    }
}

static void gameLoop(Window *window) {

    MouseEvent event = {0};
    event.mousePos = malloc(1 * sizeof(*event.mousePos));
    event.offset = malloc(1 * sizeof(*event.offset));
    event.oldPos = malloc(1 * sizeof(*event.oldPos));
    // event.event = malloc(1 * sizeof(*event.event));

    SDL_Event sdlEvent;

    const int cellRadius = (WIDTH / ROW_COUNT) / 2;

    SDL_SetWindowTitle(window->win, PLAYER_TO_MOVE(window->board->toMove));

    // Main loop
    while (window->running) {
        handleEvents(&event, window, &sdlEvent);

        SDL_SetRenderDrawColor(window->rend, 0, 0, 0, 255);

        // Clear the screen
        SDL_RenderClear(window->rend);

        // Render board
        SDL_RenderCopy(window->rend, window->board->texture, NULL,
                       window->board->rect);

        // Render selected square
        if (window->board->selectedVisible) {
            SDL_SetRenderDrawColor(window->rend, 100, 100, 100, 100);
            SDL_RenderFillRect(window->rend, window->board->selectedRect);
        }

        // Render possible moves for selected piece
        if (event.piece && event.piece->moves->count > 0) {
            for (int i = 0; i < event.piece->moves->count; i++) {
                int mX = (event.piece->moves->squares[i].x * 100) + cellRadius;
                int mY = (event.piece->moves->squares[i].y * 100) + cellRadius;

                filledCircleRGBA(window->rend, mX, mY, 30, 100, 100, 100, 100);
            }
        }

        // Render the pieces
        drawPieces(window, &event);
        // Render selected piece on top
        if (event.piece) {
            SDL_RenderCopy(window->rend, event.piece->texture, NULL,
                           event.piece->rect);
        }

        // triggers the double buffers for multiple rendering
        SDL_RenderPresent(window->rend);

        // calculates to 60 fps
        SDL_Delay(FRAME_DELAY);
    }

    // free(event.piece);
    free(event.mousePos);
    free(event.offset);
    free(event.oldPos);
    // free(event.event);

    event.mousePos = NULL;
    event.offset = NULL;
    event.oldPos = NULL;
    // event.event = NULL;
}

void initialise() {
    // Init SDL
    if (SDL_Init(SDL_INIT_VIDEO))
        exit(0);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    Window window = {
        .win = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0),
        .running = true,
    };
    window.rend = SDL_CreateRenderer(window.win, -1, SDL_RENDERER_ACCELERATED);
    window.board = makeBoard(&window);

    makePieces(&window);

    gameLoop(&window);

    // Clean up
    cleanUpPieces(window.board);
    cleanUpBoard(window.board);

    // Destroy window and renderer
    SDL_DestroyRenderer(window.rend);
    SDL_DestroyWindow(window.win);

    window.win = NULL;
    window.rend = NULL;

    // Quit SDL
    IMG_Quit();
    SDL_Quit();
}
