//
// Created by markd on 2024-12-25.
//

#ifndef CHESS_ENGINE_TESTMOVEMAKER_H
#define CHESS_ENGINE_TESTMOVEMAKER_H

#include "../src/Board.h"
#include "../src/MoveMaker.h"
#include "../src/MoveGenerator.h"

class TestMoveMaker {
public:
    static bool test_make_unmake_quiet_moves();
    static bool test_make_unmake_captures();
    static bool test_make_unmake_promotions();
    static bool test_make_unmake_castles();
    static bool test_make_unmake_enpassant();

    static int single_side_perft(Board* board, int depth, MoveGenerator* gen, MoveMaker* maker);
    static int perft(Board* board, int depth, MoveGenerator* gen, MoveMaker* maker);
};


#endif //CHESS_ENGINE_TESTMOVEMAKER_H
