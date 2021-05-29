#include <iostream>
#include "Board.h"
#include "EnemyBoard.h"

int ComputerTurn(Board *myBoard);

int main() {

    int enemyTurn = -1;
    Board *computerBoard = new Board(7);
    EnemyBoard *enemyBoard = new EnemyBoard(7);

    std::cout << "Hello, Battle Ships!" << std::endl;

    try {
        computerBoard->RandomizeShips();

    } catch (std::exception &e) {

        std::cout << e.what() << std::endl;
    }

    computerBoard->PrintBoard();

    while (enemyBoard->shipCounter < 6) {
        // Enemy turn
        while (enemyTurn != 0) {

            enemyTurn = ComputerTurn(computerBoard);

            if (enemyTurn == 2) {

                enemyBoard->PrintBoard();

                delete enemyBoard;
                delete computerBoard;

                return 0;
            }
        }

        // Computer turn
        std::cout << "My turn" << std::endl;

        if (!enemyBoard->goForth) {

            enemyBoard->StartAttacking(false);
        } else {

            enemyBoard->StartAttacking(true);
        }

        // Reset enemy turn
        enemyTurn = -1;
    }

    enemyBoard->PrintBoard();

    delete enemyBoard;
    delete computerBoard;

    return 0;

}

int ComputerTurn(Board *myBoard) {
    int attackEnemy;
    int shipCounter = 0;

    while (shipCounter < 6) {
        attackEnemy = myBoard->AttackField();

        switch (attackEnemy) {

            case 2:
                std::cout << "Bulls eye!" << std::endl;
                std::cout << "Ship sank!" << std::endl;
                shipCounter++;
                std::cout << shipCounter << std::flush;
                std::cout << " ships sank" << std::endl;
                if (shipCounter == 6)
                    return 2;

                break;
            case 1:
                std::cout << "Bulls eye!" << std::endl;
                break;
            case 0:
                std::cout << "Sorry missed" << std::endl;
                return 0;
        }
    }
}
