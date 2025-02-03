#pragma once
#include "SceneBase.h"

struct bg
{
    // 座標
	float posX;
	float posY;
    
	// 円の半径
    int range;

    // 移動スピード
    float speed;
};

class TitleScene :
    public SceneBase
{
public:
    TitleScene();
    ~TitleScene();

    void InitScene();
    void UpdateScene();

private:
    void ObjectsUpdate();

    void CollisionUpdate();

    void CameraUpdate();

	void BackgroundUpdate();

    /// <summary>
    /// スタート文の点滅
    /// </summary>
    /// <param name="value">アルファ変動値</param>
    void BlinkStartString(int value);

    // タイトルロゴ
    int m_titleHandle;
    
    // 点滅フラグ
    bool m_isBlink;
    // スタート押せのアルファ値
    int m_startAlpha = 0;

	// 背景のやつ
	bg m_bg[10];

	// sin波の移動
	float m_sinMove1 = 0;
	float m_sinMove2 = 0;

    // 基準線の色
    int m_bgColorR;
    int m_bgColorG;
	int m_bgColorB;
};

