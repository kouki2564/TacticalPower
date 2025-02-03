#pragma once
#include "DxLib.h"
#include <map>
#include <string>
#include "ToonShader.h"

/// <summary>
/// アニメーションデータ（モデルデータに付随させる）
/// </summary>
struct AnimationData
{
	// アニメーション現在フレーム
	float animaFrame = 0;
	// アニメーション遷移rate
	float animaRate = 0;
	// アニメーションインデックス
	// 再生中アニメーション
	int nowAttach = -1;
	// 移行先アニメーション
	int nextAttach = -1;
	// 現在アニメーション終了時に移行するアニメーション
	int transAttach = -1;
	// アニメーション速度
	int animaPlayFrame = 0;
	// 移行先のアニメーション速度
	int animaNextPlayFrame = 0;
	// 現在アニメーション終了時に移行するアニメーション速度
	int animaTransPlayFrame = 0;

};

/// <summary>
/// モデルデータ
/// </summary>
struct ModelData
{

	// モデルハンドル
	int handle = -1;
	// ３D空間座標
	VECTOR pos = VZero();
	// モデル回転値
	VECTOR dir = VZero();
	// モデル拡大値
	VECTOR scale = VGet(1,1,1);
	// 描画を行うかどうか
	bool isDraw = true;

	// アニメーションデータ
	AnimationData animation;


	/* 武器モデル用変数 */
	std::string ornerName = "NULL";
	int setFrameNum = 0;
	VECTOR rot = VZero();

};


/// <summary>
/// 描画
/// </summary>
class DrawingManager
{
public:
	DrawingManager()
	{
	}

	~DrawingManager(){}

	//インスタンスを返す
	static DrawingManager& Instance() {
		static DrawingManager instance;
		return instance;
	}

	/// <summary>
	/// モデル情報の登録
	/// </summary>
	void RegisterModel(std::string name, int handle, VECTOR pos, VECTOR dir, VECTOR scale);
	
	/// <summary>
	/// モデル情報の登録
	/// </summary>
	void RegisterMapModel(std::string name, int handle, VECTOR pos, VECTOR dir, VECTOR scale);

	/// <summary>
	/// 武器モデル等、モデルに付随させるモデルの登録
	/// </summary>
	void RegisterWeaponModel(std::string weaponName, int handle, VECTOR scale, VECTOR rot, std::string ownerModelName, std::string frameName);

	void DeleteModel(std::string name);

	/// <summary>
	/// モデル情報の更新
	/// </summary>
	/// <param name="name">：mapデータ呼び出し用の名前</param>
	/// <param name="modelData">：モデルの情報</param>
	void UpdateModelData(std::string name, VECTOR pos, VECTOR dir);

	void UpdateWeaponModelData(std::string name);

	void Draw();

	int GetModelHandle(std::string name) { return m_models[name].handle; }

	/// <summary>
	/// アニメーション呼び出し（呼び出し時点で切り替えるもの）
	/// </summary>
	/// <param name="modelName">：モデルの名前</param>
	/// <param name="animationName">：アニメーションの名前</param>
	/// <param name="animationFrame">：アニメーションの再生時間</param>
	void CallAnimation(std::string modelName, std::string animationName, int animationFrame);

	/// <summary>
	/// 移行アニメーション呼び出し予約(アニメーション1周後、自動でアニメーション切り替えするためのもの)
	/// </summary>
	/// <param name="modelName">：モデルの名前</param>
	/// <param name="animationName">：アニメーションの名前</param>
	/// <param name="animationFrame">：アニメーションの再生時間</param>
	void CallTransAnimation(std::string modelName, std::string animationName, int animationFrame);

	/// <summary>
	/// アニメーションの再生時間変更
	/// </summary>
	/// <param name="modelName">：モデルの名前</param>
	/// <param name="animationFrame">：アニメーションの再生時間</param>
	void ChangeSpeedAnimation(std::string modelName, int animationFrame);

	/// <summary>
	/// 現行アニメーションの名前獲得
	/// </summary>
	/// <param name="modelName">：モデルの名前</param>
	/// <returns>：アニメーションの名前</returns>
	std::string GetPlayingAnimationName(std::string modelName);

	/// <summary>
	/// 主にアニメーションによるモデルの座標のずれを獲得
	/// </summary>
	/// <param name="modelName">：モデルの名前</param>
	/// <param name="pos">：実際の座標</param>
	/// <returns> [実座標→モデル座標]のベクトル </returns>
	VECTOR GetModelPosLag(std::string& modelName, VECTOR pos) { return VSub(m_models[modelName].pos, pos); }
	
	int getnextattach(std::string name) { return m_models[name].animation.nextAttach; }

	/// <summary>
	/// アニメーション更新
	/// </summary>
	void AnimationUpdate(std::string& modelName);
private:



	void RateAnimation(std::string& modelName);

	// アニメーション関連変数

	//// アニメーション元のモデルハンドル
	//int m_modelHandle;
	//// アニメーション現在フレーム
	//float m_animaFrame;
	//// アニメーション遷移rate
	//float m_animaRate;
	//// アニメーションインデックス
	//// 再生中アニメーション
	//int m_nowAttach;
	//// 移行先アニメーション
	//int m_nextAttach;
	//// アニメーション速度
	//float m_animaSpeed;
	
	ToonShader m_tShader;

	std::map<std::string, ModelData> m_models;
};