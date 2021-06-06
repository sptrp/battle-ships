/**
 @file global.h
 @authors Ivan Ponomarev, Stefan Holzbauer
 @date 17.04.2021
 @brief Header-File for global variables, has setters and getters for log functionality
*/


#ifndef GLOBAL_H
#define GLOBAL_H

#include "board.h"
#include "enemyboard.h"
#include <vector>
namespace glb
{
    // Variables
    /**
     * const moduleName
     */
    extern const char* moduleName;
    /**
     * int of the player attack column
     */
    extern int player_attack_col;
    /**
     * int of the player attack row
     */
    extern int player_attack_row;
    /**
     * int of nao attack column
     */
    extern int nao_attack_col;
    /**
     * int of nao attack row
     */
    extern int nao_attack_row;
    /**
     * int if barcode recognition is initialized (1 yes, 0 no)
     */
    extern int barcodeReady;
    /**
     * int if game session has been started (1 yes, 0 no)
     */
    extern int gameStarted;
    /**
     * number of NAO's sunken ships
     */
    extern int naoShipCounter;
    /**
     * number of player's sunken ships
     */
    extern int playerShipCounter;
    /**
     * int if ship has been hit (1 yes, 0 no)
     */
    extern int shipMet;
    /**
     * int if ship has been destroyed (1 yes, 0 no)
     */
    extern int destroyed;
    /**
     * bool to wait for player's input if ship has been sunk or not
     */
    extern bool waitShipSunk;
    /**
     * bool if bumper recognition has been initialized
     */
    extern bool bumperInitialized;
    /**
     * NAO's board
     */
    extern Board *computerBoard;
    /**
     * player's board
     */
    extern EnemyBoard *enemyBoard;

    // Methods
    /**
     * method to get gameStarted
     * @return gameStarted
     */
    int getGameStarted();
    /**
     * setter method to for gameStarted
     * @param newValue value of gameStarted
     */
    void setGameStarted(int newValue);
    /**
     * method to get naoShipCounter
     * @return naoShipCounter
     */
    int getNaoShipCounter();
    /**
     * method that adds 1 to naoShipCounter
     */
    void incrementNaoShipCounter();
    /**
     * method to get playerShipCounter
     * @return playerShipCounter
     */
    int getPlayerShipCounter();
    /**
     * method that adds 1 to playerShipCounter
     */
    void incrementPlayerShipCounter();
    /**
     * method to get barcodeReady
     * @return barcodeReady
     */
    int getBarcodeReady();
    /**
     * setter method to for barcodeReady
     * @param newValue value of barcodeReady
     */
    void setBarcodeReady(int newValue);
    /**
     * method to get playerAttackCol
     * @return playerAttackCol
     */
    int getPlayerAttackCol();
    /**
     * setter method to for playerAttackCol
     * @param newValue value of playerAttackCol
     */
    void setPlayerAttackCol(int newValue);
    /**
     * method to get playerAttackRow
     * @return playerAttackRow
     */
    int getPlayerAttackRow();
    /**
     * setter method to for playerAttackRow
     * @param newValue value of playerAttackRow
     */
    void setPlayerAttackRow(int newValue);
    /**
     * method to get naoAttackCol
     * @return naoAttackCol
     */
    int getNaoAttackCol();
    /**
     * setter method to for naoAttackCol
     * @param newValue value of naoAttackCol
     */
    void setNaoAttackCol(int newValue);
    /**
     * method to get naoAttackRow
     * @return naoAttackRow
     */
    int getNaoAttackRow();
    /**
     * setter method to for naoAttackRow
     * @param newValue value of naoAttackRow
     */
    void setNaoAttackRow(int newValue);
    /**
     * method to get shipMet
     * @return shipMet
     */
    int getShipMet();
    /**
     * setter method to for shipMet
     * @param newValue value of shipMet
     */
    void setShipMet(int newInt);
    /**
     * method to get destroyed
     * @return destroyed
     */
    int getDestroyed();
    /**
     * setter method to for destroyed
     * @param newValue value of destroyed
     */
    void setDestroyed(int newInt);
    /**
     * method to get waitShipSunk
     * @return waitShipSunk
     */
    bool getWaitShipSunk();
    /**
     * setter method to for waitShipSunk
     * @param newBool value of waitShipSunk
     */
    void setWaitShipSunk(bool newBool);
    /**
     * method to get bumperInitialized
     * @return bumperInitialized
     */
    bool getBumperInitialized();
    /**
     * setter method to for bumperInitialized
     * @param newBool value of bumperInitialized
     */
    void setBumperInitialized(bool newBool);
}


#endif // GLOBAL_H
