#ifndef CONSTANTS_H
#define CONSTANTS_H

// Define constants for the chess board
#define BOARD_SIZE 64
#define EMPTY_BOARD 0ULL

#define NUM_PIECE_BIT_BOARDS 12

#define MAX_MOVES 300

// Define bit masks for each square on the board
#define A1 (1ULL << 0)
#define B1 (1ULL << 1)
#define C1 (1ULL << 2)
#define D1 (1ULL << 3)
#define E1 (1ULL << 4)
#define F1 (1ULL << 5)
#define G1 (1ULL << 6)
#define H1 (1ULL << 7)
#define A2 (1ULL << 8)
#define B2 (1ULL << 9)
#define C2 (1ULL << 10)
#define D2 (1ULL << 11)
#define E2 (1ULL << 12)
#define F2 (1ULL << 13)
#define G2 (1ULL << 14)
#define H2 (1ULL << 15)
#define A3 (1ULL << 16)
#define B3 (1ULL << 17)
#define C3 (1ULL << 18)
#define D3 (1ULL << 19)
#define E3 (1ULL << 20)
#define F3 (1ULL << 21)
#define G3 (1ULL << 22)
#define H3 (1ULL << 23)
#define A4 (1ULL << 24)
#define B4 (1ULL << 25)
#define C4 (1ULL << 26)
#define D4 (1ULL << 27)
#define E4 (1ULL << 28)
#define F4 (1ULL << 29)
#define G4 (1ULL << 30)
#define H4 (1ULL << 31)
#define A5 (1ULL << 32)
#define B5 (1ULL << 33)
#define C5 (1ULL << 34)
#define D5 (1ULL << 35)
#define E5 (1ULL << 36)
#define F5 (1ULL << 37)
#define G5 (1ULL << 38)
#define H5 (1ULL << 39)
#define A6 (1ULL << 40)
#define B6 (1ULL << 41)
#define C6 (1ULL << 42)
#define D6 (1ULL << 43)
#define E6 (1ULL << 44)
#define F6 (1ULL << 45)
#define G6 (1ULL << 46)
#define H6 (1ULL << 47)
#define A7 (1ULL << 48)
#define B7 (1ULL << 49)
#define C7 (1ULL << 50)
#define D7 (1ULL << 51)
#define E7 (1ULL << 52)
#define F7 (1ULL << 53)
#define G7 (1ULL << 54)
#define H7 (1ULL << 55)
#define A8 (1ULL << 56)
#define B8 (1ULL << 57)
#define C8 (1ULL << 58)
#define D8 (1ULL << 59)
#define E8 (1ULL << 60)
#define F8 (1ULL << 61)
#define G8 (1ULL << 62)
#define H8 (1ULL << 63)

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
