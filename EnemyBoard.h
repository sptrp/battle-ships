//
// Created by Sptrp on 24.04.21.
//

#ifndef BATTLE_SHIPS_ENEMYBOARD_H
#define BATTLE_SHIPS_ENEMYBOARD_H

#include <vector>
#include <array>

class EnemyBoard {

private:
    std::vector< std::vector<bool> > board;
    std::vector< std::vector<int> > blacklist;
    std::vector<int> coordinateCash;
    bool rotation;
    bool shipDestroyed;

    std::vector<int> RandomizeCoordinate();

    void ContinueAttacking(int col, int row);

    int AttackColumn(int col, int row, bool downwards);

    int AttackRow(int col, int row, bool forwards);

    bool AttackField(int col, int row);

    bool IsInBlacklist(int col, int row);

    bool IsWithinGrid(int rowNum, int colNum);

public:
    bool goForth;
    int shipCounter;

    explicit EnemyBoard(int multiplier);

    ~EnemyBoard();

    void StartAttacking(bool continueAttack);

    void PrintBoard();
};

#endif //BATTLE_SHIPS_ENEMYBOARD_H
