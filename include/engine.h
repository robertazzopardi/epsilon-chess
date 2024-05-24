#ifndef ENGINE_H
#define ENGINE_H

#include <stdint.h>

#include "consts.h"

typedef enum {
    PAWN,
    ROOK,
    KNIGHT,
    BISHOP,
    QUEEN,
    KING,
    NONE,
} Piece;

typedef enum {
    WHITE,
    BLACK,
} Colour;

typedef enum {
    QUIET,
    CAPTURE,
    EVASION,
    ENPASSANT,
    CASTLING,
} MoveType;

typedef struct {
    Square from;
    Square to;
    MoveType type;
    Piece piece;
} Move;

typedef uint64_t Bitboard;

typedef struct State {
    Bitboard bit_boards[PIECE_TYPE_COUNT];
    Bitboard all_pieces;
    Move moves[MAX_MOVES];
} State;

typedef struct Location {
    int rank;
    int file;
} Location;

State new_state();

void generate_moves(State *);

void move_piece(State *, Move *);

inline Location get_location(int sq) {
    return (Location){sq >> 3, sq & 7};
}

inline int get_square(int rank, int file) {
    return 8 * rank + file;
}

#endif /* ENGINE_H */
