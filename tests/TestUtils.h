//
// Created by markd on 2024-12-25.
//

#ifndef CHESS_ENGINE_TESTUTILS_H
#define CHESS_ENGINE_TESTUTILS_H

#include "../src/Board.h"

class TestUtils {
public:
    static void place_piece(Board* board, PieceEntry* piece);
    static void remove_piece(Board* board, PieceEntry* piece);
};


#endif //CHESS_ENGINE_TESTUTILS_H
