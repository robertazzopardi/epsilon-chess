#include <stdbool.h>
#include <stdio.h>

#include "consts.h"
#include "engine.h"

typedef enum { ROOK, BISHOP, QUEEN } Piece;

typedef enum { WHITE, BLACK } Colour;

typedef enum {
    NORTH,
    EAST,
    SOUTH,
    WEST,
    NORTH_WEST,
    NORTH_EAST,
    SOUTH_WEST,
    SOUTH_EAST
} Direction;

static const Direction directions[4] = {
    NORTH,
    EAST,
    SOUTH,
    WEST,
};

static const Direction diagonals[4] = {
    NORTH_WEST,
    NORTH_EAST,
    SOUTH_WEST,
    SOUTH_EAST,
};

typedef struct {
    Bitboard lower;
    Bitboard upper;
    Bitboard line_ex; // lower | upper
} SMasks;             // 6 (4) KByte, needs initialization

static SMasks masks[BOARD_SIZE][4];

#define FILE_OF(S) ((S) & 7)
#define GET_LOWER(S) ((1ULL << S) - 1)
#define GET_UPPER(S) (0xFFFFFFFFFFFFFFFFULL << (S))

inline void bit_clear(Bitboard *number, Bitboard n) {
    *number &= ~(1ULL << n);
}

inline void bit_set(Bitboard *number, Bitboard n) {
    *number |= (1ULL << n);
}

// Print the board represented by the given bitboard
void print_board(Bitboard board) {
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            if (board & (1ULL << square)) {
                printf("1 ");
            } else {
                printf("0 ");
            }
        }
        printf("\n");
    }
    printf("\n");
}

static const Bitboard pieces[] = {
    WHITE_PAWN, WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK, WHITE_QUEEN, WHITE_KING,
    BLACK_PAWN, BLACK_KNIGHT, BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN, BLACK_KING,
};

const Bitboard notAFile = 0xfefefefefefefefeULL;
const Bitboard notHFile = 0x7f7f7f7f7f7f7f7fULL;
const Bitboard notABFile = 0xfcfcfcfcfcfcfcfcULL;
const Bitboard notGHFile = 0x3f3f3f3f3f3f3f3fULL;

inline Bitboard sout_one(Bitboard b) {
    return b >> 8;
}

inline Bitboard nort_one(Bitboard b) {
    return b << 8;
}

inline Bitboard east_one(Bitboard b) {
    return (b << 1) & notAFile;
}

inline Bitboard no_ea_one(Bitboard b) {
    return (b << 9) & notAFile;
}

inline Bitboard so_ea_one(Bitboard b) {
    return (b >> 7) & notAFile;
}

inline Bitboard west_one(Bitboard b) {
    return (b >> 1) & notHFile;
}

inline Bitboard so_we_one(Bitboard b) {
    return (b >> 9) & notHFile;
}

inline Bitboard no_we_one(Bitboard b) {
    return (b << 7) & notHFile;
}

void generate_pawn_moves(State *game, size_t *num_moves) {
    Bitboard white_pawns = game->bit_boards[0];
    Bitboard black_pawns = game->bit_boards[6];
    Bitboard all_pieces = game->all_pieces;
    Move *moves = game->moves;

    // Single-step advances for white pawns
    Bitboard single_step_moves_white = (white_pawns << 8) & ~all_pieces;
    while (single_step_moves_white) {
        int to = __builtin_ctzll(single_step_moves_white);
        moves[(*num_moves)++] = (Move){to - 8, to, QUIET};
        single_step_moves_white &= single_step_moves_white - 1;
    }

    // Double-step advances from starting position for white pawns
    Bitboard double_step_moves_white =
        ((single_step_moves_white & (1ULL << 16)) << 8) & ~all_pieces;
    while (double_step_moves_white) {
        int to = __builtin_ctzll(double_step_moves_white);
        moves[(*num_moves)++] = (Move){to - 16, to, QUIET};
        double_step_moves_white &= double_step_moves_white - 1;
    }

    // Capturing moves diagonally for white pawns
    Bitboard captures_white = ((white_pawns & ~A_FILE) << 7) & ~H_FILE;
    captures_white |= ((white_pawns & ~H_FILE) << 9) & ~A_FILE;
    while (captures_white) {
        int to = __builtin_ctzll(captures_white);
        int from = to - 9;
        if (!(all_pieces & (1ULL << from))) {
            from = to - 7;
        }
        moves[(*num_moves)++] = (Move){from, to, CAPTURE};
        captures_white &= captures_white - 1;
    }

    // Single-step advances for black pawns
    Bitboard single_step_moves_black = (black_pawns >> 8) & ~all_pieces;
    while (single_step_moves_black) {
        int to = __builtin_ctzll(single_step_moves_black);
        moves[(*num_moves)++] = (Move){to + 8, to, QUIET};
        single_step_moves_black &= single_step_moves_black - 1;
    }

    // Double-step advances from starting position for black pawns
    Bitboard double_step_moves_black =
        ((single_step_moves_black & (1ULL << 48)) >> 8) & ~all_pieces;
    while (double_step_moves_black) {
        int to = __builtin_ctzll(double_step_moves_black);
        moves[(*num_moves)++] = (Move){to + 16, to, QUIET};
        double_step_moves_black &= double_step_moves_black - 1;
    }

    // Capturing moves diagonally for black pawns
    Bitboard captures_black = ((black_pawns & ~H_FILE) >> 7) & ~A_FILE;
    captures_black |= ((black_pawns & ~A_FILE) >> 9) & ~H_FILE;
    while (captures_black) {
        int to = __builtin_ctzll(captures_black);
        int from = to + 9;
        if (!(all_pieces & (1ULL << from))) {
            from = to + 7;
        }
        moves[(*num_moves)++] = (Move){from, to, CAPTURE};
        captures_black &= captures_black - 1;
    }
}

