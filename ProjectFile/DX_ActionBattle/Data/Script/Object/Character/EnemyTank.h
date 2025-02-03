#pragma once
#include "EnemyBase.h"
class EnemyTank :
    public EnemyBase
{
public:
    EnemyTank();
    ~EnemyTank();

    void Init(std::string name, VECTOR pos, VECTOR dir);

    void Update();

    void ApplyUpdating();

    void SetCameraDir(VECTOR cameraDir);

    void SetChaseTarget(VECTOR targetPos) { m_targetPos = targetPos; }

    bool GetIsHitAttack(Collider& colOther);

    void SetDamagePoint(float damagePoint);

    void SetUpgrade(int paramNum) {}

private:
    void MoveUpdate();
    void AttackUpdate();

};

