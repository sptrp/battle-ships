//
// Created by Sptrp on 17.04.21.
//

#ifndef BATTLE_SHIPS_BOARD_H
#define BATTLE_SHIPS_BOARD_H

#include <vector>

class Board {

private:
    std::vector< std::vector<bool> > board;
    std::vector< std::array<int, 2> > blacklist;

public:

    Board(int multiplier);

    ~Board();

    void RandomizeShips();

    std::array<int, 2> RandomizeCoordinate();

    void PlaceVarFieldsShip(int size);

    void StoreFieldInBlackList(int y, int x);

    bool GetFieldValue(int col, int row);

    bool IsWithinGrid(int rowNum, int colNum);

    bool IsInBlacklist(int col, int row);

    int CountFreeCols(int row);

    int CountFreeRows(int col);

    bool IsSlotFree(int col, int row, int size, int rotation);

    void OccupyField(int col, int row);

    void PrintBoard();
};


#endif //BATTLE_SHIPS_BOARD_H
