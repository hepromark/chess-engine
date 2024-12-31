//
// Created by markd on 2023-09-24.
//

#include "TestMoveGenerator.h"
#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "TestUtils.h"

#include <iostream>
#include <cassert>
#include <unordered_map>
#include <unordered_set>


bool TestMoveGenerator::test_board_constructor() {
    Board board;
    assert(board.board);

    return true;
}

bool TestMoveGenerator::test_out_of_bounds() {
    Board board;
    MoveGenerator gen;

    for (int i = 0; i < 128; i++) {
        bool in_bound = gen.is_in_bounds(i);
        if (i % 16 <= 7) {
            assert(in_bound);
        } else {
            assert(!in_bound);
        }
    }
    return true;
}

bool TestMoveGenerator::test_occupied_same_colour() {
    return true;
}

bool TestMoveGenerator::test_basic_quiet_move_generation() {
    Board *board = new Board();
    board->side_to_play = 1;
    MoveGenerator gen = MoveGenerator();

    // Create pieces and check pseudo legal moves

    // King at d4 so 8 legal moves
    PieceEntry* king = new PieceEntry(3, 51, 1);
    board->w_piece_records[0] = king;
    board->board[king->piece_location] = king;
    std::vector<Move> moves = gen.get_all_pseudo_legal_moves(board);
    assert(moves.size() == 8);

    // King at a1 so 3 legal moves
    board->board[king->piece_location] = nullptr;
    king->piece_location = 0;
    board->board[king->piece_location] = king;
    moves = gen.get_all_pseudo_legal_moves(board);
    assert(moves.size() == 3);

    board->board[king->piece_location] = nullptr;
    delete king;
    king = nullptr;

    // Queen at d4
    PieceEntry* queen = new PieceEntry(7, 51, 1);
    board->w_piece_records[0] = queen;
    board->board[queen->piece_location] = queen;
    moves = gen.get_all_pseudo_legal_moves(board);
    assert(moves.size() == 27);

    // Queen at h8
    board->board[queen->piece_location] = nullptr;
    queen->piece_location = 0;
    board->board[queen->piece_location] = queen;
    moves = gen.get_all_pseudo_legal_moves(board);
    assert(moves.size() == 21);

    board->board[queen->piece_location] = nullptr;
    delete queen;
    queen = nullptr;

    // Knight at d4
    PieceEntry* knight = new PieceEntry(2, 51, 1);
    board->w_piece_records[0] = knight;
    board->board[knight->piece_location] = knight;
    moves = gen.get_all_pseudo_legal_moves(board);
    assert(moves.size() == 8);

    // Knight at a8
    board->board[knight->piece_location] = nullptr;
    knight->piece_location = 7;
    board->board[knight->piece_location] = knight;
    moves = gen.get_all_pseudo_legal_moves(board);
    assert(moves.size() == 2);

    board->board[knight->piece_location] = nullptr;
    delete knight;
    knight = nullptr;

    // White Pawn at d1
    PieceEntry* pawn = new PieceEntry(1, 19, 1);
    board->w_piece_records[0] = pawn;
    board->board[pawn->piece_location] = pawn;
    moves = gen.get_all_pseudo_legal_moves(board);
    assert(moves.size() == 2);

    // White pawn at d2
    board->board[pawn->piece_location] = nullptr;
    pawn->piece_location = 35;
    board->board[pawn->piece_location] = pawn;
    moves = gen.get_all_pseudo_legal_moves(board);
    assert(moves.size() == 1);
    assert(moves[0].to_square_index == 51);

    // Black pawn at d7
    board->side_to_play = Colour::BLACK;
    board->w_piece_records[0] = nullptr;
    board->b_piece_records[0] = pawn;
    board->board[pawn->piece_location] = nullptr;
    pawn->piece_location = 99;
    pawn->piece_value = -1;
    pawn->colour = -1;
    board->board[pawn->piece_location] = pawn;
    moves = gen.get_all_pseudo_legal_moves(board);
    assert(moves.size() == 2);

    // Black pawn at d6
    board->board[pawn->piece_location] = nullptr;
    pawn->piece_location = 83;
    board->board[pawn->piece_location] = pawn;
    moves = gen.get_all_pseudo_legal_moves(board);
    assert(moves.size() == 1);
    assert(moves[0].to_square_index == 67);

    board->board[pawn->piece_location] = nullptr;
    delete pawn;
    pawn = nullptr;

    // Cleanup
    delete board;
    board = nullptr;

    return true;
}

