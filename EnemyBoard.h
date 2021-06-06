/**
 @file enemyboard.h
 @authors Ivan Ponomarev, Stefan Holzbauer
 @date 17.04.2021
 @brief Header-File
*/
#ifndef BATTLE_SHIPS_ENEMYBOARD_H
#define BATTLE_SHIPS_ENEMYBOARD_H

#include <vector>

class EnemyBoard {

private:
    // Variables
    /**
     * the board which stores the area for the ships
     */
    std::vector< std::vector<bool> > board;

    //Methods
    /**
     * Try to attack field and set goForth if point need to be saved
     * @return true if hit, false if missed
     */
    bool AttackField();

public:
    // Variables
    /**
     * bool variable which is true once ship is destroyed
     */
    bool shipDestroyed;
    /**
     * 2 dimensional vector which stores the already attacked or otherwise forbidden attack coordinates
     */
    std::vector< std::vector<int> > blacklist;

    // Methods
    /**
     * Play board constructor
     * @param multiplier the board will be created with multiplier x multiplier size
     */
    explicit EnemyBoard(int multiplier);
    /**
     * Create and sets random attack coordinate
     */
    void RandomizeCoordinate();
    /**
     * Start new attack
     */
    void StartAttacking();
    /**
     * Check if field is in blacklist because it was shot already
     * @param col
     * @param row
     * @return true if yes, otherwise false
     */
    bool IsInBlacklist(int col, int row);
    /**
     * Check if field is still in bounds of play board
     * @param col
     * @param row
     * @return true if yes, otherwise false
     */
    bool IsWithinGrid(int rowNum, int colNum);
    /**
     * Play board destructor
     */
    ~EnemyBoard();

};

#endif //BATTLE_SHIPS_ENEMYBOARD_H
