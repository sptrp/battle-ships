//
// Created by Tema Bender on 24.04.21.
//

#include "enemyboard.h"
#include "global.h"
#include <iostream>
#include <cstdlib>
#include <qi/log.hpp>

using namespace glb;

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
 * Check if field is in blacklist because it was shot already
 * @param col
 * @param row
 * @return true if yes, otherwise false
 */
bool EnemyBoard::IsInBlacklist(int col, int row) {
  //  qiLogInfo(moduleName) << "Blacklist check." << std::endl;
    if (!blacklist.empty()) {

        //qiLogInfo(moduleName) << "Blacklist not empty." << std::endl;

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
    //qiLogInfo(moduleName) << "Blacklist empty." << std::endl;
    return false;
}

/**
 * Start new attack
 * @param continueAttack true if the attack should be continued
 */
void EnemyBoard::StartAttacking(bool continueAttack) {
    qiLogInfo(moduleName) << "StartAttacking gestartet." << std::endl;
    if (getPlayerShipCounter() > 5) { return; }

    // Start new attack if continue flag is false
    if (!continueAttack) {

        bool hit = AttackField();
        qiLogInfo(moduleName) << "Printing hit: " << hit << std::endl;

        if (shipDestroyed) {
            incrementPlayerShipCounter();
            qiLogInfo(moduleName) << "Schiff zerstoert. Anzahl versunkener Schiffe: " << getPlayerShipCounter() << std::endl;
            return;
        }
        // If ship met, save field in cache and continue attack from this point
/**
        if (hit) {
            coordinateCache.push_back(getNaoAttackCol());
            coordinateCache.push_back(getNaoAttackRow());
            return;
        } else {
            // If miss, return
            return;
        }
*/

        // Otherwise continue attack from last saved point
    } else {

        ContinueAttacking();
    }
}


/**
 * Start new attack
 * @param continueAttack true if the attack should be continued
 */

/**
int EnemyBoard::StartAttacking(bool continueAttack) {
    qiLogInfo(moduleName) << shipCounter << " ships destroyed!"<< std::endl;
    if (shipCounter > 5) { return -1; }

    // Start new attack if continue flag is false
    if (!continueAttack) {
        qiLogInfo(moduleName) <<  "Starting new attack" << std::endl;
        RandomizeCoordinate();

        if (IsInBlacklist(getNaoAttackCol(), getNaoAttackRow())) {
            return 2;
            //StartAttacking(false);
        } else {
            return 1;

        }
        // Otherwise continue attack from last saved point
    } else {
        ContinueAttacking(coordinateCache[0], coordinateCache[1]);
        return 0;
    }
}


void EnemyBoard::AttackWithCoordinates() {          //need parameter
    bool shipDestroyed = false;                                                                                     //NEED CHANGES !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    setShipMet(true);
    qiLogInfo(moduleName) <<  "AttackCord function started." << std::endl;
    if (shipDestroyed) {

        incrementPlayerShipCounter();
        // Escape attacking program if ship limit reached TODO: change to 6
        if (getPlayerShipCounter() > 5) { return; }

        qiLogInfo(moduleName) << getPlayerShipCounter() << std::flush;
        qiLogInfo(moduleName) << " ships destroyed!" << std::endl;

        // Otherwise start new attack
        StartAttacking(false);
        return;
    }
    // If ship met, save field in cache and continue attack from this point
    if (getShipMet()) {
        qiLogInfo(moduleName) << "in Shipmet true teil." << std::endl;

        coordinateCache[0] = getNaoAttackCol();
        coordinateCache[1] = getNaoAttackRow();
        StartAttacking(true);
    } else {
        // If miss, return
        //return;
    }
}

*/
/**
 * Try to attack field and set goForth if point need to be saved
 * @param col
 * @param row
 * @return
 */
bool EnemyBoard::AttackField() {

    std::vector<int> coord;
    coord.push_back(getNaoAttackCol());
    coord.push_back(getNaoAttackRow());

    if (!IsInBlacklist(getNaoAttackCol(), getNaoAttackRow())) {

        if (getShipMet() == 1) {
            board[getNaoAttackCol()][getNaoAttackRow()] = true;
            if (getDestroyed() == 1) {

                goForth = false;
                shipDestroyed = true;
                blacklist.push_back(coord);
                return true;
            }
            qiLogInfo(moduleName) << "Hit but not  destroyed!" << std::endl;
            goForth = true;
            shipDestroyed = false;
            blacklist.push_back(coord);
            return true;
        }
        qiLogInfo(moduleName) <<   "Missed!" << std::endl;
        qiLogInfo(moduleName) <<   "Miss from Attack" << std::endl;
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
void EnemyBoard::ContinueAttacking() {

    //int attack;

    qiLogInfo(moduleName) <<  "Continue attack function started." << std::endl;
 /**
    // Change column if horizontal, otherwise change row
    qiLogInfo(moduleName) <<  "Trying attack row" << std::endl;


    if (IsWithinGrid(col, row) && !IsInBlacklist(col, row)) {

    }
    if (!rotation) {
        AttackRow(col + 1, row, true);
    }
    else {
        AttackColumn(col, row + 1, true);
    }


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
    */
}

/**
 * Attacking column, so long as the ship was not destroyed or reached end of playing area
 * @param col
 * @param row
 * @param downwards to switch directions
 * @return 3 if ship destroyed, 2 if attack missed, 1 if out of bounds or field is blacklisted
 */
int EnemyBoard::AttackColumn(int col, int row, bool downwards) {
    qiLogInfo(moduleName) <<  "Starting attack at column" << std::endl;
    while (row >= 0) {

        // Check if within grid and if field is in blacklist and start attacking next fields
        if (IsWithinGrid(col, row) && !IsInBlacklist(col, row)) {
            std::vector<int> coord;
            coord.push_back(col);
            coord.push_back(row);

            qiLogInfo(moduleName) << "Attacking :" << std::endl;
            qiLogInfo(moduleName) << col << std::flush;
            qiLogInfo(moduleName) << " : " << std::flush;
            qiLogInfo(moduleName) << row << std::endl;

            //int hit = AttackField(col, row);
            AttackField();
            if (!shipDestroyed && getShipMet()) {

                blacklist.push_back(coord);
                if (downwards) {
                    row++;
                } else {
                    row--;
                }

            } else if (shipDestroyed && getShipMet()) {
                // Start new attack and exit the loop if ship destroyed
                return 3; // Start new Attack if ship destroyed

            } else if (!getShipMet()) {
                qiLogInfo(moduleName) <<  "Miss from AttackColumn" << std::endl;
                return 2;
            }

        } else {
            qiLogInfo(moduleName) <<  "AttackColumn Not within Grid os in blacklist" << std::endl;
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
void EnemyBoard::AttackRow(int col, int row, bool forwards) {
    qiLogInfo(moduleName) <<  "Starting attack at row" << std::endl;
    //while (col >= 0) {

        // Check if within grid and if field is in blacklist and start attacking next fields
        if (IsWithinGrid(col, row) && !IsInBlacklist(col, row)) {
            std::vector<int> coord;
            coord.push_back(col);
            coord.push_back(row);
            setNaoAttackCol(col);
            setNaoAttackRow(row);
            qiLogInfo(moduleName) << "Attacking: " << col << ":" << row << std::endl;

            //bool hit = AttackField();
            /**
            if (!shipDestroyed && getShipMet()) {

                blacklist.push_back(coord);

                if (forwards) {
                    col++;
                } else {
                    col--;
                }

            } else if (shipDestroyed && getShipMet()) {
                // Start new attack and exit the loop if ship destroyed
                return 3; // Start new Attack if ship destroyed

            } else if (!getShipMet()) {
                qiLogInfo(moduleName) <<  "Miss from AttackRow" << std::endl;
                return 2;
            } */
        } else {
            qiLogInfo(moduleName) <<  "AttackRow Not within Grid or in blacklist" << std::endl;
            return AttackRow(col, row, false);
        }
    //}
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
 * Create random attack coordinate
 * @return coordinate
 */
void EnemyBoard::RandomizeCoordinate() {

    setNaoAttackCol(std::rand() % 7);
    setNaoAttackRow(std::rand() % 7);
    while (IsInBlacklist(getNaoAttackCol(), getNaoAttackRow())) {
        setNaoAttackCol(std::rand() % 7);
        setNaoAttackRow(std::rand() % 7);
    }
    qiLogInfo(moduleName) << "attacking coordinates found" << std::endl;
}

/**
 * Print out current enemy board
 */
void EnemyBoard::PrintBoard() {

    qiLogInfo(moduleName) <<  "!! ENEMY BOARD !!" << std::endl;

    for (int row = 0; row < board.size(); row++) {
        for (int col = 0; col < board.size(); col++) {

            if (col != board.size() - 1) {
                // Print values 0 - 6 of each row
                board[col][row] ? qiLogInfo(moduleName) << " X " << std::flush : qiLogInfo(moduleName) << " _ " << std::flush;
            } else {
                // New line if 7
                board[col][row] ? qiLogInfo(moduleName) << " X " << std::endl : qiLogInfo(moduleName) << " _ " << std::endl;
            }
        }
    }
}
