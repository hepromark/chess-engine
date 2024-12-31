//
// Created by markd on 2023-09-17.
//

#include "Eval.h"
#include <cassert>

Board::Board() {
    for (PieceEntry* & i : board) {
        i = nullptr;
    }

    for (PieceEntry* & entry : w_piece_records) {
        entry = nullptr;
    }
    for (PieceEntry* & entry : b_piece_records) {
        entry = nullptr;
    }

    side_to_play = 1;
    enpassant_index = -1;
    w_castle_k = false;
    w_castle_q = false;
    b_castle_k = false;
    b_castle_q = false;

    fifty_move_counter = 0;
}

int Board::get_empty_records_index(int colour) {
    PieceEntry** piece_records = (side_to_play == Colour::WHITE) ? w_piece_records : b_piece_records;
    for (int i = 0; i < 16; ++i) {
        if (!piece_records[i]) {return i;}
    }
    assert(false); // Shouldn't happen
}

Board::~Board() {
    for (auto piece : w_piece_records) {
        if (!piece) {delete piece;};
    }
    for (auto piece : b_piece_records) {
        if (!piece) {delete piece;}
    }
}
