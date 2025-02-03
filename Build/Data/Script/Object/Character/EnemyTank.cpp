#include "EnemyTank.h"
#include "ChackCollision.h"

// State関連
#include "Move.h"
#include "Jump.h"
#include "Attack.h"
#include "Knock.h"
#include "Dodge.h"
#include "Death.h"

namespace
{
	// モデルデータ情報
	constexpr float kRadius = 10.0f;

	constexpr float kScale = 15.0f;

	// アイドル時間
	constexpr int kIdleTime = 250;

	// 移動速度
	constexpr float kMovePow = 0.25f;

	// パンチ攻撃時間
	constexpr int kPunchTime = 250;

	// 追跡開始距離
	constexpr float kTargetRange = 100.0f;

	// 攻撃開始距離
	constexpr float kAttackRange = 20.0f;

	// 各ステータス（変動なしのためconstexpr）
	// HP
	constexpr int kMaxHP = 150;
	// 攻撃
	constexpr int kAtk = 10;
	// 防御
	constexpr int kDef = 10;
	// 素早さ
	constexpr int kAgi = 10;

	constexpr int kDeathTime = 60;
}

EnemyTank::EnemyTank()
{

}

EnemyTank::~EnemyTank()
{
	MV1DeleteModel(m_handle);
}

void EnemyTank::Init(std::string name, VECTOR pos, VECTOR dir)
{
	m_handle = MV1LoadModel("Data/Model/Enemy_2.mv1");
	m_name = name;
	m_pos = pos;
	m_dir = dir;
	auto frameIndex0 = MV1SearchFrame(m_handle, "Armature");
	auto pos0 = VAdd(m_pos, MV1GetFramePosition(m_handle, frameIndex0));
	auto frameIndex1 = MV1SearchFrame(m_handle, "mixamorig:Head");
	// マジックナンバー
	auto pos1 = VGet(pos0.x, MV1GetFramePosition(m_handle, frameIndex1).y * kScale - kRadius * 0.5f, pos0.z);
	// マジックナンバー
	auto scale = VGet(kScale, kScale, kScale);
	DrawingManager::Instance().RegisterModel(m_name, m_handle, m_pos, m_dir, scale);
	m_collider.SetCapsule(pos0, pos1, 3, kRadius);
	// マジックナンバー
	DrawingManager::Instance().CallAnimation(m_name, "Armature|Idle", 120);


	//当たり判定設定
	m_frameIndex = MV1SearchFrame(m_handle, "mixamorig:RightHand");
	// マジックナンバー
	m_hitCol.SetSphere(MV1GetFramePosition(m_handle, m_frameIndex), -1, 8);
	m_hitCol.isChackOther = false;


	// パラメータ設定
	m_parameter.SetPalameter(kMaxHP, kAtk, kDef, kAgi);
	m_collider.isOnDamage = true;


	std::shared_ptr<Move> pState = std::make_shared<Move>();
	m_state = pState;
	m_waitTimeMax = kIdleTime;
	m_waitTimer = 0;
	m_collider.isOnDamage = true;
}

void EnemyTank::Update()
{
	MoveUpdate();
	AttackUpdate();
	WaitTimeCount();
	DrawHpBar(m_pos);

	// 死んだあとの処理
	if (m_state->GetState() == State::DEATH)
	{
		if (m_waitTimer == m_waitTimeMax)
		{
			// 削除可能にする
			m_isDelete = true;
		}
	}
}

void EnemyTank::ApplyUpdating()
{
	m_dir = m_physics.GetDir();
	auto moveVec = m_physics.GetUpdateVec();
	// 座標反映
	m_pos = VAdd(m_pos, moveVec);
	m_collider.ColliderUpdate(moveVec);

	auto handle = DrawingManager::Instance().GetModelHandle(m_name);
	m_hitCol.pos[0] = MV1GetFramePosition(handle, m_frameIndex);

#ifdef _DEBUG
	DrawCapsule3D(m_collider.pos[0], m_collider.pos[1], kRadius, 8, 0xff0000, 0xffffff, false);
	if (m_hitCol.isChackOther)
	{
		DrawSphere3D(m_hitCol.pos[0], 8, 8, 0xffff00, 0xffffff, false);
	}
	else
	{
		DrawSphere3D(m_hitCol.pos[0], 8, 8, 0xff0000, 0xffffff, false);
	}

#endif // _DEBUG

	DrawingManager::Instance().UpdateModelData(m_name, m_pos, m_dir);

}

