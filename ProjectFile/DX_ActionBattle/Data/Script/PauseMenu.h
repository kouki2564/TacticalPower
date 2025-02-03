#pragma once
#include <vector>
#include "Controller.h"

struct Image
{
	// 画像のハンドル
	int handle;
	// 画像の座標
	int startX;
	int startY;
	int endX;
	int endY;
	// 画像を描画するかどうか
	bool isDraw = true;
};

enum class PauseMenuState
{
	STATUS,
	SOUND,
	EXIT
};

class PauseMenu
{
public:
	PauseMenu();
	~PauseMenu();

	
	void Update();
	void Draw();

	// ポーズメニューの表示切り替え
	void SwitchPause();

	bool GetIsEndPause() { return !m_isPause; }

	int GetUpgrade();

	// 強化ポイントの共有
	void SetUpgradePoint(int upgradePoint) { m_upgradePoint = upgradePoint; }

private:

	// ポーズメニューの展開
	void PopUp();
	// ポーズメニューの縮小
	void Close();


	// ポーズ画面展開フラグ
	bool m_isSwitchPause;

	// メニューの選択
	void SelectMenu();

	// ポーズ中かどうか
	bool m_isPause;

	// ポーズメニューの表示フラグ
	bool m_isPopUp;

	// 操作可能フラグ
	bool m_isOpe;

	// ポーズメニューの選択フラグ
	bool m_isSelect;
	// ポーズメニューの状態
	PauseMenuState m_pauseMenuState = PauseMenuState::STATUS;

	// ポーズメニューのハンドル
	std::vector<Image> m_imageData;

	// ポーズメニューの座標
	int m_posX;
	int m_posY;

	// コントローラー
	Controller m_controller;

	// カーソル位置
	int m_selectCursorNum = 0;
	int m_menuCursorNum = 0;

	// プレイヤーの強化フラグ
	int m_upgradePoint = 0;
	bool m_isUpgrade = false;
};

