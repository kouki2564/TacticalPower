﻿#pragma once
#include "DxLib.h"

enum class ColliderForm
{
	NONE,
	SPHERE,
	CAPSULE,
	MAP
};

/// <summary>
/// 当たり判定データクラス
/// </summary>
class Collider
{
public:
	Collider();


	/* 各種コライダーの登録 */

	/// <summary>
	/// 球状の判定を登録
	/// </summary>
	/// <param name="setPos"> ：判定の中心座標 </param>
	/// <param name="prefer">：当たり判定をとる優先度値（-1:貫通物　0:不動物　1:重い物　2:プレイヤー(基準)　3:軽い物）</param>
	/// <param name="radius">：球の半径 </param>
	void SetSphere(VECTOR _setPos,  int _prefer, float _radius);

	/// <summary>
	/// カプセル状の判定を登録
	/// </summary>
	/// <param name="setPos"> ：判定の中心座標 </param>
	/// <param name="prefer">：当たり判定をとる優先度値（-1:貫通物　0:不動物　1:重い物　2:プレイヤー(基準)　3:軽い物）</param>
	/// <param name="radius">：球の半径（球2つとも統一） </param>
	/// <param name="height">：２球それぞれの中心からのY軸幅</param>
	void SetCapsule(VECTOR _setPos1, VECTOR _setPos2, int _prefer, float _radius);

	/// <summary>
	/// マップモデルの判定
	/// </summary>
	/// <param name="_mapHandle"></param>
	/// <param name="_scale"></param>
	void SetMap(int _mapHandle, VECTOR _scale);

	/// <summary>
	/// コライダー形状の受け渡し
	/// </summary>
	/// <returns>コライダー形状</returns>
	ColliderForm GetCollisionForm() { return form; }

	/// <summary>
	/// コライダーの位置更新
	/// </summary>
	/// <param name="moveVec">移動量</param>
	void ColliderUpdate(VECTOR _moveVec);

	// コライダーの形状
	ColliderForm form;

	// マップモデルのハンドル
	int mapHandle;

	bool isGround = false;

	// Collisionの座標
	VECTOR pos[2];
	// 当たり判定をとる優先度値（-1:貫通物　0:不変物　1:重い物　2:プレイヤー　3:軽い物）
	int prefer;
	// 球の半径（カプセルの場合は球2つとも統一）
	float radius;
	//カプセルの中心座標
	VECTOR centerPos;

	// 他コライダーとの当たり判定をとる
	bool isChackOther = true;
	// ダメージを受けるかの判定
	bool isOnDamage = false;
};

