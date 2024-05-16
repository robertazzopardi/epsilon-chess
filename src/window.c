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

typedef struct {
    SDL_Point mouse_pos;
    SDL_Point offset;
    SDL_Point old_pos;
    SDL_Event event;
} Event;

static void handle_events(Window *window, State *game, Piece *piece,
                          SDL_Rect *selected_square, Piece *pieces,
                          Event *event) {
    // printf("%s\n", piece == NULL ? "true" : "false");
    while (SDL_PollEvent(&event->event)) {
        switch (event->event.type) {
        case SDL_QUIT:
            window->running = false;
            break;
        case SDL_MOUSEBUTTONUP:
            if (event->event.button.button == SDL_BUTTON_LEFT) {

                if (piece != NULL) {

                    if (canMovePiece()) {
                        window->board->toMove =
                            window->board->toMove == PLAYER_1 ? PLAYER_2
                                                              : PLAYER_1;

                        SDL_SetWindowTitle(
                            window->win, PLAYER_TO_MOVE(window->board->toMove));

                        align_piece(window->board);

                        // piece = NULL;

                        generate_moves(game);
                    } else {
                        piece->rect.x = event->old_pos.x;
                        piece->rect.y = event->old_pos.y;
                    }
                }
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (event->event.button.button == SDL_BUTTON_LEFT) {

                // piece = NULL;
                Piece *tmp_piece = check_if_piece(&event->mouse_pos,
                                                  &event->offset, game, pieces);
                if (tmp_piece) {
                    piece = tmp_piece;
                }

                // Save old position
                if (piece) {
                    event->old_pos.x = piece->rect.x;
                    event->old_pos.y = piece->rect.y;
                }

            } else if (event->event.button.button == SDL_BUTTON_RIGHT) {
                // Toggle the colour of a square on the window->board
                if (window->board->selected == EMPTY) {
                    if (SDL_PointInRect(&event->mouse_pos, selected_square) ||
                        window->board->selected == EMPTY) {
                        window->board->selected =
                            get_square(event->mouse_pos.x, event->mouse_pos.y);
                    }
                } else {
                    window->board->selected = EMPTY;
                }

                selected_square->x =
                    (event->mouse_pos.x / SQUARE_SIZE) * SQUARE_SIZE;
                selected_square->y =
                    (event->mouse_pos.y / SQUARE_SIZE) * SQUARE_SIZE;
            }
            break;
        case SDL_MOUSEMOTION: {
            event->mouse_pos.x = event->event.motion.x;
            event->mouse_pos.y = event->event.motion.y;

            // printf("%s %s\n", piece != NULL ? "true" : "false",
            //        event->lmb_down ? "true" : "false");
            if (event->event.button.button == SDL_BUTTON_LEFT && piece) {
                // printf("here\n");
                window->board->selected = EMPTY;
                piece->rect.x = event->mouse_pos.x - event->offset.x;
                piece->rect.y = event->mouse_pos.y - event->offset.y;
            }
        } break;
        default:
            break;
        }
    }
}

static void game_loop(Window *window, State *game,
                      PieceTextureMap *texture_map) {
    // const int cellRadius = (WIDTH / ROW_COUNT) / 2;

    SDL_SetWindowTitle(window->win, PLAYER_TO_MOVE(window->board->toMove));

    Piece *piece = NULL;

    SDL_Rect selected_square = {
        .w = WIDTH / ROW_COUNT,
        .h = WIDTH / ROW_COUNT,
        .x = 0,
        .y = 0,
    };

    Piece pieces[BOARD_SIZE];
    make_pieces(pieces, texture_map, game);

    Event event = {0};

    while (window->running) {
        handle_events(window, game, piece, &selected_square, pieces, &event);

        SDL_SetRenderDrawColor(window->rend, 0, 0, 0, 255);

        SDL_RenderClear(window->rend);

        SDL_RenderCopy(window->rend, window->board->texture, NULL,
                       window->board->rect);

        // Render selected square
        if (window->board->selected != EMPTY) {
            SDL_SetRenderDrawColor(window->rend, SQUARE_SIZE, SQUARE_SIZE,
                                   SQUARE_SIZE, SQUARE_SIZE);
            SDL_RenderFillRect(window->rend, &selected_square);
        }

        // Render possible moves for selected piece
        // if (event.piece && event.piece->moves->count > 0) {
        //     for (int i = 0; i < event.piece->moves->count; i++) {
        //         int mX = (event.piece->moves->squares[i].x * SQUARE_SIZE)
        //         +
        //                  cellRadius;
        //         int mY = (event.piece->moves->squares[i].y * SQUARE_SIZE)
        //         +
        //                  cellRadius;
        //
        //         filledCircleRGBA(window->rend, mX, mY, 30, SQUARE_SIZE,
        //                          SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE);
        //     }
        // }

        // Render the pieces
        draw_pieces(window, game, pieces);

        // Render selected piece on top
        if (piece != NULL) {
            SDL_RenderCopy(window->rend, piece->texture, NULL, &piece->rect);
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
    clean_up_board(window->board);

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
    window.board = make_board(window.rend);

    State game = new_state();

    PieceTextureMap texture_map = new_texture_map(window.rend);

    game_loop(&window, &game, &texture_map);

    clean_up_texture_map(&texture_map);
    cleanup(&window);
}
