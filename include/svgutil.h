#ifndef __SVG_UTIL__
#define __SVG_UTIL__

#include <SDL.h>

#define BLACK "#000000"
#define WHITE "#FFFFFF"

typedef struct {
    SDL_Texture *black;
    SDL_Texture *white;
} TwoToneTexture;

TwoToneTexture *getTexture(SDL_Renderer *, const char *);

SDL_Texture *makeTexture(SDL_Renderer *, char *, long);

char *openFile(const char *, long *);

#endif
