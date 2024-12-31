cmake_minimum_required(VERSION 3.25)
project(chess_engine)

set(CMAKE_CXX_STANDARD 17)

add_executable(chess_engine src/main.cpp src/Board.cpp src/Board.h tests/TestMoveGenerator.cpp tests/TestMoveGenerator.h src/MoveGenerator.cpp src/MoveGenerator.h src/MoveMaker.h src/MoveMaker.cpp utils/constants.h tests/TestUtils.h tests/TestUtils.cpp tests/TestMoveMaker.cpp)
