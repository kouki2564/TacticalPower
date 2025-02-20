#include "SceneManager.h"
#include "TitleScene.h"
#include "GameScene.h"

#ifdef _DEBUG
#include "debugScene.h"
#endif // _DEBUG


SceneManager::SceneManager() :
	m_isGameEnd(false)
{
	SoundManager::Instance().RegisterSound("Title", LoadSoundMem("Data/Sound/BGM_Title.mp3"), 200);
	SoundManager::Instance().RegisterSound("Stage", LoadSoundMem("Data/Sound/BGM_Stage.mp3"), 120);
	SoundManager::Instance().RegisterSound("Boss", LoadSoundMem("Data/Sound/BGM_Boss.mp3"), 120);

	SoundManager::Instance().RegisterSound("Hit", LoadSoundMem("Data/Sound/SE_Hit.mp3"), 150);
	SoundManager::Instance().RegisterSound("decision", LoadSoundMem("Data/Sound/SE_decision.mp3"), 150);

	EffectManager::Instance().RegisterEffect("SistemWall_Flont", LoadEffekseerEffect("Data/Effect/SistemWall.efkefc", 10.0f), VGet(0, 0, 0), VGet(0, 0, 0));
	EffectManager::Instance().RegisterEffect("SistemWall_Back", LoadEffekseerEffect("Data/Effect/SistemWall.efkefc", 10.0f), VGet(0, 0, 0), VGet(0, 0, 0));
	EffectManager::Instance().RegisterEffect("SistemWall_Right", LoadEffekseerEffect("Data/Effect/SistemWall.efkefc", 10.0f), VGet(0, 0, 0), VGet(0, 0, 0));
	EffectManager::Instance().RegisterEffect("SistemWall_Left", LoadEffekseerEffect("Data/Effect/SistemWall.efkefc", 10.0f), VGet(0, 0, 0), VGet(0, 0, 0));

#ifdef _DEBUG
	m_pScene = std::make_unique<TitleScene>();

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
		int weaponNum = m_pScene->GetWeaponNum();
		// どのシーンに向かうかの決定
		if (m_pScene->GetNextSceneNum() == Scene::TITLE)
		{
			m_pScene.reset();
			m_pScene.release();
			m_pScene = std::make_unique<TitleScene>();
		}
		else if (m_pScene->GetNextSceneNum() == Scene::GAME)
		{
			m_pScene.reset();
			m_pScene.release();
			m_pScene = std::make_unique<GameScene>();
			m_pScene->SetWeaponNum(weaponNum);
		}
#ifdef _DEBUG
		else if (m_pScene->GetNextSceneNum() == Scene::DEBUG)
		{
			m_pScene.reset();
			m_pScene.release();
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
