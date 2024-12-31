//
// Created by markd on 2024-12-19.
//

#include <bitset>
#include <iostream>
#include "MoveGenerator.h"
#include <cassert>

MoveGenerator::MoveGenerator() {};

const bool MoveGenerator::is_in_bounds(unsigned int index_8x8) {
    return (index_8x8 & 0b10001000) == 0b000000000;
}

std::vector<Move> MoveGenerator::get_all_pseudo_legal_moves(Board* board) {
    std::vector<Move> move_list;
    PieceEntry** piece_records = (board->side_to_play == Colour::WHITE) ? board->w_piece_records : board->b_piece_records;
    // Loop through all the pieces and find legal moves
    for (int i = 0; i < 16; ++i) {
        PieceEntry* piece_entry = piece_records[i];
        if (!piece_entry) { continue; }
        unsigned int start_index = piece_entry->piece_location;
        switch (abs(piece_entry->piece_value)) {
            // Jump Pieces
            // King
            case 3: {
                // Normal moves
                std::vector<int> directions({
                                                    DirectionVec::N,
                                                    DirectionVec::NE,
                                                    DirectionVec::E,
                                                    DirectionVec::SE,
                                                    DirectionVec::S,
                                                    DirectionVec::SW,
                                                    DirectionVec::W,
                                                    DirectionVec::NW
                                            });
                std::vector<Move> moves = get_pseudo_legal_moves_piece(board, piece_entry,
                                                                       directions, false);

                move_list.insert(move_list.end(), moves.begin(), moves.end());

                // Castling
                // Only is concerned with the pseudo-legality (don't care about checks on any squares)

                // White castles
                if (piece_entry->colour == Colour::WHITE) {
                    // King-side castle
                    if (board->w_castle_k && is_empty(board, 5) && is_empty(board, 6)) {
                        move_list.emplace_back(piece_entry->piece_location, 6, 0b0010);
                    }
                    // Queen-side castle
                    if (board->w_castle_q && is_empty(board, 1) && is_empty(board, 2) && is_empty(board, 3)) {
                        move_list.emplace_back(piece_entry->piece_location, 2, 0b0011);
                    }
                }

                // Black castles
                else {
                    // King-side castle
                    if (board->b_castle_k && is_empty(board, 117) && is_empty(board, 118)) {
                        move_list.emplace_back(piece_entry->piece_location, 118, 0b0010);
                    }
                    // Queen-side castle
                    if (board->b_castle_q && is_empty(board, 113) && is_empty(board, 114) && is_empty(board, 115)) {
                        move_list.emplace_back(piece_entry->piece_location, 114, 0b0011);
                    }
                }
                break;
            }

            // Knight
            case 2: {
                std::vector<int> directions({
                                                    DirectionVec::NNE,
                                                    DirectionVec::NNW,
                                                    DirectionVec::NEE,
                                                    DirectionVec::NWW,
                                                    DirectionVec::SSE,
                                                    DirectionVec::SSW,
                                                    DirectionVec::SWW,
                                                    DirectionVec::SEE,
                                            });
                std::vector<Move> moves = get_pseudo_legal_moves_piece(board, piece_entry,
                                                                       directions, false);

                // Add moves
                move_list.insert(move_list.end(), moves.begin(), moves.end());
                break;
            }

            // Sliding Pieces
            // Rooks
            case 6: {
                std::vector<int> directions({
                                                    DirectionVec::N,
                                                    DirectionVec::E,
                                                    DirectionVec::S,
                                                    DirectionVec::W,
                                            });
                std::vector<Move> moves = get_pseudo_legal_moves_piece(board, piece_entry,
                                                                       directions, true);

                // Add moves
                move_list.insert(move_list.end(), moves.begin(), moves.end());
                break;
            }

            // Bishops
            case 5: {
                std::vector<int> directions({
                                                    DirectionVec::NE,
                                                    DirectionVec::NW,
                                                    DirectionVec::SE,
                                                    DirectionVec::SW,
                                            });
                std::vector<Move> moves = get_pseudo_legal_moves_piece(board, piece_entry,
                                                                       directions, true);

                // Add moves
                move_list.insert(move_list.end(), moves.begin(), moves.end());
                break;
            }

            // Queens
            case 7: {
                std::vector<int> directions({
                                                    DirectionVec::N,
                                                    DirectionVec::NE,
                                                    DirectionVec::E,
                                                    DirectionVec::SE,
                                                    DirectionVec::S,
                                                    DirectionVec::SW,
                                                    DirectionVec::W,
                                                    DirectionVec::NW
                                            });
                std::vector<Move> moves = get_pseudo_legal_moves_piece(board, piece_entry,
                                                                       directions, true);

                // Add moves
                move_list.insert(move_list.end(), moves.begin(), moves.end());
                break;
            }

            // Pawns
            case 1: {
                // Starting-square double move
                // White rules
                int movement_direction = DirectionVec::N;
                int capture_directions[2] = {DirectionVec::NE, DirectionVec::NW};
                int starting_rank = 1;
                int promotion_rank = 7;

                if (piece_entry->colour == Colour::BLACK) {
                    // Black rules
                    movement_direction = DirectionVec::S;
                    capture_directions[0] = DirectionVec::SE;
                    capture_directions[1] = DirectionVec::SW;
                    starting_rank = 6;
                    promotion_rank = 0;
                }

                // Moves
                unsigned int new_index = piece_entry->piece_location + movement_direction;
                if (!is_in_bounds(new_index)) {assert(false);}
                if (is_empty(board, new_index)) {
                    int rank = get_rank(new_index);

                    // Promotion
                    if (rank == promotion_rank) {
                        move_list.emplace_back(piece_entry->piece_location, new_index, 0b1000);
                        move_list.emplace_back(piece_entry->piece_location, new_index, 0b1001);
                        move_list.emplace_back(piece_entry->piece_location, new_index, 0b1010);
                        move_list.emplace_back(piece_entry->piece_location, new_index, 0b1011);
                    }

                    // Single or double moves
                    else {
                        move_list.emplace_back(piece_entry->piece_location, new_index, 0b0000);
                        // Check double
                        if (get_rank(piece_entry->piece_location) == starting_rank && is_empty(board, new_index + movement_direction)) {
                            move_list.emplace_back(piece_entry->piece_location, new_index + movement_direction, 0b0000);
                        }
                    }

                }

                // Captures
                for (int capture_direction: capture_directions) {
                    new_index = piece_entry->piece_location + capture_direction;
                    int rank = get_rank(new_index);

                    // Diagonal capture
                    if (is_in_bounds(new_index) && !is_empty(board, new_index) &&
                        can_capture(board, new_index, piece_entry->colour)) {

                        if (rank != promotion_rank) {
                            // no promo capture
                            move_list.emplace_back(piece_entry->piece_location, new_index, 0b0100);
                        }
                        else {
                            // Promo capture
                            move_list.emplace_back(piece_entry->piece_location, new_index, 0b1100);
                            move_list.emplace_back(piece_entry->piece_location, new_index, 0b1101);
                            move_list.emplace_back(piece_entry->piece_location, new_index, 0b1100);
                            move_list.emplace_back(piece_entry->piece_location, new_index, 0b1111);
                        }
                        continue;
                    }

                    // En-passant capture
                    if (new_index == board->enpassant_index) {
                        move_list.emplace_back(piece_entry->piece_location, new_index, 0b0101);
                    }

                    if (rank == promotion_rank) {
                    }
                }
            }
        }
    }

    return move_list;
}

