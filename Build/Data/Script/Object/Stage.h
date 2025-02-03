#pragma once
#include "ObjectBase.h"
class Stage :
	public ObjectBase
{
public:
	Stage();
	~Stage();
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init(std::string name, VECTOR pos, VECTOR dir);

	/// <summary>
	/// キャラ周り全体の更新
	/// </summary>
	void Update(){}

	/// <summary>
	/// 最終的な更新内容の反映
	/// </summary>
	void ApplyUpdating(){}

	void SetStageHandle(int handle);

	void SetCameraDir(VECTOR cameraDir){}

	void SetChaseTarget(VECTOR targetPos) {}

	bool GetIsHitAttack(Collider& colOther) { return false; }

	void SetDamagePoint(float damagePoint) {}

	void SetUpgrade(int paramNum) {}

	/// <summary>
	/// ノックバックベクトルの設定（knockBackだけほかで使うため、セッター配置）
	/// </summary>
	/// <param name="vec">ノックバックベクトル</param>
	void SetKnockVec(VECTOR vec) { m_physics.SetKnockVec(vec); }

	/// <summary>
	/// コライダー情報の受け渡し
	/// </summary>
	/// <returns>コライダー</returns>
	Collider GetCollider() { return m_collider; }


	// ハンドル取得
	const int GetHandle() const { return m_handle; }

	void SetPosAndDir(VECTOR pos, VECTOR dir);

protected:
};

