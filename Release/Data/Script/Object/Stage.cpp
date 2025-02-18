#include "Stage.h"
#include "DrawingManager.h"

Stage::Stage()
{
}

Stage::~Stage()
{
	MV1DeleteModel(m_handle);
}

void Stage::Init(std::string name, int& handle, VECTOR pos, VECTOR dir)
{
	// m_modelData.handle = MV1LoadModel("Data/Model/DebugStage.mv1");
	m_pos = pos;
	m_dir = dir;
	DrawingManager::Instance().RegisterMapModel(name, m_handle, m_pos, m_dir, VGet(10.0f, 10.0f, 10.0f));
	m_physics.SetGravity(0, VZero());
	m_collider.SetMap(m_handle, VGet(10.0f, 10.0f, 10.0f));
}

void Stage::InitStage(std::string name, int handle)
{
	m_handle = handle;
	m_pos = VGet(0, 0, 0);
	m_dir = VGet(0, 0, -1);
	DrawingManager::Instance().RegisterMapModel(name, m_handle, m_pos, m_dir, VGet(10.0f, 10.0f, 10.0f));
	DrawingManager::Instance().SetIsShadowModel(name, false);
	m_physics.SetGravity(0, VZero());
	m_collider.SetMap(m_handle, VGet(10.0f, 10.0f, 10.0f));
}

void Stage::SetStageHandle(int handle)
{
	m_handle = handle;
}

void Stage::SetPosAndDir(VECTOR pos, VECTOR dir)
{
}