Bitboard knight_attacks(Bitboard knights) {
    Bitboard l1 = (knights >> 1) & notHFile;
    Bitboard l2 = (knights >> 2) & notGHFile;
    Bitboard r1 = (knights << 1) & notAFile;
    Bitboard r2 = (knights << 2) & notABFile;
    Bitboard h1 = l1 | r1;
    Bitboard h2 = l2 | r2;
    return (h1 << 16) | (h1 >> 16) | (h2 << 8) | (h2 >> 8);
}

Bitboard king_attacks(Bitboard kingSet) {
    Bitboard attacks = east_one(kingSet) | west_one(kingSet);
    kingSet |= attacks;
    attacks |= nort_one(kingSet) | sout_one(kingSet);
    return attacks;
}

void add_moves(Bitboard piece_moves, Move *moves, size_t *num_moves) {
    for (int square = 0; square < BOARD_SIZE; square++) {
        if (piece_moves & (1ULL << square)) {
            moves[(*num_moves)++] = (Move){-1, square, QUIET};
        }
    }
}

void generate_king_moves(State *game, size_t *num_moves, Colour colour) {
    Bitboard kings = game->bit_boards[colour == WHITE ? 5 : 11];
    Bitboard potential_moves = king_attacks(kings);
    Bitboard occupied_squares = potential_moves & game->all_pieces;
    Bitboard king_moves = potential_moves & ~occupied_squares;
    add_moves(king_moves, game->moves, num_moves);
}

void generate_knight_moves(State *game, size_t *num_moves, Colour colour) {
    Bitboard knights = game->bit_boards[colour == WHITE ? 1 : 7];
    Bitboard potential_moves = knight_attacks(knights);
    Bitboard occupied_squares = potential_moves & game->all_pieces;
    Bitboard knight_moves = potential_moves & ~occupied_squares;
    add_moves(knight_moves, game->moves, num_moves);
}

inline Bitboard init_rank(int s) {
    return 0xFFULL << (s & 56);
}

inline Bitboard init_file(int s) {
    return A_FILE << FILE_OF(s);
}

Bitboard init_diag(int sq) {
    int d = 8 * FILE_OF(sq) - (sq & 56);
    int n = -d & (d >> 31);
    int s = d & (-d >> 31);
    return (0x8040201008040201ULL >> s) << n;
}

Bitboard init_anti(int sq) {
    int d = 56 - 8 * FILE_OF(sq) - (sq & 56);
    int n = -d & (d >> 31);
    int s = d & (-d >> 31);
    return (0x0102040810204080ULL >> s) << n;
}

inline Bitboard init_low(int sq, uint64_t line) {
    return GET_LOWER(sq) & (line ^ (1ULL << sq));
}

inline Bitboard init_up(int sq, uint64_t line) {
    return GET_UPPER(sq) & (line ^ (1ULL << sq));
}

SMasks new_mask(int sq, Bitboard line) {
    SMasks masks;
    masks.lower = init_low(sq, line);
    masks.upper = init_up(sq, line);
    masks.line_ex = init_low(sq, line) | init_up(sq, line);
    return masks;
}

void init_masks() {
    for (int sq = 0; sq < BOARD_SIZE; sq++) {
        masks[sq][0] = new_mask(sq, init_rank(sq));
        masks[sq][1] = new_mask(sq, init_file(sq));
        masks[sq][2] = new_mask(sq, init_diag(sq));
        masks[sq][3] = new_mask(sq, init_anti(sq));
    }
}

/*
 * @author Michael Hoffmann, Daniel Infuehr
 * @param  Bitboard occ occupancy of the board
 * @param  SMasks* a mask structure by square and line
 * @return line attacks from that square
 */
Bitboard line_attacks(Bitboard lower, Bitboard upper, Bitboard mask) {
    Bitboard ms1b = 0x8000000000000000ULL >> __builtin_clzl(lower | 1);
    return mask & (upper - ms1b);
}