bool TestMoveGenerator::test_pawn_captures() {
    Board* board = new Board();
    MoveGenerator gen = MoveGenerator();

    PieceEntry* wp = new PieceEntry(1, 17, Colour::WHITE);
    PieceEntry* bp1 = new PieceEntry(-1, 34, Colour::BLACK);
    PieceEntry* bp2 = new PieceEntry(-1, 32, Colour::BLACK);

    TestUtils::place_piece(board, wp);
    TestUtils::place_piece(board, bp1);
    TestUtils::place_piece(board, bp2);

    std::unordered_set expected_move_end_squares = {32, 33, 34, 49};
    std::vector<Move> moves = gen.get_all_pseudo_legal_moves(board);
    assert(moves.size() == 4);
    for (const Move &move : moves) {
        assert(expected_move_end_squares.count(move.to_square_index));
        expected_move_end_squares.erase(move.to_square_index);
    }

    TestUtils::remove_piece(board, bp1);

    std::unordered_set expected_move_end_squares_2= {32, 33, 49};
    moves = gen.get_all_pseudo_legal_moves(board);
    assert(moves.size() == 3);
    for (const Move &move : moves) {
        assert(expected_move_end_squares_2.count(move.to_square_index));
        expected_move_end_squares_2.erase(move.to_square_index);
    }

    delete board;
    board = nullptr;

    return true;
}

bool TestMoveGenerator::test_ray_captures() {
    /*
     * Tests capturing for ray pieces.
     */

    Board* board = new Board();
    MoveGenerator gen = MoveGenerator();

    PieceEntry* queen = new PieceEntry(7, 51, 1);
    PieceEntry* wpawn_1 = new PieceEntry(-1, 54, 1);
    PieceEntry* bpawn_1 = new PieceEntry(-1, 50, -1);
    PieceEntry* bpawn_2 = new PieceEntry(-1, 49, -1); // xrayed but covered
    PieceEntry* bpawn_3 = new PieceEntry(-1, 3, -1);
    PieceEntry* bpawn_4 = new PieceEntry(-1, 102, -1);
    PieceEntry* bpawn_5 = new PieceEntry(-1, 37, -1); // not xrayed
    PieceEntry* bpawn_6 = new PieceEntry(-1, 55, -1); // xrayed but covered

    TestUtils::place_piece(board, queen);
    TestUtils::place_piece(board, wpawn_1);
    TestUtils::place_piece(board, bpawn_1);
    TestUtils::place_piece(board, bpawn_2);
    TestUtils::place_piece(board, bpawn_3);
    TestUtils::place_piece(board, bpawn_4);
    TestUtils::place_piece(board, bpawn_5);
    TestUtils::place_piece(board, bpawn_6);

    std::vector<Move> moves = gen.get_all_pseudo_legal_moves(board);
    moves.erase(std::remove_if(moves.begin(), moves.end(), [](Move move) { return move.from_square_index == 54; }), moves.end());

    std::unordered_set<int> capturing_moves_end_indicies;
    for (const Move &move : moves) {
        if (move.type[2]) {
            capturing_moves_end_indicies.insert(move.to_square_index);
        }

        assert(move.to_square_index != 119);
    }
    assert(moves.size() == 22);
    assert(capturing_moves_end_indicies.size() == 3);
    assert(capturing_moves_end_indicies.count(50));
    assert(capturing_moves_end_indicies.count(3));
    assert(capturing_moves_end_indicies.count(102));

    delete board;
    board = nullptr;
    delete queen;
    queen = nullptr;
    delete bpawn_1;
    delete bpawn_2;
    delete bpawn_3;
    delete bpawn_4;
    delete bpawn_5;
    bpawn_1 = nullptr;
    bpawn_2 = nullptr;
    bpawn_3 = nullptr;
    bpawn_4 = nullptr;
    bpawn_5 = nullptr;

    return true;
}

