#pragma once
#include <map>
#include <string>
#include <vector>
#include "DxLib.h"

struct ImageData
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

struct numData
{
	// 描画する桁ごとの数値
	std::vector<int> number;
	// 時間ごとに低下するアルファ値
	int decAlpha;
	// 描画する時間
	int time;
	// 描画する座標
	VECTOR pos;
	// 時間ごとのスケール拡大値
	float addScale;
};

class PanelManager
{
public:
	PanelManager();
	~PanelManager();

	//インスタンスを返す
	static PanelManager& Instance() {
		static PanelManager instance;
		return instance;
	}
	void sum() {}

	void PanelUpdate();

	// 画像の登録
	void RegisterImage(std::string name, int handle, int startX, int startY, int endX, int endY);
	
	// 画像の削除
	void DeleteImage(std::string name) { m_imageData.erase(name); }

	void Draw();

	// 画像の描画
	void DrawPanel();

	// 画像を描画するかどうか
	void SetIsDrawImage(std::string name, bool isDraw);

	// 画像の座標を設定
	void UpdateImagePos(std::string name, int startX, int startY, int endX, int endY);

	/// <summary>
	/// 描画する数値の設定
	/// </summary>
	/// <param name="number">描画する数値</param>
	/// <param name="pos">描画座標</param>
	/// <param name="scale">描画最大拡大値</param>
	/// <param name="drawTime">描画フレーム</param>
	void SetNumber(int number, VECTOR pos, float scale, int drawTime);


private:
	// 数値の描画
	void DrawNumber();

	std::map<std::string, ImageData> m_imageData;

	int m_numberHandle[10];
	
	std::vector<numData> m_numberData;
};

