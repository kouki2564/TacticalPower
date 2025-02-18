#include "Light.h"


Light::Light()
{
	m_floorDirLight.dir = VGet(0, -1, 0);
	m_cameraDirLight.dir = VGet(0, -1, 0);
	m_floorDirLight.handle = CreateDirLightHandle(m_floorDirLight.dir);
	m_cameraDirLight.handle = CreateDirLightHandle(m_cameraDirLight.dir);
	SetLightEnable(true);
	ChangeLightTypeDir(VGet(0, -1, 0));
}

Light::~Light()
{
	DeleteLightHandle(m_floorDirLight.handle);
	DeleteLightHandle(m_cameraDirLight.handle);
}

void Light::Init(VECTOR cameraPos, VECTOR frontPos)
{
	/// 標準ライト（全体的な照明用）
	// ディフューズ
	COLOR_F difColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	SetLightDifColor(difColor);
	// アンビエント
	COLOR_F ambColor = { 0.8f, 0.8f, 0.8f, 1.0f };
	SetLightAmbColor(ambColor);


	/// フロアライト（カメラ目線で自然に映す用）
	// ディフューズ
	m_floorDirLight.difColor = { 0.9f,0.9f,0.9f,1.0f };
	SetLightDifColorHandle(m_floorDirLight.handle, m_floorDirLight.difColor);
	// アンビエント
	m_floorDirLight.ambColor = { 0.8f,0.8f,0.8f,1.0f };
	SetLightAmbColorHandle(m_floorDirLight.handle, m_floorDirLight.ambColor);


	/// カメラライト（主にプレイヤー用の照明用）
	// ディフューズ
	m_cameraDirLight.difColor = { 0.8f,0.8f,0.8,1.0f };
	SetLightDifColorHandle(m_cameraDirLight.handle, m_cameraDirLight.difColor);
	// スペキュラー
	m_cameraDirLight.spcColor = { 0.8f,0.8f,0.8f,1.0f };
	SetLightSpcColorHandle(m_cameraDirLight.handle, m_cameraDirLight.spcColor);

	// ライト方向の設定
	auto cameraToPlayer = VSub(frontPos, cameraPos);
	m_floorDirLight.dir = cameraToPlayer;
	cameraToPlayer.y = 0;
	m_cameraDirLight.dir = cameraToPlayer;
	SetLightDirectionHandle(m_floorDirLight.handle, m_floorDirLight.dir);
	SetLightDirectionHandle(m_cameraDirLight.handle, m_cameraDirLight.dir);
}

void Light::Update(VECTOR cameraPos, VECTOR frontPos)
{
	auto cameraToPlayer = VSub(frontPos, cameraPos);
	m_floorDirLight.dir = cameraToPlayer;
	cameraToPlayer.y = 0;
	m_cameraDirLight.dir = cameraToPlayer;
	SetLightDirectionHandle(m_floorDirLight.handle, m_floorDirLight.dir);
	SetLightDirectionHandle(m_cameraDirLight.handle, m_cameraDirLight.dir);
}