bool TestMoveGenerator::test_enpasant() {
    /*
     * TestMoveGenerator pseudo legal en-passant move generation.
     */
    Board* board = new Board();
    MoveGenerator gen;

    PieceEntry wpawn1(1, 49, 1);
    PieceEntry wpawn2(1, 51, 1);
    PieceEntry wpawn3(1, 69, 1);
    PieceEntry bpawn1(1, 50, -1);
    PieceEntry bpawn2(1, 52, -1);
    PieceEntry bpawn3(1, 70, -1);


    TestUtils::place_piece(board, &wpawn1);
    TestUtils::place_piece(board, &wpawn2);
    TestUtils::place_piece(board, &wpawn3);
    TestUtils::place_piece(board, &bpawn1);
    TestUtils::place_piece(board, &bpawn2);
    TestUtils::place_piece(board, &bpawn3);

    // En-passant against the index 49 white pawn
    board->side_to_play = -1;
    board->enpassant_index = 33;
    std::vector<Move> moves = gen.get_all_pseudo_legal_moves(board);
    moves.erase(std::remove_if(moves.begin(), moves.end(), [](Move move) { return move.type[2] != 1; }), moves.end());
    assert(moves.size() == 1);
    assert(moves[0].from_square_index == bpawn1.piece_location);
    assert(moves[0].to_square_index == 33);
    assert(moves[0].type == std::bitset<4>(0b0101));

    // En-passant against the index 49 white pawn
    board->enpassant_index = 35;
    moves = gen.get_all_pseudo_legal_moves(board);
    moves.erase(std::remove_if(moves.begin(), moves.end(), [](Move move) { return move.type[2] != 1; }), moves.end());
    assert(moves.size() == 2);
    assert(moves[0].to_square_index == 35);
    assert(moves[1].to_square_index == 35);
    assert(moves[0].type == std::bitset<4>(0b0101));
    assert(moves[1].type == std::bitset<4>(0b0101));

    // En-passant against the index 70 black pawn
    board->side_to_play = 1;
    board->enpassant_index = 86;
    moves = gen.get_all_pseudo_legal_moves(board);
    moves.erase(std::remove_if(moves.begin(), moves.end(), [](Move move) { return move.type[2] != 1; }), moves.end());
    assert(moves.size() == 1);
    assert(moves[0].to_square_index == board->enpassant_index);
    assert(moves[0].from_square_index == wpawn3.piece_location);
    assert(moves[0].type == std::bitset<4>(0b0101));


    delete board;
    board = nullptr;

    return true;
}

bool TestMoveGenerator::test_castling() {
    /*
     * TestMoveGenerator pseudo-legal castling move gen
     */
    /*
     * TestMoveGenerator pseudo legal en-passant move generation.
     */
    Board* board = new Board();
    MoveGenerator gen;

    PieceEntry wrook1(6, 0, 1);
    PieceEntry wrook2(6, 7, 1);
    PieceEntry brook1(-6, 112, -1);
    PieceEntry brook2(-6, 119, -1);
    PieceEntry wking(3, 4, 1);
    PieceEntry bking(-3, 116, -1);

    TestUtils::place_piece(board, &wrook1);
    TestUtils::place_piece(board, &wrook2);
    TestUtils::place_piece(board, &brook1);
    TestUtils::place_piece(board, &brook2);
    TestUtils::place_piece(board, &wking);
    TestUtils::place_piece(board, &bking);

    // No castles allowed
    std::vector<Move> moves = gen.get_all_pseudo_legal_moves(board);
    moves.erase(std::remove_if(moves.begin(), moves.end(), [](Move move) { return move.type[1] != 1; }), moves.end());
    assert(moves.size() == 0);

    // White can castle
    board->w_castle_q = true;
    board->w_castle_k = true;
    moves = gen.get_all_pseudo_legal_moves(board);
    moves.erase(std::remove_if(moves.begin(), moves.end(), [](Move move) { return move.type[1] != 1; }), moves.end());
    assert(moves.size() == 2);
    assert(moves[0].to_square_index == 6);
    assert(moves[1].to_square_index == 2);


    // Black can castle
    board->b_castle_q = true;
    board->b_castle_k = true;
    board->side_to_play = -1;
    moves = gen.get_all_pseudo_legal_moves(board);
    moves.erase(std::remove_if(moves.begin(), moves.end(), [](Move move) { return move.type[1] != 1; }), moves.end());
    assert(moves.size() == 2);
    assert(moves[0].to_square_index == 118);
    assert(moves[1].to_square_index == 114);

    // White castle blocked
    board->side_to_play = 1;
    PieceEntry blocker(2, 3, 1);
    PieceEntry blocker2(2, 6, 1);

    TestUtils::place_piece(board, &blocker);
    moves = gen.get_all_pseudo_legal_moves(board);
    moves.erase(std::remove_if(moves.begin(), moves.end(), [](Move move) { return move.type[1] != 1; }), moves.end());
    assert(moves.size() == 1);
    assert(moves[0].to_square_index == 6);

    TestUtils::place_piece(board, &blocker2);
    moves = gen.get_all_pseudo_legal_moves(board);
    moves.erase(std::remove_if(moves.begin(), moves.end(), [](Move move) { return move.type[1] != 1; }), moves.end());
    assert(moves.size() == 0);

    board->side_to_play = -1;

    delete board;
    board = nullptr;

    return true;
}