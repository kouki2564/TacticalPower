#include "Camera.h"

namespace
{
	// カメラ移動量の倍率（ 0 ～ 1.0f ）
	constexpr float kMoveScale = 0.1f;
	constexpr float kMoveRange = 30.0f;
	constexpr float kTempAngle = 0.025f;
	// カメラが追従を開始する距離間隔(いらないかも)
	// constexpr float kMoveBorderRange = 100;
}

Camera::Camera():
m_distanceXZ(0.0f),
m_firstPos(VZero()),
m_cameraRange(70.0f) // マジックナンバー
{
	m_pos = m_firstPos;
	m_viewPos = VZero(); 
	m_rotate;
	m_dirY = VGet(0, 1, 0);
	m_targetViewPos = VZero(); 
	m_targetCameraPos = VZero();
	//奥行0.1～10000までをカメラの描画範囲とする
	SetCameraNearFar(1.0f, 10000.0f); // マジックナンバー
	m_collider.SetSphere(m_pos, -1, 0.1f);
	m_isTargetRock = false;
}

Camera::~Camera()
{
}

void Camera::Init(VECTOR firstPos)
{
	m_firstPos = firstPos;
	m_pos = m_firstPos;
	m_distanceXZ = sqrtf(m_pos.x * m_pos.x + m_pos.z * m_pos.z);
	m_lastObjPos = VZero();
	m_debugMove = 1.0f;
}

void Camera::Update()
{
	UpdatePos();
	// ほかに色々するならここに追加
}

void Camera::SetTargetPos(VECTOR playerPos, VECTOR targetPos)
{
	// プレイヤー見下ろしモード時
	if (m_view == ViewMode::UP)
	{
		m_targetViewPos = playerPos;
		// マジックナンバー
		m_targetCameraPos = VAdd(playerPos, VGet(0, 150, 0));

		Rotate();
	}
	// TPSモード時
	else if (m_view == ViewMode::TPS)
	{
		// ターゲット時
		if (m_isTargetRock)
		{
			// プレイヤーとボスの中間点
			m_targetViewPos = VAdd(playerPos,VScale(VSub(targetPos, playerPos), 0.8f));
			// プレイヤーからターゲット座標
			VECTOR playerToTargetView = VSub(m_targetViewPos, playerPos);
			// カメラ座標からターゲット座標のY軸のみ
			float cameraToViewY = m_targetViewPos.y - m_targetCameraPos.y;
			// ターゲット座標からプレイヤー背後のカメラ行先座標

			VECTOR dir = VNorm(VGet(-playerToTargetView.x, -cameraToViewY, -playerToTargetView.z));
			float range = VSize(VSub(playerPos, targetPos)) + kMoveRange;
			VECTOR moveVec = VScale(dir, range);
			m_targetCameraPos = VAdd(m_targetViewPos, moveVec);
		}
		// 非ターゲット時
		else
		{
			m_targetViewPos = VGet(playerPos.x, playerPos.y + 10, playerPos.z); // マジックナンバー

			if (m_isRotate)
			{
				Rotate();
			}
		}

		auto playerToCamera = VSub(m_targetCameraPos, playerPos);

		if (VSize(playerToCamera) > m_cameraRange || VSize(playerToCamera) < m_cameraRange)
		{
			auto size = VSize(playerToCamera);
			float scale = 1/(size / m_cameraRange);
			auto move = VScale(playerToCamera, scale);

			m_targetCameraPos = VAdd(playerPos, move);
			// マジックナンバー
			m_targetCameraPos.y = playerPos.y + 20;
		}

	}
	// クォータービューモード時
	else if (m_view == ViewMode::QUARTER)
	{
		m_targetViewPos = VGet(playerPos.x, playerPos.y + 10, playerPos.z); // マジックナンバー
		m_targetCameraPos = VGet(playerPos.x, playerPos.y + 80, playerPos.z - 80); // マジックナンバー

		m_dirY = VGet(0, 1, 0);
		float zeroAngle = 0;
		VECTOR zeroVec = VZero();
		m_rotate.SetMove(zeroAngle,zeroVec,zeroVec);
	}
}

