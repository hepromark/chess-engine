//
// Created by markd on 2024-12-25.
//

#include "TestUtils.h"

void TestUtils::place_piece(Board *board, PieceEntry *piece) {
    board->board[piece->piece_location] = piece;
    for (int i = 0; i < 16; ++i) {
        if (piece->colour == Colour::WHITE && board->w_piece_records[i] == nullptr) {
            board->w_piece_records[i] = piece;
            break;
        } else if (piece->colour == Colour::BLACK && board->b_piece_records[i] == nullptr) {
            board->b_piece_records[i] = piece;
            break;
        }
    }
}

void TestUtils::remove_piece(Board *board, PieceEntry *piece) {
    board->board[piece->piece_location] = nullptr;
    for (int i = 0; i < 16; ++i) {
        if (piece->colour == Colour::WHITE && board->w_piece_records[i] == piece) {
            board->w_piece_records[i] = nullptr;
            break;
        } else if (piece->colour == Colour::BLACK && board->b_piece_records[i] == piece) {
            board->b_piece_records[i] = nullptr;
            break;
        }
    }
}
