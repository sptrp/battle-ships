
#ifndef BATTLE_SHIPS_ENEMYBOARD_H
#define BATTLE_SHIPS_ENEMYBOARD_H

#include <vector>

class EnemyBoard {

private:
    std::vector< std::vector<bool> > board;

    std::vector<int> coordinateCache;
    bool rotation;






    int AttackColumn(int col, int row, bool downwards);

    void AttackRow(int col, int row, bool forwards);

    bool AttackField();
    void AttackField2();





public:
    bool IsInBlacklist(int col, int row);
    bool shipDestroyed;
    bool IsWithinGrid(int rowNum, int colNum);
    bool goForth;
    int shipCounter;
    void StartAttacking(bool continueAttack);
    explicit EnemyBoard(int multiplier);
    void RandomizeCoordinate();
    void ContinueAttacking();
    std::vector< std::vector<int> > blacklist;
    ~EnemyBoard();


    void AttackWithCoordinates();
    void PrintBoard();
};

#endif //BATTLE_SHIPS_ENEMYBOARD_H
