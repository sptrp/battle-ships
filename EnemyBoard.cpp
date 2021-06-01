//
// Created by Tema Bender on 24.04.21.
//

#include "enemyboard.h"
#include <iostream>
#include <cstdlib>
#include <qi/log.hpp>

const char* moduleName3 = "EnemyBoard";
/**
 * Play board constructor
 * @param multiplier the board will be created with multiplier x multiplier size
 */
EnemyBoard::EnemyBoard(const int multiplier) {

    board = std::vector< std::vector<bool> >(
            multiplier,std::vector<bool>(multiplier, false));

    // Flags
    shipCounter = 0;
    shipDestroyed = false;
    goForth = false;
    rotation = false;
}

/**
 * Play board destructor
 */
EnemyBoard::~EnemyBoard() {

    for (int x = 0; x < board.size(); x++) {
        // Use swap() to make sure memory was cleared from vectors
        std::vector<bool>().swap(board[x]);
    }
    std::vector< std::vector<bool> >().swap(board);
    std::vector< std::vector<int> >().swap(blacklist);
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
 * Start new attack
 * @param continueAttack true if the attack should be continued
 */
void EnemyBoard::StartAttacking(bool continueAttack) {
    qiLogInfo(moduleName3) << shipCounter << std::flush;
    qiLogInfo(moduleName3) << " ships destroyed!" << std::endl;
    if (shipCounter > 5) { return; }

    // Start new attack if continue flag is false
    if (!continueAttack) {
        qiLogInfo(moduleName3) <<  "Starting new attack" << std::endl;
        std::vector<int> newCoord = RandomizeCoordinate();

        if (IsInBlacklist(newCoord[0], newCoord[1])) {

            StartAttacking(false);
        } else {

            bool hit = AttackField(newCoord[0], newCoord[1]);

            if (shipDestroyed) {

                shipCounter++;
                // Escape attacking program if ship limit reached TODO: change to 6
                if (shipCounter > 5) { return; }

                qiLogInfo(moduleName3) << shipCounter << std::flush;
                qiLogInfo(moduleName3) << " ships destroyed!" << std::endl;

                // Otherwise start new attack
                StartAttacking(false);
                return;
            }
            // If ship met, save field in cash and continue attack from this point
            if (hit) {

                coordinateCash = newCoord;
                StartAttacking(true);
                return;
            } else {
                // If miss, return
                return;
            }

        }
        // Otherwise continue attack from last saved point
    } else {

        ContinueAttacking(coordinateCash[0], coordinateCash[1]);
    }
}

/**
 * Try to attack field and set goForth if point need to be saved
 * @param col
 * @param row
 * @return
 */
bool EnemyBoard::AttackField(int col, int row) {

    std::vector<int> coord;
    coord.push_back(col);
    coord.push_back(row);

    // TODO Send hit on NAO
    if (!IsInBlacklist(col, row)) {

        qiLogInfo(moduleName3) << col << std::flush;
        qiLogInfo(moduleName3) << " : " << std::flush;
        qiLogInfo(moduleName3) << row << std::endl;

        // TODO: input for hit result
        int shipMet = 0;
        qiLogInfo(moduleName3) << "Ship met? : ";
        //std::cin >> shipMet;

        if (shipMet == 1) {
            // TODO: input if ship met
            qiLogInfo(moduleName3) << "Hit!" << std::endl;
            board[col][row] = true;

            // TODO: input if ship destroyed
            int destroyed = 0;
            qiLogInfo(moduleName3) << "Ship destroyed? : ";
           // std::cin >> destroyed;

            if (destroyed == 1) {

                goForth = false;
                shipDestroyed = true;
                blacklist.push_back(coord);
                return true;
            }
            qiLogInfo(moduleName3) <<  "Hit but not  destroyed!" << std::endl;
            goForth = true;
            shipDestroyed = false;
            blacklist.push_back(coord);
            return true;
        }
        qiLogInfo(moduleName3) <<  "Missed!" << std::endl;
        qiLogInfo(moduleName3) <<  "Miss from Attack" << std::endl;
        shipDestroyed = false;
        blacklist.push_back(coord);
        return false;
    }
    return false;
}

/**
 * Continue attack from last saved point
 * Current rotation will be checked
 * If Rotation true then go vertical, otherwise horizontal
 */
void EnemyBoard::ContinueAttacking(int col, int row) {

    int attack;

    qiLogInfo(moduleName3) <<  "Continue attack from: " << std::flush;
    qiLogInfo(moduleName3) << coordinateCash[0] << std::flush;
    qiLogInfo(moduleName3) << " : " << std::flush;
    qiLogInfo(moduleName3) << coordinateCash[1] << std::endl;

    // Change column if horizontal, otherwise change row
    attack = !rotation ? AttackRow(col + 1, row, true) : AttackColumn(col, row + 1, true);

    switch (attack) {
        // Ship destroyed - start new attack
        case 3:
            shipCounter++;
            StartAttacking(false);
            // Miss - jump out
        case 2:
            return;
            // Out of bounds or blacklist - try other direction
        case 1:
            // Change column reversed if horizontal, otherwise change row reversed
            attack = !rotation ? AttackRow(col - 1, row, false) : AttackColumn(col, row - 1, false);

            switch (attack) {
                // Ship destroyed
                case 3:
                    shipCounter++;
                    StartAttacking(false);
                    // Miss
                case 2:
                    return;
                    // Out of bounds or blacklist - try again other rotation
                case 1:
                    rotation = !rotation;
                    ContinueAttacking(col, row);

                default: return;
            }

        default: return;
    }
}

/**
 * Attacking column, so long the ship not destroyed or reached end of playing area
 * @param col
 * @param row
 * @param downwards to switch directions
 * @return 3 if ship destroyed, 2 if attack missed, 1 if out of bounds or field is blacklisted
 */
int EnemyBoard::AttackColumn(int col, int row, bool downwards) {
    qiLogInfo(moduleName3) <<  "Starting attack at column" << std::endl;
    while (row >= 0) {

        // Check if within grid and if field is in blacklist and start attacking next fields
        if (IsWithinGrid(col, row) && !IsInBlacklist(col, row)) {
            std::vector<int> coord;
            coord.push_back(col);
            coord.push_back(row);

            qiLogInfo(moduleName3) << "Attacking :" << std::endl;
            qiLogInfo(moduleName3) << col << std::flush;
            qiLogInfo(moduleName3) << " : " << std::flush;
            qiLogInfo(moduleName3) << row << std::endl;

            int hit = AttackField(col, row);

            if (!shipDestroyed && hit) {

                blacklist.push_back(coord);
                if (downwards) {
                    row++;
                } else {
                    row--;
                }

            } else if (shipDestroyed && hit) {
                // Start new attack and exit the loop if ship destroyed
                return 3; // Start new Attack if ship destroyed

            } else if (!hit) {
                qiLogInfo(moduleName3) <<  "Miss from AttackColumn" << std::endl;
                return 2;
            }

        } else {
            qiLogInfo(moduleName3) <<  "AttackColumn Not within Grid os in blacklist" << std::endl;
            return 1;
        }
    }
}

/**
 * Attacking row, so long the ship not destroyed or reached end of playing area
 * @param col
 * @param row
 * @param forwards to switch directions
 * @return 3 if ship destroyed, 2 if attack missed, 1 if out of bounds or field is blacklisted
 */
int EnemyBoard::AttackRow(int col, int row, bool forwards) {
    qiLogInfo(moduleName3) <<  "Starting attack at row" << std::endl;
    while (col >= 0) {

        // Check if within grid and if field is in blacklist and start attacking next fields
        if (IsWithinGrid(col, row) && !IsInBlacklist(col, row)) {
            std::vector<int> coord;
            coord.push_back(col);
            coord.push_back(row);

            qiLogInfo(moduleName3) << "Attacking :" << std::endl;
            qiLogInfo(moduleName3) << col << std::flush;
            qiLogInfo(moduleName3) << " : " << std::flush;
            qiLogInfo(moduleName3) << row << std::endl;

            int hit = AttackField(col, row);

            if (!shipDestroyed && hit) {

                blacklist.push_back(coord);

                if (forwards) {
                    col++;
                } else {
                    col--;
                }

            } else if (shipDestroyed && hit) {
                // Start new attack and exit the loop if ship destroyed
                return 3; // Start new Attack if ship destroyed

            } else if (!hit) {
                qiLogInfo(moduleName3) <<  "Miss from AttackRow" << std::endl;
                return 2;
            }
        } else {
            qiLogInfo(moduleName3) <<  "AttackRow Not within Grid or in blacklist" << std::endl;
            return 1;
        }
    }
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
std::vector<int> EnemyBoard::RandomizeCoordinate() {

    std::vector<int> coord;
    coord.push_back(std::rand() % 7);
    coord.push_back(std::rand() % 7);

    return coord;
}

/**
 * Print out current enemy board
 */
void EnemyBoard::PrintBoard() {

    qiLogInfo(moduleName3) <<  "!! ENEMY BOARD !!" << std::endl;

    for (int row = 0; row < board.size(); row++) {
        for (int col = 0; col < board.size(); col++) {

            if (col != board.size() - 1) {
                // Print values 0 - 6 of each row
                board[col][row] ? qiLogInfo(moduleName3) << " X " << std::flush : qiLogInfo(moduleName3) << " _ " << std::flush;
            } else {
                // New line if 7
                board[col][row] ? qiLogInfo(moduleName3) << " X " << std::endl : qiLogInfo(moduleName3) << " _ " << std::endl;
            }
        }
    }
}
