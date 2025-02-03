#include "SceneManager.h"
#include "TitleScene.h"
#include "GameScene.h"

#ifdef _DEBUG
#include "debugScene.h"
#endif // _DEBUG


SceneManager::SceneManager() :
	m_isGameEnd(false)
{
#ifdef _DEBUG
	m_pScene = std::make_shared<DebugScene>();

#else
	m_pScene = std::make_unique<TitleScene>();

#endif // _DEBUG
}

SceneManager::~SceneManager()
{
}

void SceneManager::Init()
{
	m_pScene->InitScene();
}

void SceneManager::Update()
{
	m_pScene->UpdateScene();
	TransScene();
}

void SceneManager::TransScene()
{
	// シーン遷移
	if (m_pScene->GetIsMoveScene())
	{
		// どのシーンに向かうかの決定
		if (m_pScene->GetNextSceneNum() == Scene::TITLE)
		{
			m_pScene = std::make_unique<TitleScene>();
		}
		else if (m_pScene->GetNextSceneNum() == Scene::GAME)
		{
			m_pScene = std::make_unique<GameScene>();
		}
#ifdef _DEBUG
		else if (m_pScene->GetNextSceneNum() == Scene::DEBUG)
		{
			m_pScene = std::make_unique<DebugScene>();
		}
#endif // _DEBUG
		else
		{
			assert(false);
		}
		m_pScene->InitScene();
	}
}
