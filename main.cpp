#include <iostream>
#include "Board.h"
#include "EnemyBoard.h"

int main() {

    int attackEnemy;

    std::cout << "Hello, Battle Ships!" << std::endl;

    Board* myBoard = new Board(7);

    try {
        myBoard->RandomizeShips();

    } catch (std::exception& e) {

        std::cout << e.what() << std::endl;
    }

    myBoard->PrintBoard();

    EnemyBoard* enemyBoard = new EnemyBoard(7);

    // TODO: counter to 6
    while (myBoard->shipCounter < 6 && enemyBoard->shipCounter < 4) {

        while (attackEnemy > 0) {
            attackEnemy = myBoard->AttackField();

            switch (attackEnemy) {

                case 2:
                    std::cout <<  "Bulls eye!" << std::endl;
                    std::cout <<  "Ship sunk!" << std::endl;
                    break;
                case 1:
                    std::cout <<  "Bulls eye!" << std::endl;
                    break;
                case 0:
                    std::cout <<  "Sorry missed" << std::endl;
                    break;
            }
        }

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
