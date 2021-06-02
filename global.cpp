#include "global.h"


namespace glb
{
    const char* moduleName = "Battleship";

    Board *computerBoard = new Board(7);
    EnemyBoard *enemyBoard = new EnemyBoard(7);

    int stepOfGame2 = 1;
    int enemyTurn = -1;
    int getEnemyTurn(){
        return enemyTurn;
    }
    void setEnemyTurn(int newValue) {
        enemyTurn = newValue;
    }

    int gameStarted = 0;
    int getGameStarted(){
        return gameStarted;
    }
    void setGameStarted(int newValue) {
        gameStarted = newValue;
    }

    int naoShipCounter = 0;
    int getNaoShipCounter(){
        return naoShipCounter;
    }
    void incrementNaoShipCounter() {
        ++naoShipCounter;
    }

    int playerShipCounter = 0;
    int getPlayerShipCounter(){
        return naoShipCounter;
    }
    void incrementPlayerShipCounter() {
        ++playerShipCounter;
    }

    int barcodeReady = 0;
    int getBarcodeReady(){
        return barcodeReady;
    }
    void setBarcodeReady(int newValue) {
        barcodeReady = newValue;
    }

    int waitForHit = 0;
    int getWaitForHit(){
        return waitForHit;
    }
    void setWaitForHit(int newValue) {
        waitForHit = newValue;
    }

    int player_attack_col = -1;
    int getPlayerAttackCol(){
        return player_attack_col;
    }
    void setPlayerAttackCol(int newValue) {
        player_attack_col = newValue;
    }

    int player_attack_row = -1;
    int getPlayerAttackRow(){
        return player_attack_row;
    }
    void setPlayerAttackRow(int newValue) {
        player_attack_row = newValue;
    }

    int nao_attack_col = -1;
    int getNaoAttackCol(){
        return nao_attack_col;
    }
    void setNaoAttackCol(int newValue) {
        nao_attack_col = newValue;
    }

    int nao_attack_row = -1;
    int getNaoAttackRow(){
        return nao_attack_row;
    }
    void setNaoAttackRow(int newValue) {
        nao_attack_row = newValue;
    }

    int shipMet = 0;
    int getShipMet() {
        return shipMet;
    }
    void setShipMet(int newInt) {
        shipMet = newInt;
    }

    int destroyed = -1;
    int getDestroyed() {
        return destroyed;
    }
    void setDestroyed(int newInt) {
        destroyed = newInt;
    }

    bool waitShipSunk = false;
    bool getWaitShipSunk() {
        return waitShipSunk;
    }
    void setWaitShipSunk(bool newBool) {
        waitShipSunk = newBool;
    }

    bool shipSunk = false;

    bool answer = false;
    bool bumperInitialized = false;

}
