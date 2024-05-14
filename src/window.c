#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_events.h>
#include <SDL_image.h>

#include "board.h"
#include "engine.h"
#include "piece.h"
#include "window.h"

#define TITLE "Chess"
#define FRAME_DELAY 1000.0f / 60.0f
#define WHITE_TO_MOVE "White to move!"
#define BLACK_TO_MOVE "Black to move!"

#define PLAYER_TO_MOVE(toMove)                                                 \
    (toMove == PLAYER_2 ? WHITE_TO_MOVE : BLACK_TO_MOVE)

static void handle_events(MouseEvent *event, Window *window,
                          SDL_Event *sdlEvent, State *game) {
    while (SDL_PollEvent(sdlEvent)) {
        switch (sdlEvent->type) {
        case SDL_QUIT:
            window->running = false;
            break;
        case SDL_MOUSEBUTTONUP:
            if (event->LMBDown && sdlEvent->button.button == SDL_BUTTON_LEFT) {

                if (event->piece) {

                    if (canMovePiece(event)) {

                        window->board->toMove =
                            window->board->toMove == PLAYER_1 ? PLAYER_2
                                                              : PLAYER_1;

                        SDL_SetWindowTitle(
                            window->win, PLAYER_TO_MOVE(window->board->toMove));

                        // Align the pieces
                        alignPiece(event, window->board);

                        event->piece = NULL;

                        // generateMoves(window->board);
                        generate_moves(game);

                    } else {
                        event->piece->rect->x = event->oldPos->x;
                        event->piece->rect->y = event->oldPos->y;
                    }
                }
                event->LMBDown = false;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (!event->LMBDown && sdlEvent->button.button == SDL_BUTTON_LEFT) {
                event->LMBDown = true;

                event->piece = NULL;
                check_if_piece(event, game);

                // Save old position
                if (event->piece) {
                    event->oldPos->x = event->piece->rect->x;
                    event->oldPos->y = event->piece->rect->y;
                }

                // Toggle the colour of a square on the window->board
                SDL_Point point = {event->mousePos->x, event->mousePos->y};
                if (SDL_PointInRect(&point, window->board->selectedRect) ||
                    window->board->selected == EMPTY) {
                    window->board->selected =
                        get_square(event->mousePos->x, event->mousePos->y);
                }

                window->board->selectedRect->x =
                    (event->mousePos->x / SQUARE_SIZE) * SQUARE_SIZE;
                window->board->selectedRect->y =
                    (event->mousePos->y / SQUARE_SIZE) * SQUARE_SIZE;
            }
            break;
        case SDL_MOUSEMOTION: {
            event->mousePos->x = sdlEvent->motion.x;
            event->mousePos->y = sdlEvent->motion.y;

            if (event->LMBDown && event->piece) {
                window->board->selected = EMPTY;
                event->piece->rect->x = event->mousePos->x - event->offset->x;
                event->piece->rect->y = event->mousePos->y - event->offset->y;
            }
        } break;
        default:
            break;
        }
    }
}

static void game_loop(Window *window, State *game,
                      PieceTextureMap *texture_map) {

    MouseEvent event = {0};
    event.mousePos = malloc(1 * sizeof(*event.mousePos));
    event.offset = malloc(1 * sizeof(*event.offset));
    event.oldPos = malloc(1 * sizeof(*event.oldPos));

    SDL_Event sdlEvent;

    // const int cellRadius = (WIDTH / ROW_COUNT) / 2;

    SDL_SetWindowTitle(window->win, PLAYER_TO_MOVE(window->board->toMove));

    // generateMoves(window->board);

    // Main loop
    while (window->running) {
        handle_events(&event, window, &sdlEvent, game);

        SDL_SetRenderDrawColor(window->rend, 0, 0, 0, 255);

        // Clear the screen
        SDL_RenderClear(window->rend);

        // Render board
        SDL_RenderCopy(window->rend, window->board->texture, NULL,
                       window->board->rect);

        // Render selected square
        if (window->board->selected != EMPTY) {
            SDL_SetRenderDrawColor(window->rend, SQUARE_SIZE, SQUARE_SIZE,
                                   SQUARE_SIZE, SQUARE_SIZE);
            SDL_RenderFillRect(window->rend, window->board->selectedRect);
        }

        // Render possible moves for selected piece
        // if (event.piece && event.piece->moves->count > 0) {
        //     for (int i = 0; i < event.piece->moves->count; i++) {
        //         int mX = (event.piece->moves->squares[i].x * SQUARE_SIZE) +
        //                  cellRadius;
        //         int mY = (event.piece->moves->squares[i].y * SQUARE_SIZE) +
        //                  cellRadius;
        //
        //         filledCircleRGBA(window->rend, mX, mY, 30, SQUARE_SIZE,
        //                          SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE);
        //     }
        // }

        // Render the pieces
        draw_pieces(window, &event, game, texture_map);

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

    free(event.mousePos);
    event.mousePos = NULL;
    free(event.offset);
    event.offset = NULL;
    free(event.oldPos);
    event.oldPos = NULL;
}

static void cleanup(Window *window) {
    // cleanUpPieces(window->board);
    cleanUpBoard(window->board);

    SDL_DestroyRenderer(window->rend);
    window->rend = NULL;
    SDL_DestroyWindow(window->win);
    window->win = NULL;

    IMG_Quit();
    SDL_Quit();
}

void initialise() {
    if (SDL_Init(SDL_INIT_VIDEO)) {
        exit(0);
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    Window window = {
        .win = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0),
        .running = true,
    };
    window.rend = SDL_CreateRenderer(
        window.win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    window.board = makeBoard(window.rend);

    // makePieces(&window);

    State game = new_state();

    PieceTextureMap texture_map = new_texture_map(window.rend);

    game_loop(&window, &game, &texture_map);

    clean_up_texture_map(&texture_map);
    cleanup(&window);
}
