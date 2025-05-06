//
// Created by markd on 2024-12-23.
//

#include "MoveMaker.h"
#include "Board.h"
#include "MoveGenerator.h"
#include <stack>
#include <cassert>

MoveMaker::MoveMaker(Board* board_) {
    board = board_;
}

void MoveMaker::make_move(const Move &move) {
    /*
     * Makes a pseudo-legal move and mutates the board.
     */
    BoardData data(board->w_castle_k, board->w_castle_q, board->b_castle_k, board->b_castle_q, board->enpassant_index, board->fifty_move_counter);

    // Remove piece at 'from' square
    PieceEntry* piece = board->board[move.from_square_index];
    board->board[move.from_square_index] = nullptr;

    // Assume there is no captures (if there is this gets set to 0)
    board->fifty_move_counter += 1;

    // If capturing move then remove piece at 'to' square
    if (move.type[2]) {
        // Remove from board array
        PieceEntry* dead = board->board[move.to_square_index];
        // If ep then captured piece isn't at 'to' square
        if (move.type[0] && !move.type[1]) {
            dead = board->board[move.to_square_index + board->side_to_play * DirectionVec::S];
        }
        board->board[dead->piece_location] = nullptr;

        // Remove from piece records
        // TODO: Somehow speedup up new piece insertion
        PieceEntry** piece_records = (board->side_to_play == Colour::WHITE) ? board->b_piece_records : board->w_piece_records;
        for (int i = 0; i < 16; ++i) {
            if (piece_records[i] == dead) {
                piece_records[i] = nullptr;
                break;
            }
        }

        // Board data
        board->fifty_move_counter = 0;
        data.captured_piece_type = dead->piece_value;

        // TODO: will PieceEntries be dynamically allocated?
        delete dead;
        dead = nullptr;
    }

    // Place original piece at new square if normal
    piece->piece_location = move.to_square_index;
    board->board[piece->piece_location] = piece;

    // Is a promoting move; pawn -> another piece
    if (move.type[3]) {
        int encoded_promo_bits = move.type.to_ulong() & ((1 << 2) - 1);
        switch (encoded_promo_bits) {
            case 0b00:
                // knight
                piece->piece_value = 2 * piece->colour;
                break;
            case 0b01:
                // bishop
                piece->piece_value = 5 * piece->colour;
                break;
            case 0b10:
                // rook
                piece->piece_value = 6 * piece->colour;
                break;
            case 0b11:
                // queen
                piece->piece_value = 7 * piece->colour;
                break;
        }
    }

    switch (move.type.to_ullong()) {
        case (0b0010): {
            // King side castles
            int start_ind = 7;
            int end_ind = 5;
            if (board->side_to_play == -1) {
                start_ind += DirectionVec::N * 8;
                end_ind += DirectionVec::N * 8;
            }
            PieceEntry* rook = board->board[start_ind];
            rook->piece_location = end_ind;
            board->board[start_ind] = nullptr;
            board->board[rook->piece_location] = rook;

            (board->side_to_play == 1) ? board->w_castle_k = false : board->w_castle_k = false;
            break;
        }

        case (0b0011): {
            // Queen side castles
            // King side castles
            int start_ind = 0;
            int end_ind = 3;
            if (board->side_to_play == -1) {
                start_ind += DirectionVec::N * 8;
                end_ind += DirectionVec::N * 8;
            }
            PieceEntry* rook = board->board[start_ind];
            rook->piece_location = end_ind;
            board->board[start_ind] = nullptr;
            board->board[rook->piece_location] = rook;

            (board->side_to_play == 1) ? board->w_castle_q = false : board->w_castle_q = false;
            break;
        }
    }


    // Update history
    history.push(data);

    // Update metadata
    board->side_to_play *= -1;
}

void MoveMaker::unmake_move(const Move &move) {
    if (history.empty()) {
        assert(false);
    }
    BoardData prev_data = history.top();
    history.pop();

    // Remove piece at 'to' square
    PieceEntry* piece = board->board[move.to_square_index];
    board->board[move.to_square_index] = nullptr;

    // Place piece back at 'from' square
    piece->piece_location = move.from_square_index;
    board->board[move.from_square_index] = piece;

    // Capturing moves case
    if (move.type[2]) {
        // Previous piece captured same colour as current side to play
        PieceEntry** piece_records = (board->side_to_play == Colour::WHITE) ? board->w_piece_records : board->b_piece_records;

        // normal or promo capture; make a new piece and insert it back
        if (move.type.to_ulong() != 0b0101) {
            PieceEntry* captured_piece = new PieceEntry(prev_data.captured_piece_type, move.to_square_index, board->side_to_play);
            board->board[move.to_square_index] = captured_piece;
            piece_records[board->get_empty_records_index(board->side_to_play)] = captured_piece;
        }

        // en-passant capture; put piece back at previous en-passant index
        else {
            int ind = prev_data.enpassant_index + board->side_to_play * DirectionVec::N;
            PieceEntry* captured_pawn = new PieceEntry(prev_data.captured_piece_type, ind, board->side_to_play*-1);
            board->board[ind] = captured_pawn;
            piece_records[board->get_empty_records_index(board->side_to_play)] = captured_pawn;
        }
    }

    // Promotion case: change piece back to pawn
    if (move.type[3]) {
        piece->piece_value = piece->colour;
    }

    // Castling case: move rook back to original position
    switch (move.type.to_ullong()) {
        case (0b0010): {
            // King side castles
            int start_ind = 7;
            int end_ind = 5;
            if (board->side_to_play == 1) {
                start_ind += DirectionVec::N * 8;
                end_ind += DirectionVec::N * 8;
            }
            PieceEntry* rook = board->board[end_ind];
            rook->piece_location = start_ind;
            board->board[end_ind] = nullptr;
            board->board[rook->piece_location] = rook;
            break;
        }
        case (0b0011): {
            // Queen side castles
            int start_ind = 0;
            int end_ind = 3;
            if (board->side_to_play == 1) {
                start_ind += DirectionVec::N * 8;
                end_ind += DirectionVec::N * 8;
            }
            PieceEntry* rook = board->board[end_ind];
            rook->piece_location = start_ind;
            board->board[end_ind] = nullptr;
            board->board[rook->piece_location] = rook;
            break;
        }
    }

    // Update prev board state
    board->side_to_play *= -1;
    board->b_castle_k = prev_data.b_castle_k;
    board->b_castle_q = prev_data.b_castle_q;
    board->w_castle_k = prev_data.w_castle_k;
    board->w_castle_q = prev_data.w_castle_q;
    board->enpassant_index = prev_data.enpassant_index;
    board->fifty_move_counter = prev_data.fifty_move_counter;
}

bool MoveMaker::opponent_checking_move(const Move &move) {
    // We assume the move as already been made, so the piece
    // already exists on target square. This also removes
    // promotion complications.

    // Handle direct checks on a piece-by-piece basis
    switch (board->board[move.to_square_index]->piece_value) {
        // Pawn
        case 1: {
            break;
        }

        // Knight
        case 2: {
            const int directions[8] = {DirectionVec::NNE,
                                       DirectionVec::NNW,
                                       DirectionVec::NEE,
                                       DirectionVec::NWW,
                                       DirectionVec::SSE,
                                       DirectionVec::SEE,
                                       DirectionVec::SSW,
                                       DirectionVec::SWW}
            break;
        }

        // King
        case 3: {
            break;
        }

        // Sliding piece
        default: {
        }
    }

    // Any piece can do a discovery check

    // Ep case
    // we just do a 'raw' check since ep is rare & complicated changes
}