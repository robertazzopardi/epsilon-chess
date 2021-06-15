#ifndef __SVG_UTIL__
#define __SVG_UTIL__

typedef struct SDL_Texture SDL_Texture;
typedef struct SDL_Renderer SDL_Renderer;

typedef struct {
    SDL_Texture *black;
    SDL_Texture *white;
} TwoToneTexture;

TwoToneTexture *getTexture(SDL_Renderer *, const char *);

SDL_Texture *makeTexture(SDL_Renderer *, char *, long);

char *openFile(const char *, long *);

#endif
