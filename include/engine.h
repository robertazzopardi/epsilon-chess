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
    Bitboard bit_boards[NUM_PIECE_BIT_BOARDS];
    Bitboard all_pieces;
    Move moves[MAX_MOVES];
} State;

State new_state();

void generate_moves(State *);

#endif /* ENGINE_H */
