#include "svgutil.h"

// You must free the result if result is non-NULL.
char *replace(char *orig, char *rep, char *with) {
    char *result;   // the return string
    char *ins;      // the next insert point
    char *tmp;      // varies
    int len_rep;    // length of rep (the string to remove)
    int len_with;   // length of with (the string to replace rep with)
    int len_front;  // distance between rep and end of last rep
    int count;      // number of replacements

    // sanity checks and initialization
    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL;  // empty rep causes infinite loop during count
    if (!with)
        with = "";
    len_with = strlen(with);

    // count the number of replacements needed
    ins = orig;
    tmp = strstr(ins, rep);
    for (count = 0; tmp != NULL; ++count) {
        ins = tmp + len_rep;
        tmp = strstr(ins, rep);
    }

    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep;  // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}

char *openFile(const char *inputFilename, long *fsize) {
    FILE *f = fopen(inputFilename, "rb");
    fseek(f, 0, SEEK_END);
    *fsize = ftell(f);
    fseek(f, 0, SEEK_SET); /* same as rewind(f); */

    char *string = malloc(*fsize + 1);
    fread(string, 1, *fsize, f);

    fclose(f);

    string[*fsize] = 0;

    return string;
}

SDL_Texture *makeTexture(SDL_Renderer *renderer, char *string, long fsize) {
    SDL_RWops *rw = SDL_RWFromConstMem(string, fsize);
    SDL_Surface *surface = IMG_Load_RW(rw, 1);
    return SDL_CreateTextureFromSurface(renderer, surface);
}

TwoToneTexture *getTexture(SDL_Renderer *renderer, const char *inputFilename) {
    long fsize;
    char *string = openFile(inputFilename, &fsize);

    char *white;
    if (strstr(string, BLACK) != NULL) {
        white = replace(string, BLACK, WHITE);
    } else {
        white = malloc(fsize * sizeof(*white));
    }

    TwoToneTexture *textures = malloc(1 * sizeof(*textures));
    textures->black = makeTexture(renderer, string, fsize);
    textures->white = makeTexture(renderer, white, fsize);

    free(string);
    string = NULL;
    free(white);
    white = NULL;

    return textures;
}
