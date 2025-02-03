#include "PauseMenu.h"
#include "DxLib.h"
#include "GameData.h"

namespace
{
	constexpr int kSelectNum = 3;
	constexpr int kStatusMenuNum = 3;
	constexpr int kSoundMenuNum = 3;
}

PauseMenu::PauseMenu()
{
	// 各フラグの初期化
	m_isSwitchPause = false;
	m_isPause = false;
	m_isPopUp = false;
	m_isSelect = false;

	Image initData;
	initData.handle = LoadGraph("Data/Image/PauseMenu.png");
	initData.startX = 0;
	initData.startY = GameData::kScreenHalfHeight;;
	initData.endX = GameData::kScreenWidth;
	initData.endY = GameData::kScreenHalfHeight;
	initData.isDraw = false;
	m_imageData.push_back(initData);
}

PauseMenu::~PauseMenu()
{
}

void PauseMenu::Update()
{
	// ポーズメニューの表示
	if (m_isSwitchPause)
	{
		// ポーズ画面が表示されていないとき
		if (!m_isPopUp)
		{
			// ポーズ画面展開
			PopUp();
		}
		// ポーズ画面が表示されているとき
		else
		{
			// ポーズ画面縮小
			Close();
		}
	}
	// ポーズ画面の展開終了時の処理
	else if (m_isOpe)
	{
		// ポーズ画面処理
		SelectMenu();
	}
}

