#include "Magic.h"
#include "Shot.h"
#include "Quaternion.h"

namespace
{
	int kShotTime = 60;
}

Magic::Magic()
{ 
	kind = Weapon::MAGIC;
	m_spherePos = VGet(0, 10, 0);
}

Magic::~Magic()
{
	MV1DeleteModel(handle);
}

void Magic::WeaponUpdate()
{
	// 弾処理
	for (int i = 0; i < m_shots.size(); i++)
	{
		// 更新処理
		m_shots[i]->UpdateTargetPos(m_targetPos);
		m_shots[i]->Update();
		colliders[i].pos[0] = m_shots[i]->GetPos();

		// 消去処理
		if (!m_shots[i]->GetIsApp() || !colliders[i].isChackOther)
		{
			// vectorでeraseを行ったら空要素分自動で詰めて要素数も更新される
			m_shots.erase(m_shots.begin() + i);
			colliders.erase(colliders.begin() + i);
			// 詰めた分要素数がずれるため補正
			i--;
		}
	}
#ifdef _DEBUG


#endif // _DEBUG

	// 魔法玉（仮）
	DrawSphere3D(m_spherePos, 3, 6, 0x00a0a0, 0xfffffff, true);

}

void Magic::ColUpdate(float scale, int time)
{
}

void Magic::SetShot(int& shotHandle, VECTOR shotDir, float shotSize, int shotNum)
{
	// １弾のみなら正面そのまま
	if (shotNum == 1)
	{ 
		Shot* temp = new Shot;
		temp->SetUp(shotHandle, m_spherePos, m_targetPos, kShotTime, shotDir, shotDir, shotSize);
		m_shots.push_back(temp);
		Collider colTemp;
		colTemp.SetSphere(m_spherePos, -1, 5);
		colTemp.isChackOther = true;
		colliders.push_back(colTemp);
	}
	// 多数ならば各弾の方向ベクトルを計算
	else
	{
		for (int num = 0; num < shotNum; num++)
		{
			Shot* temp = new Shot;
			// 分散させる角度
			float angle = 2 * DX_PI_F / shotNum;
			// 各回転角
			float oneAngle = angle * num;
			VECTOR rightDir;
			// 上方向以外への打ち出し時
			if (shotDir.y != -1)
			{
				// 上ベクトルとの外積を取り、
				// 右方向ベクトルの獲得
				rightDir = VNorm(VCross(shotDir, VGet(0, 1, 0)));
			}
			// 上方向への打ち出し時
			else
			{
				rightDir = VGet(1, 0, 0);
			}
			// 右方向を１/√３(0.577)でスケール化
			rightDir = VScale(rightDir, 0.577f);
			// クォータニオン計算
			Quaternion q;
			VECTOR zero = VZero();
			q.SetMove(oneAngle, shotDir, zero);
			// スケール化した方向ベクトルを回転
			auto dir = q.Move(zero, rightDir);

			// 最終的に反映させる弾の方向ベクトル
			VECTOR resDir = VZero();

			// 上方向以外への打ち出し時
			if (shotDir.y != 1)
			{
				resDir = VNorm(dir);
			}
			// 上方向への打ち出し時
			else
			{
				resDir = VNorm(VAdd(VScale(shotDir, 3), dir));
			}

			temp->SetUp(shotHandle, m_spherePos, m_targetPos, kShotTime, shotDir, resDir, shotSize);
			m_shots.push_back(temp);
			Collider colTemp;
			colTemp.isChackOther = true;
			colTemp.SetSphere(m_spherePos, -1, 5);
			colliders.push_back(colTemp);

		}
	}
	// vector配列のサイズ調整
	// m_shots.shrink_to_fit();
}
