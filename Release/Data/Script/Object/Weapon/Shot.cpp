#include "Shot.h"
#include "Quaternion.h"
#include "EffectManager.h"
#include "EffekseerForDXLib.h"

namespace
{
	constexpr float kShotSpeed = 6.0f;
	constexpr float kFirstShotSpeed = kShotSpeed * 0.15f;
	constexpr float kShotDirMovePow = 5.0f;
	constexpr float kFirstShotDirMovePow = 100.0f;
}

void Shot::SetUp(int id, int handle, VECTOR startPos, VECTOR targetPos, int time, VECTOR baseDir, VECTOR shotDir, float shotSize)
{
	m_id = id;
	m_handle = handle;
	m_targetPos = targetPos;
	m_shotDir = shotDir;
	m_maxTime = time;
	m_pos = startPos;
	m_startPos = startPos;
	m_baseDir = baseDir;


}

void Shot::Update()
{
	// 進行方向の確定
	// 追従時
	if (m_isTargetStart && !m_isTargetEnd)
	{
		auto straightVec = VSub(m_targetPos, m_pos);
		auto straightDir = VNorm(straightVec);
		m_shotDir = VNorm(VAdd(VScale(m_shotDir, kShotDirMovePow), straightDir));
		// 接近すれば追従は切る
		if (VSize(straightVec) < 10.0f)
		{
			m_isTargetEnd = true;
		}
		m_pos = VAdd(m_pos, VScale(m_shotDir, kShotSpeed));
	}
	else if(!m_isTargetEnd)
	{
		Quaternion rot;
		auto zero = VZero();
		auto up = VGet(0, 1, 0);
		//	36度/フレーム
		auto angle = DX_PI_F / 5;
		// 上方向射出時
		if (m_shotDir.y > 0.5f)
		{
			// 発射方向の回転
			rot.SetMove(angle, up, zero);
		}
		// それ以外
		else
		{
			// 発射方向の回転
			rot.SetMove(angle, m_baseDir, zero);
		}
		m_shotDir = VNorm(rot.Move(zero, m_shotDir));
		auto vec = VScale(m_shotDir, kFirstShotSpeed);

		rot.SetMove(angle, m_baseDir, vec);
		m_pos = rot.Move(m_startPos, m_pos);

#ifdef _DEBUG

		DrawLine3D(m_startPos, VAdd(m_startPos, VScale(m_baseDir, 1000)), 0x0000ff);
		DrawLine3D(m_pos, VAdd(m_pos, VScale(m_shotDir, 1000)), 0x0000ff);

#endif // _DEBUG


		// 発射して１割フレームで追従開始
		if (m_time > m_maxTime * 0.3f)
		{
			m_isTargetStart = true;
		}

	}


	// 弾の当たり判定の描画
	// DrawSphere3D(m_pos, 5, 8, 0xa0a000, 0xffffff, true);

	auto name = "Pshot_" + std::to_string(m_id);
	EffectManager::Instance().MoveEffectPos(name, m_pos, m_shotDir);
	EffectManager::Instance().PlayEffect(name, m_pos, m_shotDir);


	// 生存時間管理
	if (m_time < m_maxTime)
	{
		m_time++;
	}
	else
	{
		m_isApp = false;
	}

}

void Shot::UpdateTargetPos(VECTOR targetPos)
{
	m_targetPos = targetPos;

}