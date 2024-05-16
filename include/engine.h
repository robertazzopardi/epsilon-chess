#ifndef ENGINE_H
#define ENGINE_H

#include <stdint.h>

#include "consts.h"

typedef struct {
    int from;
    int to;
    int flags;
    int promoted_piece;
} Move;

typedef uint64_t Bitboard;

typedef struct State {
    Bitboard bit_boards[PIECE_TYPE_COUNT];
    Bitboard all_pieces;
    Move moves[MAX_MOVES];
} State;

State new_state();

void generate_moves(State *);

typedef struct Location {
    int rank;
    int file;
} Location;

inline Location get_location(int sq) {
    // int file = sq & 7;
    // int rank = sq >> 3;
    // int rank = 7 - (sq >> 3);
    return (Location){7 - (sq >> 3), sq & 7};
}

inline int get_square(int rank, int file) {
    return 8 * rank + file;
}

#endif /* ENGINE_H */
