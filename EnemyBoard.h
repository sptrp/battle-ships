
#ifndef BATTLE_SHIPS_ENEMYBOARD_H
#define BATTLE_SHIPS_ENEMYBOARD_H

#include <vector>

class EnemyBoard {

private:
    std::vector< std::vector<bool> > board;
    std::vector< std::vector<int> > blacklist;
    std::vector<int> coordinateCache;
    bool rotation;
    bool shipDestroyed;



    void ContinueAttacking(int col, int row);

    int AttackColumn(int col, int row, bool downwards);

    int AttackRow(int col, int row, bool forwards);

    bool AttackField();
    void AttackField2();



    bool IsInBlacklist(int col, int row);

    bool IsWithinGrid(int rowNum, int colNum);

public:
    bool goForth;
    int shipCounter;
    void StartAttacking(bool continueAttack);
    explicit EnemyBoard(int multiplier);
    void RandomizeCoordinate();

    ~EnemyBoard();


    void AttackWithCoordinates();
    void PrintBoard();
};

#endif //BATTLE_SHIPS_ENEMYBOARD_H
