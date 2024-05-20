#ifndef SVG_UTIL_H
#define SVG_UTIL_H

typedef struct SDL_Texture SDL_Texture;
typedef struct SDL_Renderer SDL_Renderer;

typedef struct PieceTexture {
    SDL_Texture *black;
    SDL_Texture *white;
} PieceTexture;

PieceTexture *get_texture(SDL_Renderer *, const char *);

SDL_Texture *make_texture(SDL_Renderer *, char *, long);

char *open_file(const char *, long *);

#endif /* SVG_UTIL_H */
