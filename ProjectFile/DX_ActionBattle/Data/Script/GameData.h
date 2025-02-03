#pragma once
#include "DxLib.h"
#include <vector>
#include "DrawingManager.h"


// フォントハンドル
struct Font
{
public:
	enum FontType
	{
		BOLD = 0,
		MEDIUM = 1,
		REGULAR = 2
	};

	Font()
	{
		// 各フォントの登録
		m_fontHandle.resize(3);
		// Bold
		m_fontHandle[FontType::BOLD] = { CreateFontToHandle("せのびゴシック Bold", 30, 3, DX_FONTTYPE_ANTIALIASING_EDGE_8X8),
										 CreateFontToHandle("せのびゴシック Bold", 20, 3, DX_FONTTYPE_ANTIALIASING_EDGE_8X8),
										 CreateFontToHandle("せのびゴシック Bold", 10, 3, DX_FONTTYPE_ANTIALIASING_EDGE_8X8) };
		// Medium
		m_fontHandle[FontType::MEDIUM] =  { CreateFontToHandle("せのびゴシック Medium", 30, 3, DX_FONTTYPE_ANTIALIASING_EDGE_8X8),
											CreateFontToHandle("せのびゴシック Medium", 20, 3, DX_FONTTYPE_ANTIALIASING_EDGE_8X8),
											CreateFontToHandle("せのびゴシック Medium", 10, 3, DX_FONTTYPE_ANTIALIASING_EDGE_8X8) };
		// Regular
		m_fontHandle[FontType::REGULAR] = { CreateFontToHandle("せのびゴシック Regular", 30, 3, DX_FONTTYPE_ANTIALIASING_EDGE_8X8),
											CreateFontToHandle("せのびゴシック Regular", 20, 3, DX_FONTTYPE_ANTIALIASING_EDGE_8X8),
											CreateFontToHandle("せのびゴシック Regular", 10, 3, DX_FONTTYPE_ANTIALIASING_EDGE_8X8) };
	}
	~Font()
	{
		// フォントハンドルの削除
		for (auto& font : m_fontHandle)
		{
			for (auto& handle : font)
			{
				DeleteFontToHandle(handle);
			}
		}
	}
	
	// インスタンスを返す
	static Font& Instance()
	{
		static Font instance;
		return instance;
	}

	/// <summary>
	///	フォントハンドルの取得
	/// </summary>
	/// <param name="type">フォントタイプ</param>
	/// <param name="index">フォントサイズ 0->30, 1->20, 2->10</param>
	/// <returns>フォントハンドル</returns>
	int GetFontHandle(FontType type, int index) { return m_fontHandle[type][index]; }

private:
	// フォントタイプとサイズでハンドルの管理
	std::vector<std::vector<int>> m_fontHandle;

};

enum
{
	UP,
	DOWN,
	RIGHT,
	LEFT
};

/// <summary>
/// ゲーム全体で使う定数
/// </summary>
namespace GameData
{
	//////////////
	// 基本設定 //
	//////////////

	// 画面幅
	constexpr int kScreenWidth = 1280;      //画面の横幅
	constexpr int kScreenHeight = 720;     //画面の縦幅
	// 半分サイズ
	constexpr int kScreenHalfWidth = kScreenWidth / 2;      //画面の横幅
	constexpr int kScreenHalfHeight = kScreenHeight / 2;     //画面の縦幅

	// ゲーム色深度
	constexpr int kColorDepth = 32;        //16 or 32

	//////////////////////
	// ゲームごとの設定 //
	//////////////////////
	// 全体シーン数
	constexpr int kSceneNum = 3;

	// ステージデータ
	constexpr int kStageMaxNum = 3;
	constexpr int kStageMaxLine = 5;
	constexpr int kStageMaxRow = 3;
	constexpr int kStagePos[kStageMaxLine][kStageMaxRow] =
	{
		{0, 2, 0},
		{0, 1, 0},
		{1, 1, 1},
		{1, 1, 1},
		{1, 1, 1}
	};
	// 使用ボタン数
	constexpr int kButtonNum = 12;

	// サウンド数
	constexpr int kBGMNum = 0;
	constexpr int kSENum = 0;

	// プレイヤー通常攻撃回数
	constexpr int kAttackCountMax = 3;

	constexpr float kGravity = 4.0f;

	// フォントハンドル
	constexpr int kFontHandle = 0;
}
