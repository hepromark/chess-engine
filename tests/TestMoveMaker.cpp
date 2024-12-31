//
// Created by markd on 2024-12-25.
//

#include "TestMoveMaker.h"
#include "TestUtils.h"
#include "../src/MoveGenerator.h"
#include "../src/Board.h"
#include "../src/MoveMaker.h"

#include <iostream>
#include <cassert>
#include <vector>
#include <unordered_set>
#include <math.h>
#include <fstream>

int TestMoveMaker::single_side_perft(Board *board, int depth, MoveGenerator *gen, MoveMaker *maker) {
    if (!depth) {
        return 1;
    }

    int nodes = 0;

    std::vector<Move> moves = gen->get_all_pseudo_legal_moves(board);
    for (const Move& move : moves) {
        maker->make_move(move);
        board->side_to_play = 1;
        nodes += single_side_perft(board, depth - 1, gen, maker);
        maker->unmake_move(move);
    }

    return nodes;
}

int perft_with_debug(std::ofstream &file, Board *board, int depth, MoveGenerator *gen, MoveMaker *maker) {
    if (!depth) {
        return 1;
    }

    int nodes = 0;

    std::vector<Move> moves = gen->get_all_pseudo_legal_moves(board);
    for (const Move& move : moves) {
        maker->make_move(move);
        int new_nodes = perft_with_debug(file, board, depth - 1, gen, maker);
        if (depth - 1) {
            file << move.from_square_index << " -> " << move.to_square_index << ": " << new_nodes << std::endl;
        }
        nodes += new_nodes;
        maker->unmake_move(move);
    }

    return nodes;
}

int TestMoveMaker::perft(Board *board, int depth, MoveGenerator *gen, MoveMaker *maker) {
    if (!depth) {
        return 1;
    }

    int nodes = 0;

    std::vector<Move> moves = gen->get_all_pseudo_legal_moves(board);
    for (const Move& move : moves) {
        maker->make_move(move);
        nodes += perft(board, depth - 1, gen, maker);
        maker->unmake_move(move);
    }

    return nodes;
}

bool TestMoveMaker::test_make_unmake_quiet_moves() {
    Board* board = new Board();
    MoveGenerator gen;
    MoveMaker move_maker(board);

    // Make knight moves
    const int START_IND = 51;
    PieceEntry* piece = new PieceEntry(2, START_IND, 1);
    TestUtils::place_piece(board, piece);
    std::vector<Move> moves = gen.get_all_pseudo_legal_moves(board);

    assert(moves.size() == 8);

    int lines = 0;
    for (const Move& move : moves) {
        move_maker.make_move(move);
        assert(!board->board[START_IND]);
        assert(board->board[move.to_square_index]);
        assert(board->side_to_play == -1);

        move_maker.unmake_move(move);
        assert(board->board[START_IND]);
        assert(!board->board[move.to_square_index]);
        assert(board->side_to_play == 1);

        lines++;
    }
    assert(lines == moves.size());

    // single Perft for ray pieces
    piece->piece_value = 6; // rook
    int nodes = single_side_perft(board, 6, &gen, &move_maker);
    std::cout << nodes << " visited" << std::endl;

    // On an empty board, rooks have 14 legal moves
    assert(nodes == pow(14, 6));

    delete piece;
    piece = nullptr;
    delete board;
    board = nullptr;

    return true;
}

bool TestMoveMaker::test_make_unmake_captures() {
    Board* board = new Board();
    MoveGenerator gen;
    MoveMaker move_maker(board);

    // White Rook can take pawn or knight
    PieceEntry* rook = new PieceEntry(6, 51, 1);
    PieceEntry* pawn = new PieceEntry(-1, 50, -1);
    PieceEntry* knight = new PieceEntry(-2, 52, -1);
    TestUtils::place_piece(board, rook);
    TestUtils::place_piece(board, pawn);
    TestUtils::place_piece(board, knight);

    // Manual checks
    move_maker.make_move(Move(51, 50, 0b0100));
    assert(gen.get_all_pseudo_legal_moves(board).size() == 8);
    move_maker.unmake_move(Move(51, 50, 0b0100));

    std::ofstream file(R"(C:\Users\markd\Desktop\Coding\chess_engine\src\perft.txt)");
    if (!file.is_open()) {
        std::cout << "failed to open file" << std::endl;
        return false;
    }
    int nodes = perft_with_debug(file, board, 2, &gen, &move_maker);
    assert(nodes == 9 * 6 + 10 + 8 + 1);

    file.close();

    delete board;

    return true;
}

