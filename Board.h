/**
 @file board.h
 @authors Ivan Ponomarev, Stefan Holzbauer
 @date 17.04.2021
 @brief Header-File
*/

#ifndef BATTLE_SHIPS_BOARD_H
#define BATTLE_SHIPS_BOARD_H

#include <vector>
#include <map>


class Board {

private:
    // Variables
    /**
     *  the Board in which NAO has its ships
     */
    std::vector< std::vector<bool> > board;
    /**
     * the blacklist which contains the ships coordinates
     */
    std::vector< std::vector<int> > blacklist;
    /**
     * connects ship coordinates to a single ship
     */
    std::map< std::vector<int>, int > shipStorage;
    /**
     * number of fields of concrete size
     */
    std::map<int, int> shipMap;
    /**
     * sank ships
     */
    std::vector<int> sankShips;

    // Methods
    /**
     * Create random coordinate
     * @return coordinate
     */
    std::vector<int> RandomizeCoordinate();
    /**
     * Randomly place n-size ship on play board
     * @param size
     */
    void PlaceVarFieldsShip(int size);
    /**
     * Place given field and all fields surrounding it in blacklist
     * @param y row
     * @param x col
     */
    void StoreFieldInBlackList(int y, int x);
    /**
     * Check if field is still in bounds of play board
     * @param col
     * @param row
     * @return true if yes, otherwise false
     */
    bool IsWithinGrid(int rowNum, int colNum);
    /**
     * Check if field is in blacklist
     * @param col
     * @param row
     * @return true if yes, otherwise false
     */
    bool IsInBlacklist(int col, int row);
    /**
     * Count all free columns in a row
     * @param row
     * @return a number of free columns
     */
    int CountFreeCols(int *row);
    /**
     * Count all free rows in a column
     * @param col
     * @return a number of free rows
     */
    int CountFreeRows(int *col);
    /**
     * Check if next slot for n-size ship is free
     * @param col column of ship
     * @param row row of ship
     * @param size length of ship
     * @param rotation horizontal (0) or vertical (1)
     * @return true if free, otherwise false
     */
    bool IsSlotFree(int col, int row, int size, int rotation);
    /**
     * Occupy ship field and place in blacklist
     * @param col
     * @param row
     */
    void OccupyField(int col, int row);
    /**
     * Assign every field to ship size
     * @param col column of ship
     * @param row row of ship
     * @param size length of ship
     */
    void PutInStorage(int col, int row, int size);
    /**
     * Assign total number of fields to every size
     * @param size
     */
    void PutInMap(int size);
    /**
     * Check if ship sank
     * @param size
     * @return true if ship sunk, else false
     */
    bool IsShipSunk(int const *size);

public:
    /**
     * Play board constructor
     * @param multiplier the board will be created with multiplier x multiplier size
     */
    Board(int multiplier);
    /**
     * Play board destructor
     */
    ~Board();
    /**
     * Divide ships on the play board
     */
    void RandomizeShips();
    /**
     * Attack concrete field
     * @return field true or false (miss)
     */
    int AttackField();
    /**
     * Print out current board
     */
    void PrintBoard();
};


#endif //BATTLE_SHIPS_BOARD_H
