#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_events.h>
#include <SDL_image.h>
#include <stdbool.h>

#include "board.h"
#include "engine.h"
#include "piece.h"
#include "window.h"

#define FRAME_DELAY 1000.0f / 60.0f

static const int cell_radius = SQUARE_SIZE / 2;

typedef struct {
    SDL_Point mouse_pos;
    SDL_Point offset;
    SDL_Point old_pos;
    SDL_Event event;
} Event;

static void handle_events(Window *window, State *game, Piece **piece,
                          SDL_Rect *selected_square, Piece *pieces,
                          Event *event, bool *running) {
    while (SDL_PollEvent(&event->event)) {
        switch (event->event.type) {
        case SDL_QUIT:
            *running = false;
            break;
        case SDL_MOUSEBUTTONUP:
            if (event->event.button.button == SDL_BUTTON_LEFT &&
                piece != NULL) {
                if (can_move(game, &event->mouse_pos, &event->old_pos)) {
                    // SDL_SetWindowTitle(
                    //     window->win,
                    //     PLAYER_TO_MOVE(window->board->toMove));

                    (*piece)->rect.x =
                        (event->mouse_pos.x / SQUARE_SIZE) * SQUARE_SIZE;
                    (*piece)->rect.y =
                        (event->mouse_pos.y / SQUARE_SIZE) * SQUARE_SIZE;

                    *piece = NULL;
                    generate_moves(game);
                } else if (*piece != NULL) {
                    (*piece)->rect.x = event->old_pos.x;
                    (*piece)->rect.y = event->old_pos.y;
                }
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (event->event.button.button == SDL_BUTTON_LEFT) {
                check_if_piece(&event->mouse_pos, &event->offset, game, pieces,
                               &*piece);

                if (*piece != NULL) {
                    event->old_pos.x = (*piece)->rect.x;
                    event->old_pos.y = (*piece)->rect.y;
                }
            } else if (event->event.button.button == SDL_BUTTON_RIGHT) {
                if (window->board->selected_square == EMPTY) {
                    if (SDL_PointInRect(&event->mouse_pos, selected_square) ||
                        window->board->selected_square == EMPTY) {
                        window->board->selected_square =
                            get_square(event->mouse_pos.x, event->mouse_pos.y);
                    }
                } else {
                    window->board->selected_square = EMPTY;
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

            if (event->event.button.button == SDL_BUTTON_LEFT &&
                *piece != NULL) {
                window->board->selected_square = EMPTY;
                (*piece)->rect.x = event->mouse_pos.x - event->offset.x;
                (*piece)->rect.y = event->mouse_pos.y - event->offset.y;
            }
        } break;
        default:
            break;
        }
    }
}

static void game_loop(Window *window, State *game,
                      PieceTextureMap *texture_map) {
    // SDL_SetWindowTitle(window->win, PLAYER_TO_MOVE(window->board->toMove));

    SDL_Rect selected_square = {
        .w = WIDTH / ROW_COUNT,
        .h = WIDTH / ROW_COUNT,
        .x = 0,
        .y = 0,
    };

    Piece *piece = NULL;
    Piece pieces[BOARD_SIZE];
    make_pieces(pieces, texture_map, game);

    Event event = {0};

    bool running = true;

    while (running) {
        handle_events(window, game, &piece, &selected_square, pieces, &event,
                      &running);

        SDL_SetRenderDrawColor(window->rend, 0, 0, 0, 255);

        SDL_RenderClear(window->rend);

        SDL_RenderCopy(window->rend, window->board->texture, NULL,
                       window->board->rect);

        if (window->board->selected_square != EMPTY) {
            SDL_SetRenderDrawColor(window->rend, SQUARE_SIZE, SQUARE_SIZE,
                                   SQUARE_SIZE, SQUARE_SIZE);
            SDL_RenderFillRect(window->rend, &selected_square);
        }

        // Render possible moves for selected piece
        if (piece != NULL) {
            for (Square sq = 0; sq < MAX_MOVES; sq++) {
                Square from_sq = get_square(piece->rect.y / SQUARE_SIZE,
                                            piece->rect.x / SQUARE_SIZE);

                if (from_sq == game->moves[sq].from) {
                    int to = game->moves[sq].to;
                    int x = (to & 7) * SQUARE_SIZE + cell_radius;
                    int y = (to >> 3) * SQUARE_SIZE + cell_radius;

                    filledCircleRGBA(window->rend, x, y, 30, 100, 100, 100,
                                     100);
                }
            }
        }

        draw_pieces(window, game, pieces);

        // Render selected piece on top
        if (piece != NULL) {
            SDL_RenderCopy(window->rend, piece->texture, NULL, &piece->rect);
        }

        SDL_RenderPresent(window->rend);

        SDL_Delay(FRAME_DELAY);
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
        .win = SDL_CreateWindow("Chess", SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0),
        .rend = SDL_CreateRenderer(window.win, -1,
                                   SDL_RENDERER_ACCELERATED |
                                       SDL_RENDERER_PRESENTVSYNC),
        .board = make_board(window.rend),
    };

    State game = new_state();

    generate_moves(&game);

    PieceTextureMap texture_map = new_texture_map(window.rend);

    game_loop(&window, &game, &texture_map);

    clean_up_texture_map(&texture_map);
    cleanup(&window);
}
