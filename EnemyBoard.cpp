//
// Created by Sptrp on 24.04.21.
//

#include "EnemyBoard.h"
#include <array>
#include <iostream>

/**
 * Play board constructor
 * @param multiplier the board will be created with multiplier x multiplier size
 */
EnemyBoard::EnemyBoard(const int multiplier) {

    board = std::vector< std::vector<bool> >(
            multiplier,std::vector<bool>(multiplier, false));

    shipCounter = 0;
    shipDestroyed = false;
}

/**
 * Play board destructor
 */
EnemyBoard::~EnemyBoard() {

    // Reset random generator to produce pseudorandom values
    // rand() will return same result every time without resetting
    srand ( time(nullptr) );

    for (int x = 0; x < board.size(); x++) {
        // Use swap() to make sure memory was cleared from vectors
        std::vector<bool>().swap(board[x]);
    }
    std::vector< std::vector<bool> >().swap(board);
    std::vector< std::array<int, 2> >().swap(blacklist);
}

/**
 * Check if field is in blacklist
 * @param col
 * @param row
 * @return true if yes, otherwise false
 */
bool EnemyBoard::IsInBlacklist(int col, int row) {

    if (!blacklist.empty()) {
        // Iterate through blacklist and check if coordinate already in there
        for (int ptr = 0; ptr < blacklist.size(); ptr++) {
            int blCol = blacklist[ptr][0];

            // Check first vector 1 (columns)
            if (col == blCol) {
                int blRow = blacklist[ptr][1];

                // Check row only if column match for shorter runtime
                if (row == blRow)
                    return true;
            }
        }
    }
    return false;
}

/**
 *
 * @param continueAttack true if the attack should be continued
 */
void EnemyBoard::StartAttack(bool continueAttack) {

    if (!continueAttack) {
        std::cout <<  "Starting attack" << std::endl;
        std::array<int, 2> newCoord = RandomizeCoordinate();

        bool hit = AttackField(newCoord[0], newCoord[1]);

        std::cout << newCoord[0] << std::flush;
        std::cout << " : " << std::flush;
        std::cout << newCoord[1] << std::endl;

        // Continue attacking if not ship destroyed
        if (hit) {
            std::cout <<  "Hit but not  destroyed!" << std::endl;
            coordinateCash = newCoord;
            shipDestroyed = false;
            blacklist.push_back(newCoord);
            StartAttack(true);

        // Exit if attack failed
        } else {
            std::cout <<  "Missed!" << std::endl;
            // TODO exit attack
            shipDestroyed = false;
            blacklist.push_back(newCoord);
            return;
        }

    } else {

        ContinueAttack();
    }
}


void EnemyBoard::ContinueAttack() {

    std::cout <<  "Continue attack" << std::endl;
    std::array<int, 2> lastCoord = coordinateCash;
    int rotation = 1;

    // If vertical
    if (rotation == 1) {

        int row = lastCoord[1] + 1;

        while (row >= 0) {
            // Check if within grid and if field is in blacklist and start attacking next fields
            if (IsWithinGrid(lastCoord[0], row) && !IsInBlacklist(lastCoord[0], row)) {
                std::array<int, 2> coord = {lastCoord[0], row};

                int hit = AttackField(lastCoord[0], row);

                if (!shipDestroyed  && hit)  {

                    blacklist.push_back(coord);
                    row++;

                } else if (shipDestroyed  && hit) {
                    row = -1;
                    StartAttack(false);
                }
            // If something is bad, move backwards
            } else {

                row = lastCoord[1] - 1;

                while (row >= 0) {
                    std::array<int, 2> coord = {lastCoord[0], row};

                    while (!shipDestroyed && AttackField(lastCoord[0], row)) {

                        blacklist.push_back(coord);
                        row -= 1;
                    }
                    row = -1;
                }
            }
        }

     // If vertical
    } else {

    }
}

bool EnemyBoard::AttackField(int col, int row) {

    if (!IsInBlacklist(col, row)) {
        // TODO Send hit on NAO
        std::cout << col << std::flush;
        std::cout << " : " << std::flush;
        std::cout << row << std::endl;

        int shipHit;
        std::cout << "Ship met? : ";
        std::cin >> shipHit;


        if (shipHit == 1) {
            // Set on hit
            std::cout << "Hit!" << std::endl;
            board[col][row] = true;

            int destroyed;
            std::cout << "Ship destroyed? : ";
            std::cin >> destroyed;

            if (destroyed == 1) {
                std::cout << "Ship destroyed!" << std::endl;
                shipDestroyed = true;
            }

            return true;
        }
    }
    return false;
}

/**
 * Check if field is still in bounds of play board
 * @param col
 * @param row
 * @return true if yes, otherwise false
 */
bool EnemyBoard::IsWithinGrid(int col, int row) {
    // Is false if out of bounds
    if ((col < 0) || (row < 0) || (col > 6) || (row > 6)) {

        return false;
    }
    return true;
}

/**
 * Create random coordinate
 * @return coordinate
 */
std::array<int, 2> EnemyBoard::RandomizeCoordinate() {

    std::array<int, 2> coord = {rand() % 7, rand() % 7};

    return coord;
}

/**
 * Print out current enemy board
 */
void EnemyBoard::PrintBoard() {

    std::cout <<  "!! ENEMY BOARD !!" << std::endl;

    for (int row = 0; row < board.size(); row++) {
        for (int col = 0; col < board.size(); col++) {

            if (col != board.size() - 1) {
                // Print values 0 - 6 of each row
                GetFieldValue(col, row) ? std::cout << " X " << std::flush : std::cout << " _ " << std::flush;
            } else {
                // Enter if 7
                GetFieldValue(col, row) ? std::cout << " X " << std::endl : std::cout << " _ " << std::endl;
            }
        }
    }
}

/**
 * Get field status
 * @param col
 * @param row
 * @return true if ship placed on this field, otherwise false
 */
bool EnemyBoard::GetFieldValue(int col, int row) {

    return board[col][row];
}
