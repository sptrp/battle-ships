//
// Created by Sptrp on 17.04.21.
//

#include <iostream>
#include <time.h>
#include <array>
#include "Board.h"

/**
 * Play board constructor
 * @param multiplier the board will be created with multiplier x multiplier size
 */
Board::Board(const int multiplier) {

    board = std::vector< std::vector<bool> >(
            multiplier,std::vector<bool>(multiplier, false));
}

/**
 * Play board destructor
 */
Board::~Board() {

    for (int x = 0; x < board.size(); x++) {
        // Use swap() to make sure memory was cleared from vectors
        std::vector<bool>().swap(board[x]);
    }
    std::vector< std::vector<bool> >().swap(board);
    std::vector< std::array<int, 2> >().swap(blacklist);
}

/**
 * Divide ships on the play board
 */
void Board::RandomizeShips() {
    // Reset random generator to produce pseudorandom values
    // rand() will return same result every time without resetting
    srand ( time(nullptr) );

    PlaceVarFieldsShip(4);
    PlaceVarFieldsShip(3);
    PlaceVarFieldsShip(3);
    PlaceVarFieldsShip(2);
    PlaceVarFieldsShip(2);
    PlaceVarFieldsShip(1);
}

/**
 * Randomly place n-size ship on play board
 * @param size
 */
void Board::PlaceVarFieldsShip(int size) {

    int rotation = rand() % 2;
    std::array<int, 2> newCoord = RandomizeCoordinate();

    // If horizontal
    if (rotation == 0) {
        int freeCols = CountFreeCols(newCoord[1]);

        if (freeCols >= size) {
            std::cout << "Place found!" << std::endl;

            for (int col = 0; col < 7; col++) {
                if (IsSlotFree(col, newCoord[1], size, rotation)) {
                    for (int counter = 0; counter < size; counter++) {

                        OccupyField(col + counter, newCoord[1]);
                    }
                    break;
                }
            }

        } else {

            PlaceVarFieldsShip(size);
        }
     // If vertical
    } else {
        int freeRows = CountFreeRows(newCoord[0]);

        if (freeRows >= size) {
            std::cout << "Place found!" << std::endl;

            for (int row = 0; row < 7; row++) {
                if (IsSlotFree(newCoord[0], row, size, rotation)) {
                    for (int counter = 0; counter < size; counter++) {

                        OccupyField(newCoord[0], row + counter);
                    }
                    break;
                }
            }

        } else {

            PlaceVarFieldsShip(size);
        }
    }
}

/**
 * Check if next slot for n-size ship is free
 * @param col
 * @param row
 * @param size
 * @param rotation
 * @return true if free, otherwise false
 */
bool Board::IsSlotFree(int col, int row, int size, int rotation) {

    // Check next size-x cols or rows and return false if any occupied
    for (int counter = size - 1; counter >= 0; counter--) {
        if (rotation == 0) {

            if (IsInBlacklist(col + counter, row))
                return false;
        } else {

            if (IsInBlacklist(col, row + counter))
                return false;
        }
    }
    return true;
}

/**
 * Occupy ship field and place in blacklist
 * @param col
 * @param row
 */
void Board::OccupyField(int col, int row) {

    board[col][row] = true;
    StoreFieldInBlackList(col, row);
}

/**
 * Count all free columns in a row
 * @param row
 * @return a number of free columns
 */
int Board::CountFreeCols(int row) {

    int freeFieldsResult = 0;
    int freeFieldsCounter = 0;

    for (int col = 0; col < 7; col++) {

        if (!IsInBlacklist(col, row)) {

            freeFieldsCounter++;
            // If counter > last result, store new result
            if (freeFieldsCounter > freeFieldsResult) {

                freeFieldsResult = freeFieldsCounter + 0;
            }
        } else {

            freeFieldsCounter = 0;
        }
    }

    return freeFieldsResult;
}

/**
 * Count all free rows in a column
 * @param col
 * @return a number of free rows
 */
int Board::CountFreeRows(int col) {

    int freeFieldsResult = 0;
    int freeFieldsCounter = 0;

    for (int row = 0; row < 7; row++) {
        if (!IsInBlacklist(col, row)) {

            freeFieldsCounter++;

            if (freeFieldsCounter > freeFieldsResult) {
                // Store new result, if counter > last result
                freeFieldsResult = freeFieldsCounter + 0;
            }
        } else {

            freeFieldsCounter = 0;
        }
    }

    return freeFieldsResult;
}

/**
 * Place given field and all fields surrounding it in blacklist
 * @param y
 * @param x
 */
void Board::StoreFieldInBlackList(int y, int x) {

    // Find all surrounding cells
    for (int row = x - 1; row <= x + 1; row++) {
        for (int col = y - 1; col <= y + 1; col++) {

            // Check if cell is inside grid
            if (IsWithinGrid (col, row)) {

                std::array<int, 2> coord = {col, row};
                // Check if coordinate is already in blacklist to avoid duplicates
                if (!(IsInBlacklist(coord[0], coord[1])))
                    blacklist.push_back(coord);
            }
        }
    }
}

/**
 * Check if field is in blacklist
 * @param col
 * @param row
 * @return true if yes, otherwise false
 */
bool Board::IsInBlacklist(int col, int row) {

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
 * Check if field is still in bounds of play board
 * @param col
 * @param row
 * @return true if yes, otherwise false
 */
bool Board::IsWithinGrid(int col, int row) {
    // Is false if out of bounds
    if ((col < 0) || (row < 0) || (col > 6) || (row > 6)) {

        return false;
    }
    return true;
}

/**
 * Create random coordinate
 * @return coordinate
 */
std::array<int, 2> Board::RandomizeCoordinate() {

    std::array<int, 2> coord = {rand() % 7, rand() % 7};

    return coord;
}

/**
 * Print out current board
 */
void Board::PrintBoard() {

    for (int row = 0; row < board.size(); row++) {
        for (int col = 0; col < board.size(); col++) {

            if (col != board.size() - 1) {
                // Print values 0 - 6 of each row
                board[col][row] ? std::cout << " X " << std::flush : std::cout << " _ " << std::flush;
            } else {
                // Enter if 7
                board[col][row] ? std::cout << " X " << std::endl : std::cout << " _ " << std::endl;
            }
        }
    }
}

bool Board::AttackField() {

    int col, row;

    std::cout <<  "Starting your attack" << std::endl;

    std::cout << "Enter col : ";
    std::cin >> col;
    std::cout << "Enter row : ";
    std::cin >> row;

    std::cout << col << std::flush;
    std::cout << " : " << std::flush;
    std::cout << row << std::endl;

    return board[col][row];
}

/**
 *
 */
//void Board::ShipOrMiss() {



/**
 *
 */
//void Board::RepeatFunc() {
