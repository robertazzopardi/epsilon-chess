#ifndef _PIECE_H_
#define _PIECE_H_

typedef struct SDL_Rect SDL_Rect;
typedef struct SDL_Texture SDL_Texture;
typedef struct SDL_Point SDL_Point;

typedef struct Piece Piece;
typedef struct Player Player;
typedef struct Board Board;
typedef struct Window Window;
typedef struct MouseEvent MouseEvent;

typedef struct {
    SDL_Point *p1;
    SDL_Point *p2;
    Player *player;
    SDL_Point *mousePos;
    Board *board;
    // Piece *piece;
    char *firstMove;
    char *initial;
} MoveConditions;

typedef struct Piece {
    char initial;
    SDL_Texture *texture;
    SDL_Rect *rect;
    char firstMove;
    char player;
    // char (*canMove)(SDL_Point *, SDL_Point *, Player *);
    char (*canMove)(MoveConditions *);

} Piece;

char canMove(MouseEvent *, Board *);

void makePieces(Window *, Board *);

void drawPieces(Window *, Board *);

void cleanUpPieces(Board *);

void checkIfPiece(MouseEvent *, Player *);

void alignPiece(MouseEvent *, Board *);

void checkIfFirstMove(MouseEvent *, Board *);

#endif
