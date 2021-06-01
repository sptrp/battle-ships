#ifndef GLOBAL_H
#define GLOBAL_H

namespace glb
{
    extern int player_attack_col;
    extern int player_attack_row;
    extern int nao_attack_col;
    extern int nao_attack_row;
    extern int enemyTurn;
    extern int barcodeReady;

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
