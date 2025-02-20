#include "TitleScene.h"

#include "SoundManager.h"

// SceneState
#include "SceneStart.h"
#include "SceneMain.h"
#include "SceneEnd.h"
#include "ScenePause.h"

namespace
{
	constexpr int kBgColorTop = 0x1d1d8d;
	constexpr int kBgColorBottom = 0x000000;
	constexpr float kBgColorMoveR = 0x1d;
	constexpr float kBgColorMoveG = 0x1d;
	constexpr float kBgColorMoveB = 0x8d;
}

TitleScene::TitleScene()
{
	m_titleHandle = LoadGraph("Data/Image/TitleLogo.png");
	m_isBlink = false;
	m_sceneState = std::make_unique<SceneStart>();

	m_bgColorR = 0x1d;
	m_bgColorG = 0x1d;
	m_bgColorB = 0x8d;
	SoundManager::Instance().loopPlay("Title");

	// カメラ初期化
	m_camera.Init(VGet(0, 80, -180));
	m_light.Init(m_camera.GetPos(), m_camera.GetTargetPos());
}

TitleScene::~TitleScene()
{
	DeleteGraph(m_titleHandle);
	SoundManager::Instance().Stop("Title");
}

void TitleScene::InitScene()
{
	// 背景の準備
	for (int i = 0; i < 10; i++)
	{
		// 座標をランダムに設定
		m_bg[i].posX = GetRand(GameData::kScreenWidth);
		m_bg[i].posY = GetRand(GameData::kScreenHeight);
		// 円の半径を5～20でランダムに設定
		m_bg[i].range = GetRand(5) + 5;
		// 移動スピードを1.0～2.0でランダムに設定
		m_bg[i].speed = GetRand(100) * 0.01f + 1.0f;
	}
}

void TitleScene::UpdateScene()
{
	// 主操作
	if (m_sceneState->isMove)
	{
		// スタート文の点滅処理
		BlinkStartString(5);

		if (m_controller.GetPOVDown(InputPOV::RIGHT))
		{
			m_weaponSelect++;
			if (m_weaponSelect > 2) m_weaponSelect = 0;
		}
		else if (m_controller.GetPOVDown(InputPOV::LEFT))
		{
			m_weaponSelect--;
			if (m_weaponSelect < 0) m_weaponSelect = 2;
		}

		if (m_controller.GetInputDown(Button::A))
		{
			// シーン遷移
			ChangeState(SceneState::END);
			SoundManager::Instance().OnePlay("decision");
		}

	}

	/* Draw */
	// 背景
	BackgroundUpdate();
	// タイトルロゴ
	DrawGraph(0, 0, m_titleHandle, true);
	// スタート文章
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_startAlpha);
	if (m_weaponSelect == 0) 
		DrawFormatString2F(GameData::kScreenWidth * 0.3f, GameData::kScreenHeight * 0.7f, 0xa0a0a0, 0xeeece0, "＜　剣　＞　で戦いに向かう");
	else if (m_weaponSelect == 1) 
		DrawFormatString2F(GameData::kScreenWidth * 0.3f, GameData::kScreenHeight * 0.7f, 0xa0a0a0, 0xeeece0, "＜　斧　＞　で戦いに向かう");
	else if (m_weaponSelect == 2)
		DrawFormatString2F(GameData::kScreenWidth * 0.3f, GameData::kScreenHeight * 0.7f, 0xa0a0a0, 0xeeece0, "＜ 魔法 ＞　で戦いに向かう");

		// ゲーム進行
	if (m_sceneState->isProgress)
	{
		if (m_sceneState->GetState() == SceneState::START)
		{
			// フェードスキップ
			if (m_controller.GetInputDown(Button::A))
			{
				FadeSkip(0);
			}
			// フェードイン
			if (FadeIn(2))
			{
				ChangeState(SceneState::MAIN);
				m_isBlink = true;
			}

		}
		else if (m_sceneState->GetState() == SceneState::END)
		{
			// フェードアウト
			if (FadeOut(3))
			{
				m_nextScene = Scene::GAME;
				m_isMoveScene = true;
			}

			// スタート文の点滅処理
			BlinkStartString(20);
		}
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
}