void Camera::UpdatePos()
{

	// カメラの注視点まで移動する距離ベクトル
	VECTOR ViewShiftVec = VScale(VSub(m_targetViewPos, m_viewPos), kMoveScale);
	m_viewPos = VAdd(m_viewPos, ViewShiftVec);
	if (m_view == ViewMode::UP)
	{
		VECTOR zeroVec = VZero();
		m_dirY = m_rotate.Move(zeroVec, m_dirY);
	}
	m_targetViewPos = m_viewPos;
	// カメラ自体の移動する距離ベクトル
	VECTOR CameraShiftVec = VSub(m_targetCameraPos, m_pos);
	// 移動
	m_pos = VAdd(m_pos, VScale(CameraShiftVec, kMoveScale));
	m_pos = m_rotate.Move(m_viewPos, m_pos);
	m_targetCameraPos = m_pos;
	// カメラの座標とターゲット座標の反映（暫定で上ベクトルはy軸方向に固定）
	SetCameraPositionAndTargetAndUpVec(m_pos, m_viewPos, m_dirY);
	// カメラ方向の固定
	m_dir = VSub(m_viewPos, m_pos);

#ifdef _DEBUG

	DrawFormatString(10, 150, 0xffff00, "%f,%f,%f", m_pos.x, m_pos.y, m_pos.z);
	DrawFormatString(10, 170, 0xffff00, "%f,%f,%f", m_viewPos.x, m_viewPos.y, m_viewPos.z);

#endif // _DEBUG

}

void Camera::ChangeViewMode(ViewMode mode, VECTOR playerPos, VECTOR playerDir)
{
	m_view = mode;
	// プレイヤー見下ろしモード時
	if (m_view == ViewMode::UP)
	{
		m_dirY = m_dir;
	}
	// TPSモード時
	else if (m_view == ViewMode::TPS)
	{
		auto temp = VScale(playerDir, m_cameraRange);
		// マジックナンバー
		temp.y = 20;
		m_targetCameraPos = VAdd(playerPos, temp);
		m_dirY = VGet(0, 1, 0);
	}
	// クォータービューモード時
	else if (m_view == ViewMode::QUARTER)
	{
		m_targetViewPos = VZero();
		m_targetCameraPos = VGet(0, 50, -50);

		m_dirY = VGet(0, 1, 0);
		float zeroAngle = 0;
		VECTOR zeroVec = VZero();
		m_rotate.SetMove(zeroAngle, zeroVec, zeroVec);
	}
}

void Camera::ChangeIsTarget()
{ 
	m_isTargetRock = !m_isTargetRock;
	float zeroAngle = 0;
	VECTOR zeroVec = VZero();
	m_rotate.SetMove(zeroAngle, zeroVec, zeroVec);
}

VECTOR Camera::GetCameraDir()
{
	if (m_view != ViewMode::UP)
	{	
		if (m_isRotate)
		{
			return VGet(0, 0, 1);
		}
		else
		{
			auto dir = VSub(m_targetViewPos, m_targetCameraPos);
			dir.y = 0;
			return VNorm(dir);
		}
	}
	else
	{
		return VNorm(m_dirY);
	}
}

void Camera::Focus(VECTOR forcusPoint)
{

}

void Camera::Rotate()
{
	// カメラの回転値設定
	// ゲームパッドからの回転入力
	auto inputState = m_controller.GetStick(Stick::STICK_R);

	// 回転角の確定
	auto rotInput = kTempAngle * inputState.x;

	float angle = 0;
	VECTOR axis = VGet(0, 1, 0);
	VECTOR move = VZero();
	if (rotInput > 0)
	{
		angle = (1.5f * DX_PI_F / 2.0f) / 60;
	}
	else if (rotInput < 0)
	{
		angle = -(1.5f * DX_PI_F / 2.0f) / 60;
	}
	else
	{
		angle = 0;
	}
	m_rotate.SetMove(angle, axis, move);
}
