cmake_minimum_required(VERSION 3.17)
project(battle_ships)

set(CMAKE_CXX_STANDARD 98)

add_executable(battle_ships main.cpp Board.cpp Board.h EnemyBoard.cpp EnemyBoard.h)