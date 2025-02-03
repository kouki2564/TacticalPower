#pragma once
#include "StateBase.h"
class Guard :
    public StateBase
{
public:
	// この関数内で各Stateの全ての処理を行う
	void StateUpdate()  ;

protected:

	// 各Stateでの動作処理
	void ActionUpdate()  ;
	// アニメーション動作
	void PlayAnimation()  ;
	// SE再生
	void PlaySE()  ;
	// エフェクト
	void PlayEffect()  ;

private:
};

