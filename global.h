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

    extern Board *computerBoard;
    extern EnemyBoard *enemyBoard;

    int getGameStarted();
    void setGameStarted(int newValue);

    int getNaoShipCounter();
    void incrementNaoShipCounter();

    int getEnemyTurn();
    void setEnemyTurn(int newValue);

    int getBarcodeReady();
    void setBarcodeReady(int newValue);

    int getPlayerAttackCol();
    void setPlayerAttackCol(int newValue);

    int getPlayerAttackRow();
    void setPlayerAttackRow(int newValue);

    int getNaoAttackCol();
    void setNaoAttackCol(int newValue);

    int getNaoAttackRow();
    void setNaoAttackRow(int newValue);
}


#endif // GLOBAL_H
