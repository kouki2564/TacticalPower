#include "EnemyMagic.h"
#include "Quaternion.h"
#include "ChackCollision.h"

#include "SoundManager.h"
#include "EffectManager.h"

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
	constexpr float kMovePow = 0.4f;

	constexpr float kShotRadius = 3.0f;

	// 魔法弾数
	constexpr int kShotNum = 3;

	// 魔法速度
	constexpr float kShotSpeed = 0.3f;

	// 魔法残存時間
	constexpr int kShotTime = 240;

	// 魔法攻撃時間
	constexpr int kSpellingTime = 80;

	// 逃走開始距離
	constexpr float kEscapeRange = 50.0f;

	// 攻撃開始距離
	constexpr float kAttackRange = 100.0f;

	// 各ステータス（変動なしのためconstexpr）
	// HP
	constexpr int kMaxHP = 100;
	// 攻撃
	constexpr int kAtk = 10;
	// 防御
	constexpr int kDef = 10;
	// 素早さ
	constexpr int kAgi = 10;

	constexpr int kKnockTime = 20;
	constexpr float kKnockPow = 0.1f;

	constexpr int kDeathTime = 60;
}

EnemyMagic::EnemyMagic()
{
	EffectManager::Instance().RegisterEffect("HitM", LoadEffekseerEffect("Data/Effect/Hit.efkefc", 10.0f), VGet(0, 0, 0), VGet(0, 0, 0));
	for (int i = 0; i < 50; i++)
	{
		EffectManager::Instance().RegisterEffect("ShotM" + std::to_string(i), LoadEffekseerEffect("Data/Effect/EnemyShot.efkefc", 1.0f), VGet(0, 0, 0), VGet(0, 0, 0));
	}
}

EnemyMagic::~EnemyMagic()
{
	MV1DeleteModel(m_handle);
}