void TitleScene::ObjectsUpdate()
{
}

void TitleScene::CollisionUpdate()
{
}

void TitleScene::CameraUpdate()
{
}

void TitleScene::BackgroundUpdate()
{
	// SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	// sin波の移動
	m_sinMove1 += 3.0f;
	m_sinMove2 -= 5.0f;

	// 合成したsin波を基準色として、上をだんだん青く、下をだんだん黒くしたグラデーション背景
	for (int x = 0; x < GameData::kScreenWidth; x++)
	{
		// 二つのsin波を合成
		auto baseY = static_cast<int>(((GameData::kScreenHeight / 2 + sin((x + m_sinMove1) * 0.05) * 40) +
									   (GameData::kScreenHeight / 2 + sin((x + m_sinMove2 + 80) * 0.025) * 40)) * 0.25f);

		// sin波を合成したy座標地点から上を基準色から青く、下を基準色から黒く変化させる
		for (int y = 0; y < GameData::kScreenHeight; y++)
		{
			// 描画する色
			auto colorR = static_cast<int>(abs(baseY - y * 0.5f) * kBgColorMoveR / GameData::kScreenHalfHeight);
			if (colorR < 0) colorR = 0;
			else if (colorR > 0xff) colorR = 0xff;
			auto colorG = static_cast<int>(abs(baseY - y * 0.5f) * kBgColorMoveG / GameData::kScreenHalfHeight);
			if (colorG < 0) colorG = 0;
			else if (colorG > 0xff) colorG = 0xff;
			auto colorB = static_cast<int>(abs(baseY - y * 0.5f) * kBgColorMoveB / GameData::kScreenHalfHeight);
			if (colorB < 0) colorB = 0;
			else if (colorB > 0xff) colorB = 0xff;

			// 描画
			DrawPixel(x, y, GetColor(colorR, colorG, colorB));

		}
	}

	// 上昇して消えていく円の描画
	for (int i = 0; i < 10; i++)
	{
		// 上昇
		m_bg[i].posY -= m_bg[i].speed;

		// 画面外に出たらｙ座標を一番下の画面外に再配置してｘ座標をランダムに設定
		if (m_bg[i].posY + m_bg[i].range < 0)
		{
			m_bg[i].posY = GameData::kScreenHeight + m_bg[i].range;
			m_bg[i].posX = GetRand(GameData::kScreenWidth);
		}

		// アルファ値の変化量の設定
		int alphaMove = 255 / m_bg[i].range;
		// 座標の上昇によるアルファ値倍率
		int alphaScale = m_bg[i].posY / GameData::kScreenHeight;

		// 円を半径の長さ分に分けて描画
		for (int range = 0; range <= m_bg[i].range; range++)
		{
			// アルファ値を半径に応じて、外側に行くほど薄く、上昇するほど薄く変化
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 - alphaMove * range));
			// DrawCircle(m_bg[i].posX, m_bg[i].posY, range, 0xffffff, true);
			DrawCircle(static_cast<int>(m_bg[i].posX), static_cast<int>(m_bg[i].posY), range, 0xffffff, true);
		}
	}
	// アルファ値を元に戻す
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

	
}

void TitleScene::BlinkStartString(int value)
{
	if (m_isBlink)
	{
		if (m_startAlpha > 255)
		{
			m_startAlpha += value;
		}
		else
		{
			m_startAlpha = 255;
			m_isBlink = false;
		}
	}
	else
	{
		if (m_startAlpha > 0)
		{
			m_startAlpha -= value;
		}
		else
		{
			m_startAlpha = 0;
			m_isBlink = true;
		}
	}
}
