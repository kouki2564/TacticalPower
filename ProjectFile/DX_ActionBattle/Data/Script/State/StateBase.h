#pragma once
#include "Controller.h"
#include "Physics.h"
#include <memory>

enum class State
{
	MOVE,
	ATTACK,
	DODGE,
	ADJUST,
	KNOCK,
	JUMP,
	DEATH
};

class StateBase
{
public:
	StateBase(){}
	~StateBase(){}

	State GetState() { return state; }

	/* 各状態移行の許可項目 */
	// ダメージをうけるか
	bool isOnDamage = false;
	// 動けるか
	bool isMove = false;
	// 動作方向を変えられるか
	bool isMoveDir = false;
	// ノックバックを受けるか
	bool isKnockBack = false;
	// 攻撃に移れるか
	bool isAttack = false;
	// 回避できるか
	bool isDodge = false;
	// ジャンプできるか
	bool isJump = false;

	State state = State::MOVE;

};