void EnemyMagic::Init(std::string name, int& handle, VECTOR pos, VECTOR dir)
{
	m_handle = MV1DuplicateModel(handle);
	m_name = name;
	m_dir = dir;
	m_pos = pos;
	auto frameIndex0 = MV1SearchFrame(m_handle, "Armature");
	auto pos0 = VAdd(m_pos, MV1GetFramePosition(m_handle, frameIndex0));
	auto frameIndex1 = MV1SearchFrame(m_handle, "mixamorig10:Head");
	// マジックナンバー
	auto pos1 = VGet(pos0.x, MV1GetFramePosition(m_handle, frameIndex1).y * kScale - kRadius * 0.5f, pos0.z);
	// マジックナンバー
	auto scale = VGet(kScale, kScale, kScale);
	DrawingManager::Instance().RegisterModel(m_name, m_handle, m_pos, m_dir, scale);
	m_collider.SetCapsule(pos0, pos1, 2, kRadius);
	// マジックナンバー
	DrawingManager::Instance().CallAnimation(m_name, "Armature|Idle", 90);


	//当たり判定設定
	m_frameIndex = MV1SearchFrame(m_handle, "mixamorig10:RightHand");
	auto poss = MV1GetFramePosition(m_handle, m_frameIndex);
	// マジックナンバー
	m_hitCol.SetSphere(pos, -1, 0);
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

void EnemyMagic::Update()
{
	MoveUpdate();
	AttackUpdate();
	MagicUpdate();
	KnockBackUpdate();
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

void EnemyMagic::ApplyUpdating()
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
	if (m_controller.GetInputDown(Button::STICK_R))
	{
		printf("\nEnemy動作状況表示\n");
		auto temp = m_physics.GetMove();
		printf("move  : %f, %f, %f\n", temp.x, temp.y, temp.z);
		temp = m_physics.GetPush();
		printf("push  : %f, %f, %f\n", temp.x, temp.y, temp.z);
		temp = m_physics.GetKnock();
		printf("knock : %f, %f, %f\n", temp.x, temp.y, temp.z);
	}

#endif // _DEBUG

	// 魔法弾描画
	for (int i = 0; i < m_magicShot.size(); i++)
	{
		DrawSphere3D(m_magicShot[i].shotCol.pos[0], kShotRadius, 8, 0xffff00, 0xffffff, true);

	}

	DrawingManager::Instance().UpdateModelData(m_name, m_pos, m_dir);
}

bool EnemyMagic::GetIsHitAttack(Collider& colOther)
{
	// 当たり判定処理
	if (colOther.isOnDamage)
	{
		bool isHit = false;
		// ダメージ計算
		m_damagePoint = 0;
		// 基礎攻撃値を設定
		float tempPoint = m_parameter.GetPalameter(1) * 1.5f;

		for (int i = 0; i < m_magicShot.size(); i++)
		{
			if (ChackCollision::Instance().GetIsCollision(m_magicShot[i].shotCol, colOther))
			{
				isHit = true;
#ifdef _DEBUG
				printf("\nHit\n");
#endif // _DEBUG
				m_damagePoint += tempPoint * 1.0f;
				m_hitCol.isChackOther = false;

				// 消滅させる
				m_magicShot.erase(m_magicShot.begin() + i);
				i--;
			}
		}

		return isHit;
	}
	else
	{
		return false;
	}
}

bool EnemyMagic::SetDamagePoint(float damagePoint)
{
	// 防御値分の計算
	int resDamagePoint = static_cast<int>(damagePoint - (m_parameter.GetPalameter(2) * 0.1f));

	// 最低限のダメージは入れてクリア不可能対策
	if (resDamagePoint <= 0) resDamagePoint = 1;
	else
	{
		if (m_state->isKnockBack)
		{
			ChangeState(State::KNOCK);
			m_waitTimeMax = kKnockTime;
			m_waitTimer = 0;
		}
	}

	// 反映
	m_parameter.SetDamage(resDamagePoint);
	SoundManager::Instance().OnePlay("Hit");

	// ダメージ表示
		// 10以下で拡大値1.5
	if (resDamagePoint < 10)
	{
		PanelManager::Instance().SetNumber(resDamagePoint, m_pos, 1.5f, 30, m_cameraDir);
	}
	// 10以上で拡大値2.0
	else if (resDamagePoint < 30)
	{
		PanelManager::Instance().SetNumber(resDamagePoint, m_pos, 2.0f, 30, m_cameraDir);
	}
	// 30以上で拡大値2.5
	else if (resDamagePoint < 50)
	{
		PanelManager::Instance().SetNumber(resDamagePoint, m_pos, 2.5f, 30, m_cameraDir);
	}
	// 50以上で拡大値3.0
	else if (resDamagePoint < 80)
	{
		PanelManager::Instance().SetNumber(resDamagePoint, m_pos, 3.0f, 30, m_cameraDir);
	}
	// 80以上で拡大値3.5
	else
	{
		PanelManager::Instance().SetNumber(resDamagePoint, m_pos, 3.5f, 30, m_cameraDir);
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

	EffectManager::Instance().PlayEffect("HitM", m_pos, m_dir);

	return true;

#ifdef _DEBUG

	printf("\n%d のダメージ！\n松佳 エイリ  残HP：%d\n", resDamagePoint, m_parameter.GetPalameter(0));
	if (m_parameter.GetPalameter(0) <= 0)
	{
		printf("討伐っ！\n");
	}

#endif // _DEBUG
}

void EnemyMagic::MoveUpdate()
{
	if (m_state->GetState() == State::MOVE)
	{
		auto vec = VSub(m_pos, m_targetPos);
		vec.y = 0;
		// プレイヤー接近時逃走
		if (VSize(vec) < kEscapeRange)
		{
			DrawingManager::Instance().CallAnimation(m_name, "Armature|Walk", 60);
			m_physics.Move(VNorm(vec), kMovePow);
		}
		// プレイヤー攻撃範囲内
		else if (VSize(vec) < kAttackRange)
		{

			if (m_state->isAttack)
			{
				m_physics.SetDir(VNorm(VSub(VZero(), vec)));
				ChangeState(State::ATTACK);
				m_waitTimeMax = kSpellingTime;
				m_waitTimer = 0;
			}
		}
		// それ以外はアイドル
		else
		{
			DrawingManager::Instance().CallAnimation(m_name, "Armature|Idle", 120);
		}
	}
	else
	{
		int a = 0;
		a++;
	}
}

void EnemyMagic::AttackUpdate()
{
	if (m_state->GetState() == State::ATTACK)
	{
		DrawingManager::Instance().CallAnimation(m_name, "Armature|Magic", kSpellingTime);

		// 魔法弾生成
		if (m_waitTimer == static_cast<int>(m_waitTimeMax * 0.35f))
		{
			Quaternion q;
			for (int i = 0; i < kShotNum; i++)
			{
				EnemyShot shotTemp;
				shotTemp.shotCol.SetSphere(m_hitCol.pos[0], -1, kShotRadius);
				shotTemp.shotCol.isChackOther = true;
				shotTemp.shotCount = 0;

				// 方向設定
				auto dir = VSub(m_targetPos, m_hitCol.pos[0]);
				dir.y = 0;
				dir = VNorm(dir);
				// 30度ずつずらす
				auto angle = (i - 1) * (DX_PI_F / 3);
				auto upVec = VGet(0, 1, 0);
				auto zeroVec = VZero();
				q.SetMove(angle, upVec, zeroVec);
				dir = q.Move(zeroVec, dir);
				shotTemp.shotDir = dir;
				// 弾の設定反映
				m_magicShot.push_back(shotTemp);
			}
		}
		// 攻撃終了
		if (m_waitTimer == m_waitTimeMax)
		{
			ChangeState(State::MOVE);
		}
	}
}

void EnemyMagic::MagicUpdate()
{
	for (int i = 0; i < m_magicShot.size(); i++)
	{
		m_magicShot[i].shotCount++;
		m_magicShot[i].shotCol.pos[0] = VAdd(m_magicShot[i].shotCol.pos[0], VScale(m_magicShot[i].shotDir, kShotSpeed));

		if (m_magicShot[i].shotCount >= kShotTime)
		{
			// 消滅条件時、消して数値補正
			m_magicShot.erase(m_magicShot.begin() + i);
			i--;
		}
	}
}

void EnemyMagic::KnockBackUpdate()
{
	if (m_state->GetState() == State::KNOCK)
	{
		DrawingManager::Instance().CallAnimation(m_name, "Armature|KnockBack", kKnockTime);
		m_collider.isOnDamage = false;
		auto vec = VScale(VSub(VZero(), m_dir), kKnockPow);
		m_physics.SetKnockVec(vec);
		// ノックバック終了
		if (m_waitTimer == kKnockTime)
		{
			m_collider.isOnDamage = true;
			ChangeState(State::MOVE);
			DrawingManager::Instance().CallAnimation(m_name, "Armature|Idle", 1);
		}
	}
}
