#ifndef GLOBAL_H
#define GLOBAL_H

#include "board.h"
#include "enemyboard.h"
namespace glb
{
    extern const char* moduleName;
    extern int player_attack_col;
    extern int player_attack_row;
    extern int nao_attack_col;
    extern int nao_attack_row;
    extern int enemyTurn;
    extern int barcodeReady;
    extern int gameStarted;
    extern int naoShipCounter;
    extern int playerShipCounter;
    extern int waitForHit;
    extern int shipMet;
    extern int destroyed;
    extern bool waitShipSunk;
    extern bool shipSunk;       //getter setter needed
    extern bool answer;
    extern int stepOfGame2;
    extern bool bumperInitialized;

    extern Board *computerBoard;
    extern EnemyBoard *enemyBoard;

    int getGameStarted();
    void setGameStarted(int newValue);

    int getNaoShipCounter();
    void incrementNaoShipCounter();

    int getPlayerShipCounter();
    void incrementPlayerShipCounter();

    int getEnemyTurn();
    void setEnemyTurn(int newValue);

    int getBarcodeReady();
    void setBarcodeReady(int newValue);

    int getWaitForHit();
    void setWaitForHit(int newValue);

    int getPlayerAttackCol();
    void setPlayerAttackCol(int newValue);

    int getPlayerAttackRow();
    void setPlayerAttackRow(int newValue);

    int getNaoAttackCol();
    void setNaoAttackCol(int newValue);

    int getNaoAttackRow();
    void setNaoAttackRow(int newValue);

    int getShipMet();
    void setShipMet(int newInt);

    int getDestroyed();
    void setDestroyed(int newInt);

    bool getWaitShipSunk();
    void setWaitShipSunk(bool newBool);
}


#endif // GLOBAL_H
