#pragma once
#include "EnemyBase.h"

struct EnemyShot
{
    Collider shotCol;
    int shotCount;
    VECTOR shotDir;
};

class EnemyMagic :
    public EnemyBase
{
public:
    EnemyMagic();
    ~EnemyMagic();

    void Init(std::string name, int& handle, VECTOR pos, VECTOR dir);

    void Update();

    void ApplyUpdating();

    void SetWeapon(int num) {}

    void SetChaseTarget(VECTOR targetPos) { m_targetPos = targetPos; }

    bool GetIsHitAttack(Collider& colOther);

    bool SetDamagePoint(float damagePoint);

    void SetUpgrade(int paramNum) {}

private:
    void MoveUpdate();
    void AttackUpdate();
    void MagicUpdate();

    std::vector<EnemyShot> m_magicShot;

    /// <summary>
    /// ノックバック
    /// </summary>
    void KnockBackUpdate();
	Controller m_controller;

    std::vector<int> m_shotEfcNum;
};

