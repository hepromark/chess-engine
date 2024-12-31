//
// Created by markd on 2023-09-24.
//

#ifndef CHESS_ENGINE_TESTMOVEGENERATOR_H
#define CHESS_ENGINE_TESTMOVEGENERATOR_H

#include "../src/Board.h"
#include "../src/MoveGenerator.h"

class TestMoveGenerator {
    friend class Board;
    friend class MoveGenerator;

public:
    // Initialization tests
    static bool test_board_constructor();
    static bool test_out_of_bounds();
    static bool test_occupied_same_colour();
    static bool test_basic_quiet_move_generation();
    static bool test_pawn_captures();
    static bool test_ray_captures();
    static bool test_enpasant();
    static bool test_castling();

    // TestMoveGenerator Utils
};

#endif
