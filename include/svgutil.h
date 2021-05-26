#ifndef __SVG_UTIL__
#define __SVG_UTIL__

#include <SDL.h>
#include <SDL_image.h>

#define BLACK "#000000"
#define WHITE "#FFFFFF"

typedef struct {
    SDL_Texture *black;
    SDL_Texture *white;
} TwoToneTexture;

TwoToneTexture *getTexture(SDL_Renderer *renderer, const char *inputFilename);

SDL_Texture *makeTexture(SDL_Renderer *renderer, char *string, long fsize);

char *openFile(const char *inputFilename, long *fsize);

#endif
