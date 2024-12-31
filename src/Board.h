//
// Created by markd on 2023-09-17.
//
#include <vector>
#include <array>
#include <string>
#include <unordered_map>
#include <string>

#ifndef CHESS_ENGINE_BOARD_H
#define CHESS_ENGINE_BOARD_H

struct Colour {
    static const int WHITE = 1;
    static const int BLACK = -1;
};

struct PieceEntry {
    // The value corresponding to piece type
    /*
     * +/- corresponds to +white and -black
     * 1 - pawn
     * 2 - knight
     * 3 - king
     * 5 - bishop
     * 6 - rook
     * 7 - queen
     * */
    int piece_value;

    // Index of piece in the 0x88 array, ranges 0 -> 127
    int piece_location;

    int colour;

    // Constructor
    PieceEntry(int val, int loc, int c): piece_value(val), piece_location(loc), colour(c) {};
};

class Board {
public:
    // 0x88 style board
    PieceEntry* board[128];

    // 16-element piece record array, each element for 1 piece.
    // 0 - 7: pawns
    // 8 - 9: knights
    // 10 - 11: bishops
    // 12- 13: rooks
    // 14: queen
    // 15: king
    PieceEntry* w_piece_records[16];
    PieceEntry* b_piece_records[16];

    // TODO: Add metadata stuff
    short side_to_play; // 1 for white, -1 for black
    int enpassant_index; // Pawn which may do an en-passant capture.
    bool w_castle_k;
    bool w_castle_q;
    bool b_castle_k;
    bool b_castle_q;
    int fifty_move_counter;

    // Constructor
    Board();
    ~Board();

    // Helpers
    int get_empty_records_index(int colour);
};

#endif //CHESS_ENGINE_BOARD_H
