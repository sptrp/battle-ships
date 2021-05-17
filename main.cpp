#include <iostream>
#include "Board.h"
#include "EnemyBoard.h"

int main() {
    std::cout << "Hello, Battle Ships!" << std::endl;

    Board* myBoard = new Board(7);

    try {
        myBoard->RandomizeShips();

    } catch (std::exception& e) {

        std::cout << e.what() << std::endl;
    }

    myBoard->PrintBoard();

    EnemyBoard* enemyBoard = new EnemyBoard(7);

    std::cout <<  "Starting 1 move" << std::endl;
    enemyBoard->StartAttacking(false);
    std::cout <<  "Starting 2 move" << std::endl;
    enemyBoard->StartAttacking(false);

    enemyBoard->PrintBoard();
    delete enemyBoard;
    delete myBoard;

    return 0;
}