bool TestMoveMaker::test_make_unmake_promotions() {
    Board* board = new Board();
    MoveGenerator gen;
    MoveMaker move_maker(board);

    // Pawn can take either knights to promote
    PieceEntry* pawn = new PieceEntry(1, 99, 1);
    PieceEntry* knight1 = new PieceEntry(-2, 114, -1);
    PieceEntry* knight2 = new PieceEntry(-2, 116, -1);
    TestUtils::place_piece(board, pawn);
    TestUtils::place_piece(board, knight1);
    TestUtils::place_piece(board, knight2);

    // Manual checks
    move_maker.make_move(Move(99, 114, 0b1111));
    assert(board->board[114]->piece_value == 7);
    assert(board->board[114]->colour == 1);
    move_maker.unmake_move(Move(99, 114, 0b1111));
    assert(board->board[114]->piece_value == -2);
    assert(board->board[99]->piece_value == 1);

    std::ofstream file(R"(C:\Users\markd\Desktop\Coding\chess_engine\src\perft.txt)");
    if (!file.is_open()) {
        std::cout << "failed to open file" << std::endl;
        return false;
    }
    int nodes = perft_with_debug(file, board, 1, &gen, &move_maker);
    assert(nodes == 3 * 4);  // Promo available on 3 squares

    nodes = perft_with_debug(file, board, 2, &gen, &move_maker);
    assert(nodes == 4 * 8 + 8 * 4);

    file.close();

    delete board;

    return true;
}

bool TestMoveMaker::test_make_unmake_enpassant() {
    Board* board = new Board();
    MoveGenerator gen;
    MoveMaker move_maker(board);

    // Pawn can take either knights to promote
    PieceEntry* w1 = new PieceEntry(1, 65, 1);
    PieceEntry* w2 = new PieceEntry(1, 54, 1);
    PieceEntry* b1 = new PieceEntry(-1, 66, -1);
    PieceEntry* b2 = new PieceEntry(-1, 53, -1);
    TestUtils::place_piece(board, w1);
    TestUtils::place_piece(board, b1);
    TestUtils::place_piece(board, w2);
    TestUtils::place_piece(board, b2);

    // b1 pawn will be captured ep
    board->enpassant_index = 82;

    // Manual checks
    move_maker.make_move(Move(65, 82, 0b0101));
    assert(board->board[82]->piece_value == 1);
    assert(board->board[65] == nullptr);
    assert(board->board[66] == nullptr);
    move_maker.unmake_move(Move(65, 82, 0b0101));
    assert(board->board[66]->piece_value == -1);
    assert(board->board[65]->piece_value == 1);
    assert(board->board[82] == nullptr);

    std::ofstream file(R"(C:\Users\markd\Desktop\Coding\chess_engine\src\perft.txt)");
    if (!file.is_open()) {
        std::cout << "failed to open file" << std::endl;
        return false;
    }
    int nodes = perft_with_debug(file, board, 2, &gen, &move_maker);
    assert(nodes == 2 * 2 + 1 * 1);


    board->enpassant_index = 38;
    board->side_to_play = -1;
    nodes = perft(board, 2, &gen, &move_maker);
    assert(nodes == 2 * 2 + 1 * 1);

    file.close();

    delete board;

    return true;
}

bool TestMoveMaker::test_make_unmake_castles() {
    Board* board = new Board();
    MoveGenerator gen;
    MoveMaker move_maker(board);

    // Pawn can take either knights to promote
    PieceEntry* king = new PieceEntry(3, 4, 1);
    PieceEntry* krook = new PieceEntry(6, 7, 1);
    PieceEntry* qrook = new PieceEntry(6, 0, 1);
    TestUtils::place_piece(board, king);
    TestUtils::place_piece(board, krook);
    TestUtils::place_piece(board, qrook);

    board->w_castle_k = true;
    board->w_castle_q = true;

    // Manual checks
    move_maker.make_move(Move(4, 6, 0b0010));
    assert(board->w_castle_q && !board->w_castle_k);
    assert(board->board[4] == nullptr);
    assert(board->board[5]->piece_value == 6);
    assert(board->board[6]->piece_value == 3);
    assert(board->board[7] == nullptr);
    move_maker.unmake_move(Move(4, 6, 0b0010));
    assert(board->w_castle_q && board->w_castle_k);
    assert(board->board[4]->piece_value == 3);
    assert(board->board[5] == nullptr);
    assert(board->board[6] == nullptr);
    assert(board->board[7]->piece_value == 6);

    std::ofstream file(R"(C:\Users\markd\Desktop\Coding\chess_engine\src\perft.txt)");
    if (!file.is_open()) {
        std::cout << "failed to open file" << std::endl;
        return false;
    }

    board->w_castle_q = true;
    int nodes = perft_with_debug(file, board, 1, &gen, &move_maker);
    assert(nodes == 26);
    file.close();

    delete board;

    return true;
}