void EnemyTank::SetCameraDir(VECTOR cameraDir)
{
}

bool EnemyTank::GetIsHitAttack(Collider& colOther)
{
	// 当たり判定処理
	if (colOther.isOnDamage)
	{
		bool isHit = ChackCollision::Instance().GetIsCollision(m_hitCol, colOther);

		// ダメージ計算
		m_damagePoint = 0;
		// 基礎攻撃値を設定
		float tempPoint = m_parameter.GetPalameter(1) * 1.5f;

		if (isHit)
		{
#ifdef _DEBUG
			printf("\nHit\n");
#endif // _DEBUG
			m_damagePoint += tempPoint * 2.0f;
			m_hitCol.isChackOther = false;
		}

		return isHit;
	}
	else
	{
		return false;
	}
}

void EnemyTank::SetDamagePoint(float damagePoint)
{
	// 防御値分の計算
	int resDamagePoint = static_cast<int>(damagePoint - (m_parameter.GetPalameter(2) * 0.1f));

	// 最低限のダメージは入れてクリア不可能対策
	if (resDamagePoint <= 0) resDamagePoint = 1;

	// 反映
	m_parameter.SetDamage(resDamagePoint);

	// ダメージ表示
	// 10未満で拡大値1.5
	if (resDamagePoint < 10)
	{
		PanelManager::Instance().SetNumber(resDamagePoint, m_pos, 1.0f, 30);
	}
	// 100未満で拡大値2.0
	else if (resDamagePoint < 100)
	{
		PanelManager::Instance().SetNumber(resDamagePoint, m_pos, 1.5f, 30);
	}
	// 1000未満で拡大値2.0
	else if (resDamagePoint < 1000)
	{
		PanelManager::Instance().SetNumber(resDamagePoint, m_pos, 2.0f, 30);
	}
	// 1000以上で拡大値2.5
	else
	{
		PanelManager::Instance().SetNumber(resDamagePoint, m_pos, 2.5f, 30);
	}

	// 一応HPのマイナス化防止とやられた判定をここでとる
	if (m_parameter.GetPalameter(0) <= 0)
	{
		ChangeState(State::DEATH);
		DrawingManager::Instance().CallAnimation(m_name, "Armature|Die", kDeathTime);
		m_waitTimeMax = kDeathTime;
		m_waitTimer = 0;
		// 当たり判定の消去
		m_collider.isOnDamage = false;
		// 死んだあとには攻撃判定を消す
		m_hitCol.isChackOther = false;
	}

#ifdef _DEBUG

	printf("\n%d のダメージ！\n郷田 重雄  残HP：%d\n", resDamagePoint, m_parameter.GetPalameter(0));
	if (m_parameter.GetPalameter(0) <= 0)
	{
		printf("討伐っ！\n");
	}

#endif // _DEBUG
}

void EnemyTank::MoveUpdate()
{
	if (m_state->GetState() == State::MOVE)
	{
		auto vec = VSub(m_targetPos, m_pos);
		vec.y = 0;
		if (VSize(vec) > kTargetRange)
		{
			DrawingManager::Instance().CallAnimation(m_name, "Armature|Idle", 120);
		}
		else if (VSize(vec) > kAttackRange)
		{
			DrawingManager::Instance().CallAnimation(m_name, "Armature|Walk", 120);
			m_physics.Move(VNorm(vec), kMovePow);
		}
		else
		{
			if (m_state->isAttack)
			{
				m_physics.SetDir(VNorm(vec));
				ChangeState(State::ATTACK);
				m_waitTimeMax = kPunchTime;
				m_waitTimer = 0;
			}
		}
	}
}

void EnemyTank::AttackUpdate()
{
	if (m_state->GetState() == State::ATTACK)
	{
		DrawingManager::Instance().CallAnimation(m_name, "Armature|Punch", kPunchTime);

		if (m_waitTimer == static_cast<int>(m_waitTimeMax * 0.3f))
		{
			m_hitCol.isChackOther = true;
		}
		else if (m_waitTimer == static_cast<int>(m_waitTimeMax * 0.4f))
		{
			m_hitCol.isChackOther = false;
		}
		// 攻撃終了
		if (m_waitTimer == m_waitTimeMax)
		{
			ChangeState(State::MOVE);
		}
	}
}