bool MoveGenerator::can_move_or_take(Board *board, int index, int colour, bool &capture) {
    /**
     * Returns true if this square can be moved into (w or w/o) capture by a piece of input colour.
     * Assumes the square is valid.
     *
     * capture: bool. Returned by reference, true if this is a capturing move.
     */
     PieceEntry* piece = board->board[index];

     // Empty square
     if (!piece) {return true;}

     // Capture if piece is the other colour
     if (piece->colour != colour) {
         capture = true;
         return true;
     }

     return false;
}

std::vector<Move>
MoveGenerator::get_pseudo_legal_moves_piece(Board *board, PieceEntry *piece, std::vector<int> directions, bool long_range) {
    std::vector<Move> move_list;
    unsigned int start_index = piece->piece_location;
    for (int increment : directions) {
        // Try to 'ray' from the current position
        unsigned int current_index = start_index;
        do {
            current_index = current_index + increment;
            // In-bounds
            if (!is_in_bounds(current_index)) { break;}

            // Capture or move
            bool capture = false;
            if (can_move_or_take(board, current_index, piece->colour, capture)) {
                // Handle move encoding
                std::bitset<4> move_code(0b000u);
                move_code[2] = capture; // 2nd bit is true for captures
                move_list.emplace_back(start_index, current_index, move_code);
                if (capture) {
                    break;
                }

            } else {
                // In-bounds but can't capture nor move; is blocked by a friendly piece
                break;
            }
        } while (long_range); // Long range pieces should break out of this loop
    }
    return move_list;
}

const bool MoveGenerator::can_capture(Board *board, unsigned int index, int colour) {
    return board->board[index]->colour != colour;
}

const bool MoveGenerator::is_empty(Board *board, unsigned int index_8x8) {
    /**
     * Returns true if square is empty. Assumes square is in bounds.
     */
    return board->board[index_8x8] == nullptr;
}

unsigned int MoveGenerator::to_index_0x88(unsigned int rank, unsigned int file) {
    return rank * 16 + file;
}

unsigned int MoveGenerator::to_index_0x88(unsigned int index_8x8) {
    return index_8x8 + index_8x8 & ~7;
}

unsigned int MoveGenerator::to_index_8x8(unsigned int index_0x88) {
    return (index_0x88 + (index_0x88 & 7)) >> 1;
}

unsigned int MoveGenerator::get_file(unsigned int index_0x88) {
    std::bitset<8> binary_index(index_0x88);
    return binary_index.to_ulong() & 7;
}

unsigned int MoveGenerator::get_rank(unsigned int index_0x88) {
    std::bitset<8> binary_index(index_0x88);
    return binary_index.to_ulong() >> 4;
}


