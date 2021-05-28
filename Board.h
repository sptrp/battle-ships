//
// Created by Sptrp on 17.04.21.
//

#ifndef BATTLE_SHIPS_BOARD_H
#define BATTLE_SHIPS_BOARD_H

#include <vector>
#include <map>

class Board {

private:
    std::vector< std::vector<bool> > board;
    std::vector< std::array<int, 2> > blacklist;
    std::map< std::array<int, 2>, int > shipStorage;
    std::map<int, int> shipMap;
    std::vector<int> sankShips;

public:
    int shipCounter;

    Board(int multiplier);

    ~Board();

    void RandomizeShips();

    std::array<int, 2> RandomizeCoordinate();

    void PlaceVarFieldsShip(int size);

    void StoreFieldInBlackList(int y, int x);

    bool IsWithinGrid(int rowNum, int colNum);

    bool IsInBlacklist(int col, int row);

    int CountFreeCols(int *row);

    int CountFreeRows(int *col);

    bool IsSlotFree(int col, int row, int size, int rotation);

    void OccupyField(int col, int row);

    int AttackField();

    void PutInStorage(int col, int row, int size);

    void PutInMap(int size);

    bool IsShipSunk(int const *size);

    void PrintBoard();
};


#endif //BATTLE_SHIPS_BOARD_H
