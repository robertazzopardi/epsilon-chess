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

// Define bit masks for each file on the board
#define A_FILE 0x0101010101010101ULL // Represents the A file (leftmost column)
#define B_FILE 0x0202020202020202ULL // Represents the B file
#define C_FILE 0x0404040404040404ULL // Represents the C file
#define D_FILE 0x0808080808080808ULL // Represents the D file
#define E_FILE 0x1010101010101010ULL // Represents the E file
#define F_FILE 0x2020202020202020ULL // Represents the F file
#define G_FILE 0x4040404040404040ULL // Represents the G file
#define H_FILE 0x8080808080808080ULL // Represents the H file (rightmost column)

// Define bit masks for each rank on the board
#define RANK_1 0x00000000000000FFULL // Represents the first rank (bottom row)
#define RANK_2 0x000000000000FF00ULL // Represents the second rank
#define RANK_3 0x0000000000FF0000ULL // Represents the third rank
#define RANK_4 0x00000000FF000000ULL // Represents the fourth rank
#define RANK_5 0x000000FF00000000ULL // Represents the fifth rank
#define RANK_6 0x0000FF0000000000ULL // Represents the sixth rank
#define RANK_7 0x00FF000000000000ULL // Represents the seventh rank
#define RANK_8 0xFF00000000000000ULL // Represents the eighth rank (top row)

// Define bit masks for each piece type
#define WHITE_PAWN                                                             \
    (1ULL << 8) | (1ULL << 9) | (1ULL << 10) | (1ULL << 11) | (1ULL << 12) |   \
        (1ULL << 13) | (1ULL << 14) |                                          \
        (1ULL << 15)                           // White pawns on second rank
#define WHITE_KNIGHT (1ULL << 1) | (1ULL << 6) // White knights on B1 and G1
#define WHITE_BISHOP (1ULL << 2) | (1ULL << 5) // White bishops on C1 and F1
#define WHITE_ROOK (1ULL << 0) | (1ULL << 7)   // White rooks on A1 and H1
#define WHITE_QUEEN (1ULL << 3)                // White queen on D1
#define WHITE_KING (1ULL << 4)                 // White king on E1

#define BLACK_PAWN                                                             \
    (1ULL << 48) | (1ULL << 49) | (1ULL << 50) | (1ULL << 51) | (1ULL << 52) | \
        (1ULL << 53) | (1ULL << 54) |                                          \
        (1ULL << 55)                             // Black pawns on seventh rank
#define BLACK_KNIGHT (1ULL << 57) | (1ULL << 62) // Black knights on B8 and G8
#define BLACK_BISHOP (1ULL << 58) | (1ULL << 61) // Black bishops on C8 and F8
#define BLACK_ROOK (1ULL << 56) | (1ULL << 63)   // Black rooks on A8 and H8
#define BLACK_QUEEN (1ULL << 59)                 // Black queen on D8
#define BLACK_KING (1ULL << 60)                  // Black king on E8

// Define special move flags
#define NORMAL_MOVE 0
#define DOUBLE_PAWN_PUSH 1
#define QUEEN_SIDE_CASTLE 2
#define KING_SIDE_CASTLE 3
#define EN_PASSANT 4
#define PROMOTION 5

#endif /* CONSTANTS_H */
