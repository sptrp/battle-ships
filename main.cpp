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

    while (myBoard->shipCounter < 6 && enemyBoard->shipCounter < 6) {

        while (myBoard->AttackField()) {

            std::cout <<  "Bulls eye!" << std::endl;
        }

        std::cout <<  "Sorry missed" << std::endl;
        std::cout <<  "My turn" << std::endl;

        if (!enemyBoard->goForth) {

            enemyBoard->StartAttacking(false);
        } else {

            enemyBoard->StartAttacking(true);
        }

    }
    enemyBoard->PrintBoard();

    delete enemyBoard;
    delete myBoard;

    return 0;
}