void generate_attacks_by_ray(int square, Direction direction, Bitboard occ,
                             Bitboard *lines) {
    int current_square = square;
    bool found_obstruction = false;

    for (int i = 0; i < 8; i++) {
        switch (direction) {
        case NORTH:
            current_square += 8;
            break;
        case EAST:
            current_square += 1;
            break;
        case SOUTH:
            current_square -= 8;
            break;
        case WEST:
            current_square -= 1;
            break;
        case NORTH_WEST:
            current_square += 7;
            break;
        case NORTH_EAST:
            current_square += 9;
            break;
        case SOUTH_WEST:
            current_square -= 9;
            break;
        case SOUTH_EAST:
            current_square -= 7;
            break;
        }

        if (current_square < 0 || current_square >= BOARD_SIZE) {
            break;
        }

        if (occ & (1ULL << current_square)) {
            found_obstruction = true;
        }

        if (found_obstruction) {
            bit_clear(lines, current_square);
        }
    }
}

Bitboard sliding_attacks(Bitboard pieces, Bitboard occ, int sq, Piece piece) {
    SMasks *r = masks[sq];

    Bitboard lines = EMPTY_BOARD;

    if (piece == ROOK) {
        lines = line_attacks(r[0].lower & pieces, r[0].upper & pieces,
                             r[0].line_ex) |
                line_attacks(r[1].lower & pieces, r[1].upper & pieces,
                             r[1].line_ex);
    } else if (piece == BISHOP) {
        lines = line_attacks(r[2].lower & pieces, r[2].upper & pieces,
                             r[2].line_ex) |
                line_attacks(r[3].lower & pieces, r[3].upper & pieces,
                             r[3].line_ex);
    }

    if (lines == EMPTY_BOARD) {
        return lines;
    }

    for (int i = 0; i < 4; i++) {
        if (piece == ROOK) {
            generate_attacks_by_ray(sq, directions[i], occ, &lines);
        } else {
            generate_attacks_by_ray(sq, diagonals[i], occ, &lines);
        }
    }

    return lines;
}

Bitboard queen_attacks(Bitboard pieces, Bitboard occ, int sq) {
    return sliding_attacks(pieces, occ, sq, ROOK) |
           sliding_attacks(pieces, occ, sq, BISHOP);
}

void generate_sliding_moves(State *game, Bitboard pieces, size_t *num_moves,
                            Piece piece) {
    for (int square = 0; square < BOARD_SIZE; square++) {
        if (pieces & (1ULL << square)) {
            Bitboard piece_moves = 0;
            if (piece == ROOK || piece == BISHOP) {
                piece_moves =
                    sliding_attacks(pieces, game->all_pieces, square, piece);
            } else if (piece == QUEEN) {
                piece_moves = queen_attacks(pieces, game->all_pieces, square);
            }

            if (piece_moves & (1ULL << square)) {
                game->moves[*num_moves].from = square;
                game->moves[*num_moves].to = square;
                (*num_moves)++;
            }
        }
    }
}

void generate_moves(State *game) {
    size_t num_moves = 0;

    // Single move pieces
    generate_pawn_moves(game, &num_moves);
    generate_knight_moves(game, &num_moves, WHITE);
    generate_knight_moves(game, &num_moves, BLACK);
    generate_king_moves(game, &num_moves, WHITE);
    generate_king_moves(game, &num_moves, BLACK);

    // Sliding pieces
    generate_sliding_moves(game, game->bit_boards[3], &num_moves, ROOK);
    generate_sliding_moves(game, game->bit_boards[9], &num_moves, ROOK);
    generate_sliding_moves(game, game->bit_boards[2], &num_moves, BISHOP);
    generate_sliding_moves(game, game->bit_boards[8], &num_moves, BISHOP);
    generate_sliding_moves(game, game->bit_boards[4], &num_moves, QUEEN);
    generate_sliding_moves(game, game->bit_boards[10], &num_moves, QUEEN);

    // printf("Number of moves: %lu\n", num_moves);
    // for (size_t i = 0; i < num_moves; i++) {
    //     printf("From: %d, To: %d\n", game->moves[i].from, game->moves[i].to);
    // }
}

void move_piece() {
}

State new_state() {
    State game;

    game.all_pieces = EMPTY_BOARD;
    for (size_t i = 0; i < PIECE_TYPE_COUNT; i++) {
        game.bit_boards[i] = EMPTY_BOARD | pieces[i];
        game.all_pieces |= game.bit_boards[i];
    }

    init_masks();

    return game;
}

// int main(void) {
//     State game;
//
//     game.all_pieces = EMPTY_BOARD;
//     for (size_t i = 0; i < PIECE_TYPE_COUNT; i++) {
//         game.bit_boards[i] = EMPTY_BOARD | pieces[i];
//         game.all_pieces |= game.bit_boards[i];
//     }
//
//     print_board(game.all_pieces);
//
//     init_masks();
//
//     generate_moves(&game);
//
//     return 0;
// }
