#ifndef CONSTANTS_H
#define CONSTANTS_H

// Define constants for the chess board
#define BOARD_SIZE 64
#define RANKS 8
#define FILES 8
#define EMPTY_BOARD 0ULL

#define PIECE_TYPE_COUNT 12

#define MAX_MOVES 300

// clang-format off
typedef enum Square {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    EMPTY
} Square;
// clang-format on

#define A_FILE 0x0101010101010101ULL
#define B_FILE 0x0202020202020202ULL
#define C_FILE 0x0404040404040404ULL
#define D_FILE 0x0808080808080808ULL
#define E_FILE 0x1010101010101010ULL
#define F_FILE 0x2020202020202020ULL
#define G_FILE 0x4040404040404040ULL
#define H_FILE 0x8080808080808080ULL

#define RANK_1 0x00000000000000FFULL
#define RANK_2 0x000000000000FF00ULL
#define RANK_3 0x0000000000FF0000ULL
#define RANK_4 0x00000000FF000000ULL
#define RANK_5 0x000000FF00000000ULL
#define RANK_6 0x0000FF0000000000ULL
#define RANK_7 0x00FF000000000000ULL
#define RANK_8 0xFF00000000000000ULL

#define DIAG_MASK 0x8040201008040201ULL
#define ANTI_DIAG_MASK 0x0102040810204080ULL

#define WHITE_PAWN                                                             \
    (1ULL << A2) | (1ULL << B2) | (1ULL << C2) | (1ULL << D2) | (1ULL << E2) | \
        (1ULL << F2) | (1ULL << G2) | (1ULL << H2)
#define WHITE_KNIGHT (1ULL << B1) | (1ULL << G1)
#define WHITE_BISHOP (1ULL << C1) | (1ULL << F1)
#define WHITE_ROOK (1ULL << A1) | (1ULL << H1)
#define WHITE_QUEEN (1ULL << D1)
#define WHITE_KING (1ULL << E1)

#define BLACK_PAWN                                                             \
    (1ULL << A7) | (1ULL << B7) | (1ULL << C7) | (1ULL << D7) | (1ULL << E7) | \
        (1ULL << F7) | (1ULL << G7) | (1ULL << H7)
#define BLACK_KNIGHT (1ULL << B8) | (1ULL << G8)
#define BLACK_BISHOP (1ULL << C8) | (1ULL << F8)
#define BLACK_ROOK (1ULL << A8) | (1ULL << H8)
#define BLACK_QUEEN (1ULL << D8)
#define BLACK_KING (1ULL << E8)

// Define special move flags
#define NORMAL_MOVE 0
#define DOUBLE_PAWN_PUSH 1
#define QUEEN_SIDE_CASTLE 2
#define KING_SIDE_CASTLE 3
#define EN_PASSANT 4
#define PROMOTION 5

#endif /* CONSTANTS_H */
