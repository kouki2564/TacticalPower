#pragma once
#include "EnemyBase.h"
class EnemyStandard :
    public EnemyBase
{
public:
    EnemyStandard();
    ~EnemyStandard();

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

    /// <summary>
    /// ノックバック
    /// </summary>
    void KnockBackUpdate();
};

