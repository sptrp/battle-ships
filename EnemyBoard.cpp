/**
 @file enemyboard.cpp
 @authors Ivan Ponomarev, Stefan Holzbauer
 @date 17.04.2021
 @brief Implementation-File
*/

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

    // Flag
    shipDestroyed = false;
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
 */
void EnemyBoard::StartAttacking() {
    qiLogInfo(moduleName) << "StartAttacking gestartet." << std::endl;
    if (getPlayerShipCounter() > 5) { return; }

    // Start new attack if continue flag is false
    bool hit = AttackField();
    qiLogInfo(moduleName) << "Printing hit: " << hit << std::endl;

    if (shipDestroyed) {
        incrementPlayerShipCounter();
        qiLogInfo(moduleName) << "Schiff zerstoert. Anzahl versunkener Schiffe: " << getPlayerShipCounter() << std::endl;
        return;
    }
}

/**
 * Try to attack field and set goForth if point need to be saved
 * @return true if hit, false if missed
 */
bool EnemyBoard::AttackField() {

    std::vector<int> coord;
    coord.push_back(getNaoAttackCol());
    coord.push_back(getNaoAttackRow());

    if (!IsInBlacklist(getNaoAttackCol(), getNaoAttackRow())) {

        if (getShipMet() == 1) {
            board[getNaoAttackCol()][getNaoAttackRow()] = true;
            if (getDestroyed() == 1) {

                shipDestroyed = true;
                blacklist.push_back(coord);
                return true;
            }
            qiLogInfo(moduleName) << "Hit but not  destroyed!" << std::endl;
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
 * Create and sets random attack coordinate
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
