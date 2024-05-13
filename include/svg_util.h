#ifndef SVG_UTIL_H
#define SVG_UTIL_H

typedef struct SDL_Texture SDL_Texture;
typedef struct SDL_Renderer SDL_Renderer;

typedef struct {
    SDL_Texture *black;
    SDL_Texture *white;
} TwoToneTexture;

TwoToneTexture *getTexture(SDL_Renderer *, const char *);

SDL_Texture *makeTexture(SDL_Renderer *, char *, long);

char *openFile(const char *, long *);

#endif /* SVG_UTIL_H */
