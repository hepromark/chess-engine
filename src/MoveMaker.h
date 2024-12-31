//
// Created by markd on 2024-12-23.
//

#ifndef CHESS_ENGINE_MOVEMAKER_H
#define CHESS_ENGINE_MOVEMAKER_H

#include "Board.h"
#include "MoveGenerator.h"
#include <stack>

/*
 * Metadata history struct.
 */
struct BoardData {
    bool w_castle_k;
    bool w_castle_q;
    bool b_castle_k;
    bool b_castle_q;
    int enpassant_index;
    int fifty_move_counter;
    int captured_piece_type;

    BoardData(bool wk, bool wq, bool bk, bool bq, int en, int fif) : w_castle_k(wk), w_castle_q(wq), b_castle_k(bk), b_castle_q(bq), enpassant_index(en), fifty_move_counter(fif) {};
};

/*
 * Move making and unmaking class for the chess engine.
 * Incremental update architecture.
 */
class MoveMaker {
public:
    // Global board to incrementally update. This board is constantly
    // mutated by reference.
    Board* board;

    // History metadata stack
    // This stores the previous data
    std::stack<BoardData> history;

    // Make and unmake
    // Can only make/unmake LEGAL moves
    void make_move(const Move &move);
    void unmake_move(const Move &move);

    // Checks
    // Method to compute if this move leaves opponent side in check
    bool opponent_checking_move(const Move &move);

    // Method to compute if this pseudo-legal move leaves own side in check.
    // 'false' == illegal move
    bool self_checking_move(const Move& move);

    // Helpers
    void remove_piece_from_records(PieceEntry* piece);

    // Constructor
    MoveMaker(Board* board_);
};


#endif //CHESS_ENGINE_MOVEMAKER_H
