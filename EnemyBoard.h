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
    std::vector< std::array<int, 2> > blacklist;
    std::array<int, 2> coordinateCash;

    int shipCounter;
    bool shipDestroyed;

public:
    explicit EnemyBoard(int multiplier);

    ~EnemyBoard();

    std::array<int, 2> RandomizeCoordinate();

    void StartAttacking(bool continueAttack);

    void ContinueAttacking();

    void AttackDownwards(int col, int row);

    void AttackUpwards(int col, int row);

    void AttackForwards(int col, int row);

    void AttackBackwards(int col, int row);

    bool AttackField(int col, int row);

    void StoreFieldInBlackList(int col, int row);

    bool IsInBlacklist(int col, int row);

    bool IsWithinGrid(int rowNum, int colNum);

    void PrintBoard();
};

#endif //BATTLE_SHIPS_ENEMYBOARD_H
