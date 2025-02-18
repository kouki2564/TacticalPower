#pragma once

// 攻撃等の行動類は1か所にまとめて
// そこから引っ張ったほうがいいのかなと
// 考えてみたりみなかったり他の実装を優先したり。

//namespace
//{
//	// モデルデータ情報
//	constexpr float kRadius = 30.0f;
//
//	// アイドル時間
//	constexpr int kIdleTime = 250;
//
//	// 追跡最大時間
//	constexpr int kChaseTime = 600;
//
//	// 移動速度
//	constexpr float kMovePow = 0.4f;
//
//	// 攻撃ロール最大回数
//	constexpr int kAttackNumMax = 8;
//
//	// パンチ攻撃時間
//	constexpr int kPunchTime = 90;
//
//	// ブレス攻撃時間
//	constexpr int kBreathTime = 200;
//
//	// ジャンプ前予備動作時間
//	constexpr int kPreJumpTime = 80;
//
//	// ジャンプ攻撃時間
//	constexpr int kJumpAttackTime = 150;
//
//	// 強攻撃のジャンプ回数
//	constexpr int kMaxJumpCount = 3;
//
//	// 疲れるまでの強攻撃回数
//	constexpr int kToFatigueCount = 3;
//
//	// 疲労による停止時間
//	constexpr int kFatigueTime = 500;
//
//}
//
//struct MutantMove
//{
//	struct Punch
//	{
//		void init()
//		{
//			if (m_attackNum == Attack::NONE && VSize(toTargetVec) <= 80)
//			{
//				m_state->state = State::ATTACK;
//				m_attackNum = Attack::PUNCH;
//
//				m_waitTimeMax = kPunchTime;
//				m_waitTimer = 0;
//			}
//		}
//	};
//};