void PauseMenu::Draw()
{
	// 背景を暗くする
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(0, 0, GameData::kScreenWidth, GameData::kScreenHeight, 0x000000, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

	// ウインドウの描画
	if (m_imageData[0].isDraw)
	{
		DrawExtendGraph(m_imageData[0].startX, m_imageData[0].startY, m_imageData[0].endX, m_imageData[0].endY, m_imageData[0].handle, TRUE);
	}

	// 左ウインドウ内容
	if (m_isPopUp)
	{
		// メニュー概要の描画
		
	}

	// 右ウインドウ内容
	if (m_pauseMenuState == PauseMenuState::STATUS)
	{		
		if (m_imageData[0].isDraw)
		{
			// ステータス内容の描画
		}
	}
	else if (m_pauseMenuState == PauseMenuState::SOUND)
	{
		if (m_imageData[0].isDraw)
		{
			// サウンド調整内容の描画
		}
	}
	else if (m_pauseMenuState == PauseMenuState::EXIT)
	{
		if (m_imageData[0].isDraw)
		{
			// タイトルに戻る内容の描画
		}
	}
}

void PauseMenu::SwitchPause()
{
	if (!m_isSwitchPause)
		m_isSwitchPause = true;
}

int PauseMenu::GetUpgrade()
{
	if (m_isUpgrade)
	{
		m_isUpgrade = false;

		// 強化パラメータの選択

		
	}
	else
	{
		return -1;
	}
}

void PauseMenu::PopUp()
{
	// ポーズ画面の開始
	m_isPause = true;
	// ポーズ画面の表示
	m_imageData[0].isDraw = true;
	// ポーズ画面展開
	if (m_imageData[0].startY > 0)
	{
		m_imageData[0].startY -= 20;
		if (m_imageData[0].startY <= 0)
		{
			m_imageData[0].startY = 0;
		}
	}

	if (m_imageData[0].endY < GameData::kScreenHeight)
	{
		m_imageData[0].endY += 20;
		if (m_imageData[0].endY >= GameData::kScreenHeight)
		{
			m_imageData[0].endY = GameData::kScreenHeight;
		}
	}

	// 展開終了後機能開始
	if (m_imageData[0].startY == 0 && m_imageData[0].endY == GameData::kScreenHeight)
	{
		// ポーズ画面の使用開始
		m_isPopUp = true;
		// 操作開始
		m_isOpe = true;
		m_isSwitchPause = false;
	}
}

void PauseMenu::Close()
{
	// 操作終了
	m_isOpe = false;

	// ポーズ画面の縮小
	if (m_imageData[0].startY < GameData::kScreenHalfHeight)
	{
		m_imageData[0].startY += 20;
		if (m_imageData[0].startY >= GameData::kScreenHalfHeight)
		{
			m_imageData[0].startY = GameData::kScreenHalfHeight;
		}
	}
	
	if (m_imageData[0].endY > GameData::kScreenHalfHeight)
	{
		m_imageData[0].endY -= 20;
		if (m_imageData[0].endY <= GameData::kScreenHalfHeight)
		{
			m_imageData[0].endY = GameData::kScreenHalfHeight;
		}
	}

	// ポーズ画面を非表示にする
	if (m_imageData[0].startY == GameData::kScreenHalfHeight && m_imageData[0].endY == GameData::kScreenHalfHeight)
	{
		// ポーズ画面の表示終了
		m_isPopUp = false;
		m_imageData[0].isDraw = false;
		m_isSwitchPause = false;

		// ポーズ画面の終了
		m_isPause = false;
	}
}

void PauseMenu::SelectMenu()
{
	// 概要の選択
	if (m_isOpe)
	{
		// カーソル移動量
		int cursorMove = 0;
		// パッドによる上下の選択
		if (m_controller.GetPOVDown(InputPOV::UP) || m_controller.GetStickDown(Stick::STICK_L, false) >= 500)
		{
			// 上にカーソル移動値
			cursorMove = 1;

		}
		else if (m_controller.GetPOVDown(InputPOV::DOWN) || m_controller.GetStickDown(Stick::STICK_L, false) <= -500)
		{
			// 下にカーソル移動値
			cursorMove = -1;
		}

		

		// 内容の選択
		if (!m_isSelect)
		{
			// カーソル位置の指定
			m_selectCursorNum += cursorMove;
			if (m_selectCursorNum < 0) m_selectCursorNum = kSelectNum - 1;
			if (m_selectCursorNum >= kSelectNum) m_selectCursorNum = 0;

			// 右ウインドウ準備
			if (m_selectCursorNum == 0)
			{
				m_pauseMenuState = PauseMenuState::STATUS;
				// 表示物の準備
			}
			else if (m_selectCursorNum == 1)
			{
				m_pauseMenuState = PauseMenuState::SOUND;
				// 表示物の準備
			}
			else if (m_selectCursorNum == 2)
			{
				m_pauseMenuState = PauseMenuState::EXIT;
				// 表示物の準備
			}

			// 概要の決定
			if (m_controller.GetInputDown(Button::A))
			{
				m_isSelect = true;

				// 追加の画面表示の準備
				if (m_pauseMenuState == PauseMenuState::EXIT)
				{
					// 終了選択画面の表示準備
				}

			}
		}
		else
		{
			m_menuCursorNum += cursorMove;
			// ステータス確認・強化時
			if (m_pauseMenuState == PauseMenuState::STATUS)
			{
				// カーソル箇所の移動
				if (m_menuCursorNum < 0) m_menuCursorNum = kStatusMenuNum - 1;
				if (m_menuCursorNum >= kStatusMenuNum) m_menuCursorNum = 0;

				// 選択
				if (m_controller.GetInputDown(Button::A))
				{
					// 強化ポイントがあれば、パラメータ強化フラグを立てる
					if (m_upgradePoint > 0)
					{
						m_upgradePoint--;
						m_isUpgrade = true;
					}
				}
			}
			// サウンド調整時
			else if (m_pauseMenuState == PauseMenuState::SOUND)
			{
				if (m_menuCursorNum < 0) m_menuCursorNum = kSoundMenuNum - 1;
				if (m_menuCursorNum >= kSoundMenuNum) m_menuCursorNum = 0;
			}
			else if (m_pauseMenuState == PauseMenuState::EXIT)
			{
				if (m_menuCursorNum < 0) m_menuCursorNum = 1;
				if (m_menuCursorNum > 1) m_menuCursorNum = 0;
			}
			
			// ポーズ解除時
			if (m_controller.GetInputDown(Button::B))
			{
				m_isSelect = false;
			}
		}
	}
}
