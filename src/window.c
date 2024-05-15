#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_events.h>
#include <SDL_image.h>
#include <stdbool.h>

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

static void handle_events(SDL_Event *event, Window *window, State *game,
                          Piece *piece) {
    SDL_Point mouse_pos;
    SDL_Point offset;
    bool lmb_down = false;
    SDL_Point old_pos;

    while (SDL_PollEvent(event)) {
        switch (event->type) {
        case SDL_QUIT:
            window->running = false;
            break;
        case SDL_MOUSEBUTTONUP:
            if (lmb_down && event->button.button == SDL_BUTTON_LEFT) {

                if (piece != NULL) {

                    if (canMovePiece()) {

                        window->board->toMove =
                            window->board->toMove == PLAYER_1 ? PLAYER_2
                                                              : PLAYER_1;

                        SDL_SetWindowTitle(
                            window->win, PLAYER_TO_MOVE(window->board->toMove));

                        // Align the pieces
                        align_piece(window->board);

                        piece = NULL;

                        // generateMoves(window->board);
                        generate_moves(game);

                    } else {
                        piece->rect->x = old_pos.x;
                        piece->rect->y = old_pos.y;
                    }
                }
                lmb_down = false;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (!lmb_down && event->button.button == SDL_BUTTON_LEFT) {
                lmb_down = true;

                piece = NULL;
                check_if_piece(&mouse_pos, &offset, piece, game);

                // Save old position
                if (piece) {
                    old_pos.x = piece->rect->x;
                    old_pos.y = piece->rect->y;
                }

                // Toggle the colour of a square on the window->board
                SDL_Point point = {mouse_pos.x, mouse_pos.y};
                if (SDL_PointInRect(&point, window->board->selectedRect) ||
                    window->board->selected == EMPTY) {
                    window->board->selected =
                        get_square(mouse_pos.x, mouse_pos.y);
                }

                window->board->selectedRect->x =
                    (mouse_pos.x / SQUARE_SIZE) * SQUARE_SIZE;
                window->board->selectedRect->y =
                    (mouse_pos.y / SQUARE_SIZE) * SQUARE_SIZE;
            }
            break;
        case SDL_MOUSEMOTION: {
            mouse_pos.x = event->motion.x;
            mouse_pos.y = event->motion.y;

            if (lmb_down && piece) {
                window->board->selected = EMPTY;
                piece->rect->x = mouse_pos.x - offset.x;
                piece->rect->y = mouse_pos.y - offset.y;
            }
        } break;
        default:
            break;
        }
    }
}

static void game_loop(Window *window, State *game,
                      PieceTextureMap *texture_map) {
    SDL_Event event;

    // const int cellRadius = (WIDTH / ROW_COUNT) / 2;

    SDL_SetWindowTitle(window->win, PLAYER_TO_MOVE(window->board->toMove));

    // generateMoves(window->board);

    Piece *piece = NULL;

    // Main loop
    while (window->running) {
        handle_events(&event, window, game, piece);

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
        draw_pieces(window, game, texture_map);

        // Render selected piece on top
        if (piece != NULL) {
            SDL_RenderCopy(window->rend, piece->texture, NULL, piece->rect);
        }

        // triggers the double buffers for multiple rendering
        SDL_RenderPresent(window->rend);

        // calculates to 60 fps
        SDL_Delay(FRAME_DELAY);
    }

    if (piece != NULL) {
        free(piece);
        piece = NULL;
    }
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
