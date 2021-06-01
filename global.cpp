#include "global.h"

namespace glb
{
    int enemyTurn = -1;
    int getEnemyTurn(){
        return enemyTurn;
    }
    void setEnemyTurn(int newValue) {
        enemyTurn = newValue;
    }

    int barcodeReady = 0;
    int getBarcodeReady(){
        return barcodeReady;
    }
    void setBarcodeReady(int newValue) {
        barcodeReady = newValue;
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
}
