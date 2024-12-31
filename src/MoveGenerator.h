//
// Created by markd on 2024-12-19.
//

#ifndef CHESS_ENGINE_MOVEGENERATOR_H
#define CHESS_ENGINE_MOVEGENERATOR_H

#include <bitset>
#include <vector>
#include "Board.h"

struct Move {
    unsigned int from_square_index;
    unsigned int to_square_index;
    std::bitset<4> type;

    Move(unsigned int from, unsigned int to, std::bitset<4> t): from_square_index(from), to_square_index(to), type(t) {};
    Move();
};

struct DirectionVec {
    /*
     * Increments to the current 0x88 index to move in that direction.
     */
    static const int N = 16;
    static const int S = -16;
    static const int E = 1;
    static const int W = -1;

    static const int NE = N + E;
    static const int NW = N + W;
    static const int SE = S + E;
    static const int SW = S + W;

    static const int NNE = 33;
    static const int NNW = 31;
    static const int NEE = 18;
    static const int NWW = 14;
    static const int SSE = -31;
    static const int SSW = -33;
    static const int SEE = -14;
    static const int SWW = -18;
};

/*
 * Pseudo-legal move generator class
 */
class MoveGenerator {
public:
    // Constants
    static const unsigned int BIN_0x88 = 0b10001000;


    // Movement checker functions
    const bool is_in_bounds(unsigned int index_8x8);
    const bool is_empty(Board* board, unsigned int index_8x8);
    const bool can_capture(Board* board, unsigned int index, int colour);

    // Constructor
    MoveGenerator();

    // Helper conversion
    unsigned int to_index_0x88(unsigned int rank, unsigned int file);
    unsigned int to_index_0x88(unsigned int index_8x8);
    unsigned int to_index_8x8(unsigned int index_0x88);

    unsigned int get_rank(unsigned int index_0x88);
    unsigned int get_file(unsigned int index_0x88);

    // Generator methods
    bool can_move_or_take(Board* board, int index, int colour, bool &capture);

    // Gets all pseudo legal moves in a position
    std::vector<Move> get_all_pseudo_legal_moves(Board *board);
    std::vector<Move> get_pseudo_legal_moves_piece(Board *board, PieceEntry *piece, std::vector<int> directions, bool long_range);

};


#endif //CHESS_ENGINE_MOVEGENERATOR_H
