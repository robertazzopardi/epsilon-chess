#include "window.h"

typedef struct
{
    SDL_Window *win;
    SDL_Renderer *rend;
} Window;

Window mainWindow;

void cleanUp()
{
    // destroy renderer
    SDL_DestroyRenderer(mainWindow.rend);

    // destroy window
    SDL_DestroyWindow(mainWindow.win);

    // close SDL
    SDL_Quit();
}

void initWindow()
{
    memset(&mainWindow.win, 0, sizeof mainWindow.win);
    memset(&mainWindow.rend, 0, sizeof mainWindow.rend);

    mainWindow.win = SDL_CreateWindow(TITLE, // creates a window
                                      SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED,
                                      WIDTH, HEIGHT, 0);

    mainWindow.rend = SDL_CreateRenderer(mainWindow.win, -1, SDL_RENDERER_ACCELERATED);

    // return mainWindow;
}

void handleInputs(bool *running)
{
    SDL_Event event;

    // Events mangement
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {

        case SDL_QUIT:
            // handling of close button
            *running = false;
            break;

        case SDL_KEYDOWN:
            // keyboard API for key pressed
            switch (event.key.keysym.scancode)
            {
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_UP:

                break;
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_LEFT:

                break;
            case SDL_SCANCODE_S:
            case SDL_SCANCODE_DOWN:

                break;
            case SDL_SCANCODE_D:
            case SDL_SCANCODE_RIGHT:

                break;
            default:
                break;
            }
        }
    }
}

void gameLoop(Board board)
{
    // // controls annimation loop
    bool running = true;

    // annimation loop
    while (running)
    {
        handleInputs(&running);

        SDL_SetRenderDrawColor(mainWindow.rend, 0, 0, 0, 255);

        // clears the screen
        SDL_RenderClear(mainWindow.rend);

        drawBoard(mainWindow.rend, &board);

        // triggers the double buffers
        // for multiple rendering
        SDL_RenderPresent(mainWindow.rend);

        // calculates to 60 fps
        SDL_Delay(FRAME_DELAY);
    }
}

void initialise(Board board)
{
    initWindow();

    // retutns zero on success else non-zero
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }

    gameLoop(board);

    cleanUp();
}
