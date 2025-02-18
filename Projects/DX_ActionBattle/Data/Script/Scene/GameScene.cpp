#include "GameScene.h"
#include <string>
#include "Camera.h"
// プレイヤー
#include "Player.h"
// ボス（ミュータント）
#include "Mutant.h"
// 通常敵情報
#include "EnemyBase.h"
#include "EnemyStandard.h"
#include "EnemyMagic.h"
#include "EnemyTank.h"

#include "Stage.h"
#include "ChackCollision.h"

// SceneState
#include "SceneStart.h"
#include "SceneMain.h"
#include "SceneEnd.h"
#include "ScenePause.h"
#include "PauseMenu.h"

#include "EffectManager.h"
#include "SoundManager.h"
#include "PanelManager.h"


#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <iostream>

namespace
{
	constexpr int kEnemyMaxNum = 3;
	constexpr float kBarScaleStartX = 0.05f;
	constexpr float kBarScaleStartY = 0.14f;
	constexpr float kBarScaleEndX = 0.275f;
	constexpr float kBarScaleEndY = 0.24f;

	constexpr int kTutorialTimeMax = 120;

	constexpr int kEndingTime = 30;
	constexpr int kEndingMaxAlpha = 100;
}

GameScene::GameScene()
{
	SoundManager::Instance().loopPlay("Stage");

	m_sceneState = std::make_unique<SceneStart>();
	m_pPause = std::make_shared<PauseMenu>();

	// ステージ移動フラグをfalseに初期化
	for (int i = 0; i < GameData::kStageMaxLine; i++)
	{
		for (int j = 0; j < GameData::kStageMaxRow; j++)
		{
			m_isMoveMap[i][j] = false;
		}
	}
	m_nextPlayerPos = VGet(0, 10, -100);
	m_nextPlayerDir = VGet(0, 0, 1);

	// オブジェクトハンドルの初期化
	m_objectHandle.push_back(MV1LoadModel("Data/Model/Player.mv1"));
	m_objectHandle.push_back(MV1LoadModel("Data/Model/Boss_1.mv1"));
	m_objectHandle.push_back(MV1LoadModel("Data/Model/Enemy_1.mv1"));
	m_objectHandle.push_back(MV1LoadModel("Data/Model/Enemy_2.mv1"));
	m_objectHandle.push_back(MV1LoadModel("Data/Model/Enemy_3.mv1"));

	// ステージハンドルの初期化
	m_stageHandle.push_back(MV1LoadModel("Data/Model/StandardMap.mv1"));
	m_stageHandle.push_back(MV1LoadModel("Data/Model/BossStage.mv1"));

	// チュートリアルの画像ハンドルの初期化
	m_tutorialHandle.push_back(LoadGraph("Data/Image/Tutorial_L.png"));
	m_tutorialHandle.push_back(LoadGraph("Data/Image/Tutorial_X.png"));
	m_tutorialHandle.push_back(LoadGraph("Data/Image/Tutorial_A.png"));
	m_tutorialHandle.push_back(LoadGraph("Data/Image/Tutorial_B.png"));
	m_tutorialHandle.push_back(LoadGraph("Data/Image/Tutorial_Start.png"));
	m_tutorialHandle.push_back(LoadGraph("Data/Image/Tutorial_Go.png"));
	m_tutorialHandle.push_back(LoadGraph("Data/Image/Tutorial_Pow.png"));
	m_tutorialHandle.push_back(LoadGraph("Data/Image/Tutorial_R.png"));
	m_tutorialHandle.push_back(LoadGraph("Data/Image/Tutorial_RB.png"));

	m_tutorialHandle.push_back(LoadGraph("Data/Image/pin.png"));
}

GameScene::~GameScene()
{
	// 各種メモリ解放
	MV1DeleteModel(m_stageHandle[0]);
	m_stageHandle.clear();
	m_objects.clear();
	// std::map<std::string, std::shared_ptr<ObjectBase>>().swap(m_objects);
	m_stage.clear();
	m_enemyData.clear();
	m_enemyData.shrink_to_fit();
	m_pPause.reset();
	m_sceneState.reset();

	for (auto& i : m_objectHandle)
	{
		MV1DeleteModel(i);
	}

	for (auto& i : m_tutorialHandle)
	{
		DeleteGraph(i);
	}

	for (auto& i : m_stage)
	{
		i.second.reset();
		i.second.release();
	}
	m_stage.clear();

	for (auto& i : m_objects)
	{
		i.second.reset();
		i.second.release();
	}
	m_objects.clear();

	DrawingManager::Instance().DeleteAllModel();
	SoundManager::Instance().Stop("Stage");
	SoundManager::Instance().Stop("Boss");
}

void GameScene::InitScene()
{
	
	m_camera.Init(VGet(0, 80, -180));

	m_nowStageNum[0] = 4;
	m_nowStageNum[1] = 1;

	// プレイヤー情報
	m_objects.insert(std::make_pair("Player", std::make_unique<Player>()));
	m_objects["Player"] ->Init("Player", m_objectHandle[0], m_nextPlayerPos, m_nextPlayerDir);
	m_objects["Player"]->SetWeapon(m_weaponSelect);
	auto statusBarGreenHandle = LoadGraph("Data/Image/BarGreen.png");
	auto statusBarRedHandle = LoadGraph("Data/Image/BarRed.png");
	auto statusBarBackHandle = LoadGraph("Data/Image/BarBack.png"); 
	auto statusHandle = LoadGraph("Data/Image/Player_StatusBar.png");
	auto settei = LoadGraph("Data/Image/settei.png");
	auto nun = LoadGraph("Data/Image/nun.png");
	auto fire = LoadGraph("Data/Image/fire.png");
	auto sword = LoadGraph("Data/Image/sword.png");
	auto axe = LoadGraph("Data/Image/axe.png");

	PanelManager::Instance().RegisterImage("BarBack", statusBarBackHandle, GameData::kScreenWidth * kBarScaleStartX, GameData::kScreenHeight * kBarScaleStartY, GameData::kScreenWidth * kBarScaleEndX, GameData::kScreenHeight * kBarScaleEndY);
	PanelManager::Instance().RegisterImage("BarGreen", statusBarGreenHandle, GameData::kScreenWidth * kBarScaleStartX, GameData::kScreenHeight * kBarScaleStartY, GameData::kScreenWidth * kBarScaleEndX, GameData::kScreenHeight * kBarScaleEndY);
	PanelManager::Instance().RegisterImage("BarRed", statusBarRedHandle, GameData::kScreenWidth * kBarScaleStartX, GameData::kScreenHeight * kBarScaleStartY, GameData::kScreenWidth * kBarScaleEndX, GameData::kScreenHeight * kBarScaleEndY);
	PanelManager::Instance().RegisterImage("Status", statusHandle, 10, 10, GameData::kScreenWidth * 0.3f, GameData::kScreenHeight * 0.25f);

	// PanelManager::Instance().RegisterImage("Settei", settei, GameData::kScreenWidth * 0.9f, GameData::kScreenHeight * 0.03f, GameData::kScreenWidth * 0.98f, GameData::kScreenHeight * 0.18f);
	// PanelManager::Instance().RegisterImage("Nun", nun, GameData::kScreenWidth * 0.95f, GameData::kScreenHeight * 0.07f, GameData::kScreenWidth * 0.98f, GameData::kScreenHeight * 0.18f);

	PanelManager::Instance().RegisterImage("Fire", fire, GameData::kScreenWidth * kBarScaleStartX-20, GameData::kScreenHeight * kBarScaleStartY+15, GameData::kScreenWidth * kBarScaleStartX + 5, GameData::kScreenHeight * kBarScaleStartY + 55 );
	PanelManager::Instance().RegisterImage("Sword", sword, GameData::kScreenWidth * kBarScaleStartX-20, GameData::kScreenHeight * kBarScaleStartY+15, GameData::kScreenWidth * kBarScaleStartX + 5, GameData::kScreenHeight * kBarScaleStartY + 55);
	PanelManager::Instance().RegisterImage("Axe", axe, GameData::kScreenWidth * kBarScaleStartX-20, GameData::kScreenHeight * kBarScaleStartY+15, GameData::kScreenWidth * kBarScaleStartX + 5, GameData::kScreenHeight * kBarScaleStartY + 55);
	PanelManager::Instance().SetIsDrawImage("BarRed", false);

	// ステージ情報
	InitStageData(m_nowStageNum[0], m_nowStageNum[1]);

	// 敵の準備
	InitEnemyData(m_nowStageNum[0], m_nowStageNum[1]);
	SetEnemyData(m_nowStageNum[0], m_nowStageNum[1]);

	m_camera.ChangeViewMode(ViewMode::QUARTER, m_objects["Player"] ->GetPos(), m_objects["Player"] ->GetDir());
	// m_camera.SetTargetPos(m_objects["Player"]->GetPos(), m_objects["Mutant"]->GetCollider().pos[1]);
	m_camera.SetIsCameraRotate(false);

	m_light.Init(m_camera.GetPos(), m_camera.GetTargetPos());
}

void GameScene::UpdateScene()
{
	// 主操作
	if (m_sceneState->isMove)
	{
		// 勝敗確定時
		if (m_isWinEnd || m_isLoseEnd)
		{
			EndingUpdate();
		}
		// 未確定でゲーム進行時
		else
		{
			// ポーズ画面への移行
			if (m_controller.GetInputDown(Button::START))
			{
				ChangeState(SceneState::PAUSE);

				UpgradeData data;
				data.upgradePoint = m_upgradePoint;
				data.attackLevel = m_objects["Player"]->GetLevel().Atk;
				data.defenseLevel = m_objects["Player"]->GetLevel().Def;
				data.speedLevel = m_objects["Player"]->GetLevel().Agi;
				// ポーズ画面側に現在の強化ポイント情報を渡す
				m_pPause->SetUpgradePoint(data);

				// ポーズ画面展開
				m_pPause->SwitchPause();
			}
			else
			{
				// マップ移動処理
				ChackMoveMap();

				// 各オブジェクトの入力や行動の登録
				ObjectsUpdate();

				// 行動結果による当たり判定を測定
				CollisionUpdate();

				// 当たり判定を加味した上での最終行動結果の登録
				for (auto it = m_objects.begin(); it != m_objects.end(); )
				{
					auto& t = *it;
					t.second->ApplyUpdating();
					if (t.second->GetIsDelete())
					{
						if (t.first == "Player")
						{
							// プレイヤーの削除
							m_isLoseEnd = true;
							++it;
						}
						else if (t.first == "Mutant")
						{
							// ボスの削除
							m_isWinEnd = true;
							++it;
						}
						else
						{
							// 敵の削除
							DrawingManager::Instance().DeleteModel(t.first);
							m_enemyData.pop_back();
							it = m_objects.erase(it);
						}
					}
					else
					{
						++it;
					}
				}

				// 敵の残存数確認
				if (m_enemyData.size() <= 0)
				{
					// ステージクリア時だけポイント加算
					if (!m_isMoveMap[m_nowStageNum[0]][m_nowStageNum[1]] && 
						!(m_nowStageNum[0] == 0 && m_nowStageNum[1] == 1))
					{
						m_isMoveMap[m_nowStageNum[0]][m_nowStageNum[1]] = true;
						m_upgradePoint++;
						m_tutorialNum = 6;
						m_tutorialTimer = 0;
						m_isTutorialPop = true;
					}
				}
				// チュートリアルの更新
				TutorialUpdate();
			}
		}
	}

	// ポーズ画面
	m_pPause->Update();

	

	if (m_sceneState->isPause)
	{
		// ポーズ中の処理// ポーズ画面への移行
		if (m_controller.GetInputDown(Button::START))
		{
			m_pPause->SwitchPause();
		}
		// ポーズ画面の終了処理
		if (m_pPause->GetIsEndPause())
		{
 			auto data = m_pPause->GetUpgradePoint();
			m_upgradePoint = data.upgradePoint;
			Level level = { data.attackLevel, data.defenseLevel, data.speedLevel };
			m_objects["Player"]->SetLevel(level);
			ChangeState(SceneState::MAIN);
		}
		if (m_pPause->GetIsEndGame())
		{
			m_nextScene = Scene::TITLE;
			m_isMoveScene = true;
		}
		// パラメータの強化値を取得してプレイヤーに渡す
		int upgrade = m_pPause->GetUpgrade();
		if (upgrade != -1)
		{
			// 強化ポイントを減らしてプレイヤーを強化
			m_upgradePoint--;
			m_objects["Player"] ->SetUpgrade(upgrade);
		}
	}
	// プレイヤーのHP表示の更新
	PlayerStatusPop();

	// カメラの更新
	CameraUpdate();
	EffectManager::Instance().Update();

	// tennzi
	if (m_controller.GetInputDown(Button::LB) && m_controller.GetPOVDown(InputPOV::LEFT))
	{
		m_upgradePoint = 10;
	}

	// 描画
	DrawingManager::Instance().Draw();
	EffectManager::Instance().Draw();
	DrawFloor();
	PanelManager::Instance().Draw();
	// チュートリアルの描画
	TutorialDraw();

	DrawParameter();
	m_pPause->Draw();
	EndingDraw();

	// ゲーム進行(主にフェード処理)
	if (m_sceneState->isProgress)
	{
		if (m_sceneState->GetState() == SceneState::START)
		{
			//if (/* Init処理の未完了時 */)
			//{
			//	// なんか処理
			//}
			if (FadeIn(5))
			{
				// オブジェクトの参照数チェック
				for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
				{
					auto& t = *it;
				}

				ChangeState(SceneState::MAIN);
			}
		}
		else if (m_sceneState->GetState() == SceneState::END)
		{
			// ステージ更新処理等

			// フェードアウト後
			if (FadeOut(5))
			{
				// 最終ステージ時
				if (m_isLoseEnd || m_isWinEnd)
				{
					// エンディング移行
					m_nextScene = Scene::TITLE;
					m_isMoveScene = true;
					return;
				}
				// それ以外
				else
				{
					// 各方向への部屋移動
					if (m_moveRoomDir == UP)
					{
						m_nextPlayerPos = VGet(0, 0, -100);
						m_nextPlayerDir = VGet(0, 0, 1);
						m_nowStageNum[0]--;
						m_moveRoomDir = -1;
					}
					else if (m_moveRoomDir == DOWN)
					{
						m_nextPlayerPos = VGet(0, 0, 100);
						m_nextPlayerDir = VGet(0, 0, -1);
						m_nowStageNum[0]++;
						m_moveRoomDir = -1;
					}
					else if (m_moveRoomDir == RIGHT)
					{
						m_nextPlayerPos = VGet(-100, 0, 0);
						m_nextPlayerDir = VGet(1, 0, 0);
						m_nowStageNum[1]++;
						m_moveRoomDir = -1;
					}
					else if (m_moveRoomDir == LEFT)
					{
						m_nextPlayerPos = VGet(100, 0, 0);
						m_nextPlayerDir = VGet(-1, 0, 0);
						m_nowStageNum[1]--;
						m_moveRoomDir = -1;
					}

					// ステージデータの削除
					DeleteStatgeData();

					// 最終ステージへの移行以外ではステージの更新は行わない
					if (m_nowStageNum[0] == 0 && m_nowStageNum[1] == 1)
					{
						// ステージの更新
						InitStageData(m_nowStageNum[0], m_nowStageNum[1]);
					}
					// プレイヤーの準備
					// m_objects.insert(std::make_pair("Player", std::make_unique<Player>()));
					// m_objects["Player"] ->Init("Player", m_nextPlayerPos, m_nextPlayerDir);
					m_objects["Player"]->SetPosAndDir(m_nextPlayerPos, m_nextPlayerDir);

					// 敵の準備
					if (m_isMoveMap[m_nowStageNum[0]][m_nowStageNum[1]] == false)
					{
						InitEnemyData(m_nowStageNum[0], m_nowStageNum[1]);
						SetEnemyData(m_nowStageNum[0], m_nowStageNum[1]);
					}

					ChangeState(SceneState::START);
				}
			}
		}
	}
}

// ステージの更新
void GameScene::UpdateStage()
{
}

// オブジェクトの更新
void GameScene::ObjectsUpdate()
{
	/* インプット処理、各オブジェクトの行動決定パート */
	for (auto it = m_objects.begin(); it != m_objects.end(); )
	{
		auto& t1 = *it;
		// プレイヤーのとき
		if (t1.first == "Player")
		{
			auto pos = VZero();
			/// 基準距離
			float enemyRange = 100.0f;

			// 最接近の敵を検索
			for (auto its = m_objects.begin(); its != m_objects.end(); ++its)
			{
				auto& t2 = *its;
				auto vec = VSub(t2.second ->GetCollider().pos[0], t1.second ->GetCollider().pos[0]);
				if (t2.first != "Player" && VSize(vec) <= enemyRange)
				{
					enemyRange = VSize(vec);
					pos = t2.second ->GetCollider().pos[0];
				}
			}
			t1.second ->SetChaseTarget(pos);
		}
		// プレイヤー以外の時
		else
		{
			t1.second ->SetChaseTarget(m_objects["Player"] ->GetPos());
		}

		// 各オブジェクトの行動
		t1.second ->Update();

		// 死んだキャラクターの削除
		if (t1.second->GetIsDelete())
		{
			// ゲームの勝敗判定
			if (t1.first == "Player")
			{
				m_isLoseEnd = true;
				++it;
			}
			else if (t1.first == "Mutant")
			{
				m_isWinEnd = true;
				++it;
			}
			else
			{
				// 描画モデルの削除
				DrawingManager::Instance().DeleteModel(t1.first);
				m_enemyData.pop_back();
				it = m_objects.erase(it);
			}
		}
		else
		{
			++it;
		}
	}
}

void GameScene::CollisionUpdate()
{
	/* 当たり判定のチェック、各数値決定パート */

	// コライダーを獲得して
	// コライダー同士による押し出し量の決定
	for (auto it1 = m_objects.begin(); it1 != m_objects.end(); )
	{
		auto& t1 = *it1;
		// 押し出されるオブジェクト
		auto colA = t1.second ->GetCollider();
		for (auto it2 = m_objects.begin(); it2 != m_objects.end(); ++it2)
		{
			auto& t2 = *it2;
			// 比較対象が同じものでないことの確認
			if (t1 != t2)
			{
				// 押し出すオブジェクト
				auto colB = t2.second ->GetCollider();

				// ダメージ判定処理
				// t1のオブジェクトにt2のコライダー情報を渡して
				// t1の中でHIT処理してもらう
				if (t1.second ->GetIsHitAttack(colB))
				{
					// t1から取得したダメージをt2に付与
					// 敵同士の攻撃の当たり判定はなし
					if (t1.first == "Player" || (t1.first != "Player" && t2.first == "Player"))
					{
						auto damage = t1.second->GetDamagePoint();
						if (t2.second->SetDamagePoint(damage) && t1.first != "Player")
						{
							m_camera.SetVibration(10);
						}
					}
					// 当たった際のエフェクトやらなんとやらの処理
					// m_pSoundManager->OnePlay("SE_Boss_Hit");
					// m_hitStopCount = m_pPlayer->GetDamage() / 100;
					// m_pEffectManager->PlayEffect("Hit", m_pBoss->GetPos(), m_pBoss->GetDir());
				}

				// 押し出し処理
				auto VecZero = VZero();
				auto pushVec = VZero();
				if (colB.prefer > 0)
				{
					pushVec = ChackCollision::Instance().GetPushVec(colA, colB, VecZero);
				}
				t1.second ->SetPushVec(pushVec);
			}
		}

		// マップとの判定
		for (auto it2 = m_stage.begin(); it2 != m_stage.end(); ++it2)
		{
			auto& t2 = *it2;
			// 押し出しがない（マップに接していない）ときに
			// 次フレームでの接触を確認する

			if (m_nowStageNum[0] == 0 && m_nowStageNum[1] == 1 && t2.first == "Stage")
			{
				continue;
			}

			// 壁当たり判定(応急処置)
			auto move = VZero();
			// 通常ステージ
			if (t2.first == "Stage")
			{
				auto pos = t1.second ->GetPos();
				// 壁
				if (pos.x < -130)
				{
					move.x = -130 - pos.x;
				}
				if (pos.x > 130)
				{
					move.x = 130 - pos.x;
				}
				if (pos.z < -130)
				{
					move.z = -130 - pos.z;
				}
				if (pos.z > 130)
				{
					move.z = 130 - pos.z;
				}
			}
			// ボスステージ
			else
			{
				auto pos = t1.second->GetPos();
				// 壁
				if (pos.x < -160)
				{
					move.x = -160 - pos.x;
				}
				if (pos.x > 160)
				{
					move.x = 160 - pos.x;
				}
				if (pos.z < -200)
				{
					move.z = -200 - pos.z;
				}
				if (pos.z > 200)
				{
					move.z = 200 - pos.z;
				}
			}

			colA = t1.second ->GetCollider();
			auto colB = t2.second->GetCollider();
			auto colAPreMoveVec = t1.second ->GetSemiUpdateVec();
			auto pushVec = VAdd(ChackCollision::Instance().GetPushVec(colA, colB, colAPreMoveVec), move);
			t1.second ->SetFloorPushVec(pushVec);
			// 次のフレームで床にあたる判定の獲得
			t1.second ->SetIsToGround(ChackCollision::Instance().GetIsToGround(colA, colB, colAPreMoveVec));
		}

		// 復帰処理
		if (t1.second->GetPos().y < - 50)
		{
			t1.second->SetPosAndDir(VGet(0, 10, 0), VGet(0, 0, 1));
		}

		++it1;
	}
}

void GameScene::TutorialUpdate()
{
	// 表示フレームのカウント
	if (m_tutorialTimer < kTutorialTimeMax)
	{
		m_tutorialTimer++;
	}

	// チュートリアルの切り替え処理
	auto deltaAlpha = 10;
	// タイマー満期でフェードアウト
	if (m_tutorialTimer == kTutorialTimeMax && m_isTutorial)
	{
		if (m_tutorialAlpha > 0)
		{
			m_tutorialAlpha -= deltaAlpha;
		}
		else
		{
			m_tutorialAlpha = 0;
			m_isTutorial = false;
			// チュートリアルの切り替え
			if (m_nowStageNum[0] == 4 && m_nowStageNum[1] == 1)
			{
				if (m_tutorialNum < 5)
					m_tutorialNum++;
				else
					m_tutorialNum = 0;
			}
			else if(m_nowStageNum[0] == 0 && m_nowStageNum[1] == 1)
			{
				if (m_tutorialNum < 8)
					m_tutorialNum++;
				else
					m_isTutorialPop = false;
			}
			else
			{
				m_tutorialNum = 6;
				m_isTutorialPop = false;
			}
		}
	}

	// 表示開始
	if (m_isTutorial == false && m_isTutorialPop)
	{
		if (m_tutorialAlpha < 255)
		{
			m_tutorialAlpha += deltaAlpha;
		}
		else
		{
			m_tutorialAlpha = 255;
			
			m_isTutorial = true; 
			// 完全表示状態からタイマー計測開始
			m_tutorialTimer = 0;
		}
	}

}

void GameScene::TutorialDraw()
{
	// アルファ値の設定
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_tutorialAlpha);

	// チュートリアルの描画
	if (m_tutorialNum == 0)
	{
		DrawGraph(GameData::kScreenWidth * 0.3f, GameData::kScreenHeight * 0.7f, m_tutorialHandle[0], true);
	}
	else if (m_tutorialNum == 1)
	{
		DrawGraph(GameData::kScreenWidth * 0.3f, GameData::kScreenHeight * 0.7f, m_tutorialHandle[1], true);
	}
	else if (m_tutorialNum == 2)
	{
		DrawGraph(GameData::kScreenWidth * 0.3f, GameData::kScreenHeight * 0.7f, m_tutorialHandle[2], true);
	}
	else if (m_tutorialNum == 3)
	{
		DrawGraph(GameData::kScreenWidth * 0.3f, GameData::kScreenHeight * 0.7f, m_tutorialHandle[3], true);
	}
	else if (m_tutorialNum == 4)
	{
		DrawGraph(GameData::kScreenWidth * 0.3f, GameData::kScreenHeight * 0.7f, m_tutorialHandle[4], true);
	}
	else if (m_tutorialNum == 5)
	{
		DrawGraph(GameData::kScreenWidth * 0.3f, GameData::kScreenHeight * 0.7f, m_tutorialHandle[5], true);
	}
	else if (m_tutorialNum == 6)
	{
		DrawGraph(GameData::kScreenWidth * 0.2f, GameData::kScreenHeight * 0.7f, m_tutorialHandle[6], true);
	}
	else if (m_tutorialNum == 7)
	{
		DrawGraph(GameData::kScreenWidth * 0.3f, GameData::kScreenHeight * 0.7f, m_tutorialHandle[7], true);
	}
	else if (m_tutorialNum == 8)
	{
		DrawGraph(GameData::kScreenWidth * 0.3f, GameData::kScreenHeight * 0.7f, m_tutorialHandle[8], true);
	}

	// アルファ値もどし
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
}

void GameScene::CameraUpdate()
{
	// カメラ関係
	int size = m_objects.size();
	m_camera.SetTargetPos(m_objects["Player"] ->GetPos(), VZero());

	if (m_nowStageNum[0] == 0 && m_nowStageNum[1] == 1)
	{
		if (m_controller.GetInputDown(Button::RB))
		{
			m_camera.ChangeIsTarget();
		}
		if (m_objects.find("Mutant") != m_objects.end())
		{
			m_camera.SetTargetPos(m_objects["Player"]->GetPos(), m_objects["Mutant"]->GetCollider().pos[1]);
		}
	}

	for (auto& t : m_objects)
	{
		t.second->SetCameraDir(m_camera.GetCameraDir());
	}

	m_camera.Update();
}

void GameScene::ChackMoveMap()
{
	// 現在マップの敵が全滅してマップ移動可能かどうかのチェック
	if (!(m_nowStageNum[0] == 0 && m_nowStageNum[1] == 1) &&
		m_isMoveMap[m_nowStageNum[0]][m_nowStageNum[1]] || (m_nowStageNum[0] == 4 && m_nowStageNum[1] == 1))
	{
		auto pos = m_objects["Player"] ->GetPos();

		// どの方向に進めるかのチェック
		if ((m_nowStageNum[0] > 0 && m_nowStageNum[0] < 4) && m_nowStageNum[1] > 0)															// 左
		{
			// プレイヤーの座標チェック
			if (pos.x <= -130 && abs(pos.z) < 15)
			{
				m_moveRoomDir = LEFT;
				ChangeState(SceneState::END);
			}
		}
		if ((m_nowStageNum[0] > 0 && m_nowStageNum[0] < 4) && m_nowStageNum[1] < GameData::kStageMaxRow - 1)								// 右
		{
			// プレイヤーの座標チェック
			if (pos.x >= 130 && abs(pos.z) < 15)
			{
				m_moveRoomDir = RIGHT;
				ChangeState(SceneState::END);
			}
		}
		if (m_nowStageNum[0] > 0 && !(m_nowStageNum[0] == 1 && m_nowStageNum[1] != 1))														// 奥
		{
			// プレイヤーの座標チェック
			if (abs(pos.x) < 15 && pos.z >= 130)
			{
				m_moveRoomDir = UP;
				ChangeState(SceneState::END);
			}
		}
		if (m_nowStageNum[0] < GameData::kStageMaxLine - 1 && !(m_nowStageNum[0] == GameData::kStageMaxLine - 2 && m_nowStageNum[1] != 1))	// 手前
		{
			// プレイヤーの座標チェック
			if (abs(pos.x) < 15 && pos.z <= -130)
			{
				m_moveRoomDir = DOWN;
				ChangeState(SceneState::END);
			}
		}
	}
}

void GameScene::DeleteStatgeData()
{
	// オブジェクトを全て削除
	// プレイヤーの描画モデルの削除
	// DrawingManager::Instance().DeleteModel("Player");

	// オブジェクトの削除
	// m_objects.clear();
	// std::map<std::string, std::shared_ptr<ObjectBase>>().swap(m_objects);
	// プレイヤー以外のオブジェクトの削除
	for (auto it = m_objects.begin(); it != m_objects.end(); )
	{
		auto& t = *it;
		if (t.first != "Player")
		{
			// 描画モデルの削除
			DrawingManager::Instance().DeleteModel(t.first);
			it = m_objects.erase(it);
		}
		else
		{
			++it;
		}
	}


	m_setEnemyNum = 0;
}

void GameScene::InitStageData(int stageLine, int stageRow)
{
	// ステージ本体の準備
	// ボスステージ
	if (stageLine == 0 && stageRow == 1)
	{
		SoundManager::Instance().Stop("Stage");
		SoundManager::Instance().loopPlay("Boss");
		// ステージセットアップ
		m_stage.insert(std::make_pair("Boss", std::make_unique<Stage>()));
		m_stage["Boss"]->InitStage("Boss", m_stageHandle[1]);
		m_stage["Stage"]->SetIsUse(false);
		DrawingManager::Instance().SetIsDrawModel("Stage", false);

		// カメラ更新
		m_camera.ChangeViewMode(ViewMode::TPS, m_objects["Player"]->GetPos(), m_objects["Player"]->GetDir());
		m_camera.SetIsCameraRotate(true);
		m_camera.ChangeIsTarget();

		// チュートリアルの準備
		m_tutorialNum = 7;
		m_tutorialTimer = 0;
		m_isTutorialPop = true;
	}
	// それ以外のステージ
	else
	{
		m_stage.insert(std::make_pair("Stage", std::make_unique<Stage>()));
		m_stage["Stage"]->InitStage("Stage", m_stageHandle[0]);
	}

}

void GameScene::InitEnemyData(int stageLine, int stageRow)
{
	// 前データを削除
	m_enemyData.clear();

	// ボス部屋
	if (stageLine == 0)
	{
		// ボスの情報

	}
	// 通常部屋
	else if (stageLine == 1)
	{
		if (stageRow == 0)
		{
			m_enemyData.push_back(EnemyKind::MAGIC);
			m_enemyData.push_back(EnemyKind::MAGIC);
			m_enemyData.push_back(EnemyKind::MAGIC);
		}
		else if (stageRow == 1)
		{
			m_enemyData.push_back(EnemyKind::STANDARD);
			m_enemyData.push_back(EnemyKind::MAGIC);
			m_enemyData.push_back(EnemyKind::TANK);
		}
		else
		{
			m_enemyData.push_back(EnemyKind::TANK);
			m_enemyData.push_back(EnemyKind::TANK);
			m_enemyData.push_back(EnemyKind::TANK);
		}
	}
	else if (stageLine == 2)
	{
		if (stageRow == 0)
		{
			m_enemyData.push_back(EnemyKind::MAGIC);
			m_enemyData.push_back(EnemyKind::STANDARD);
			m_enemyData.push_back(EnemyKind::MAGIC);
		}
		else if (stageRow == 1)
		{
			m_enemyData.push_back(EnemyKind::STANDARD);
			m_enemyData.push_back(EnemyKind::STANDARD);
		}
		else
		{
			m_enemyData.push_back(EnemyKind::TANK);
			m_enemyData.push_back(EnemyKind::STANDARD);
			m_enemyData.push_back(EnemyKind::TANK);
		}
	}
	else if (stageLine == 3)
	{
		if (stageRow == 0)
		{
			m_enemyData.push_back(EnemyKind::STANDARD);
			m_enemyData.push_back(EnemyKind::MAGIC);
		}
		else if (stageRow == 1)
		{
			m_enemyData.push_back(EnemyKind::STANDARD);
		}
		else
		{
			m_enemyData.push_back(EnemyKind::STANDARD);
			m_enemyData.push_back(EnemyKind::TANK);
		}
	}
	// チュートリアル部屋
	else if (stageLine == 4)
	{
		if (stageRow == 1)
		{
			m_enemyData.push_back(EnemyKind::STANDARD);
		}
	}
}

void GameScene::SetEnemyData(int stageLine, int stageRow)
{
	auto enemyNum = m_enemyData.size();

	// ボスマップ以外
	if (!(stageLine == 0 && stageRow == 1))
	{
		// 敵の配置
		for (int i = 0; i < kEnemyMaxNum; i++)
		{
			VECTOR enemyPos;

			// エネミーの配置座標
			if (i == 0)
			{
				enemyPos = VGet(0, 10, -10);
			}
			else if (i == 1)
			{
				enemyPos = VGet(-75, 10, 50);
			}
			else
			{
				enemyPos = VGet(75, 10, 50);
			}

			// 配置するエネミーの情報
			if (enemyNum > i)
			{
				if (m_enemyData[i] == EnemyKind::STANDARD)
				{
					m_setEnemyNum++;
					std::string name = "Enemy_" + std::to_string(m_setEnemyNum);
					m_objects.insert(std::make_pair(name, std::make_unique<EnemyStandard>()));
					if (m_nowStageNum[0] == 4 && m_nowStageNum[1] == 1)
					{
						m_objects[name]->SetIsLock();
					}
					m_objects[name]->Init(name, m_objectHandle[2], enemyPos, VGet(0, 0, 1));
				}
				else if (m_enemyData[i] == EnemyKind::TANK)
				{
					m_setEnemyNum++;
					std::string name = "Enemy_" + std::to_string(m_setEnemyNum);
					m_objects.insert(std::make_pair(name, std::make_unique<EnemyTank>()));
					m_objects[name]->Init(name, m_objectHandle[3], enemyPos, VGet(0, 0, 1));
				}
				else
				{
					m_setEnemyNum++;
					std::string name = "Enemy_" + std::to_string(m_setEnemyNum);
					m_objects.insert(std::make_pair(name, std::make_unique<EnemyMagic>()));
					m_objects[name]->Init(name, m_objectHandle[4], enemyPos, VGet(0, 0, 1));
				}
			}
		}
	}
	// ボスマップ
	else
	{
		// ボスの配置
		VECTOR bossPos = VGet(0, 10, 120);
		m_objects.insert(std::make_pair("Mutant", std::make_unique<Mutant>()));
		m_objects["Mutant"]->Init("Mutant", m_objectHandle[1], bossPos, VGet(0, 0, 1));
	}
	
}

void GameScene::PlayerStatusPop()
{
	// プレイヤーのHP割合
	auto playerHpRate = m_objects["Player"]->GetParameter().GetHPRate();
	// HPバーの座標の更新
	auto UpdateEndX = GameData::kScreenWidth * kBarScaleStartX + (GameData::kScreenWidth * kBarScaleEndX - GameData::kScreenWidth * kBarScaleStartX) * playerHpRate;
	// HPバーの位置調整
	PanelManager::Instance().UpdateImagePos("BarGreen", GameData::kScreenWidth * kBarScaleStartX, GameData::kScreenHeight * kBarScaleStartY, UpdateEndX, GameData::kScreenHeight * kBarScaleEndY);
	PanelManager::Instance().UpdateImagePos("BarRed", GameData::kScreenWidth * kBarScaleStartX, GameData::kScreenHeight * kBarScaleStartY, UpdateEndX, GameData::kScreenHeight * kBarScaleEndY);
	// HPバーの色の変更
	if (playerHpRate < 0.25f)
	{
		PanelManager::Instance().SetIsDrawImage("BarRed", true);
		PanelManager::Instance().SetIsDrawImage("BarGreen", false);
	}
	else
	{
		PanelManager::Instance().SetIsDrawImage("BarRed", false);
		PanelManager::Instance().SetIsDrawImage("BarGreen", true);
	}
}

void GameScene::DrawParameter()
{
	// フォント変更
	ChangeFontFromHandle(Font::Instance().GetFontHandle(Font::FontType::BOLD, 1));

	auto color = GetColor(10, 10, 10);
	auto selectColor = GetColor(200, 200, 0);
	auto edgeColor = GetColor(255, 255, 255);

	// プレイヤーのレベル表記
	Level playerLevel = m_objects["Player"]->GetLevel();
	// プレイヤーの攻撃レベル表記
	auto playerAtkLevel = playerLevel.Atk;
	DrawFormatString2F(GameData::kScreenWidth * 0.06f, GameData::kScreenHeight * 0.07f, color, edgeColor, "LV.%d", playerAtkLevel);
	// プレイヤーの防御レベル表記
	auto playerDefLevel = playerLevel.Def;
	DrawFormatString2F(GameData::kScreenWidth * 0.145f, GameData::kScreenHeight * 0.07f, color, edgeColor, "LV.%d", playerDefLevel);
	// プレイヤーのスピードレベル表記
	auto playerAgiLevel = playerLevel.Agi;
	DrawFormatString2F(GameData::kScreenWidth * 0.23f, GameData::kScreenHeight * 0.07f, color, edgeColor, "LV.%d", playerAgiLevel);
}

void GameScene::DrawFloor()
{
	// アルファ値の設定
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);

	DrawBoxAA(GameData::kScreenWidth -50 - 10 - 100,	20,			GameData::kScreenWidth - 50 - 10 - 50,		20 + 50,	   GetColor(200, 200, 200), true);
																																		
	DrawBox(GameData::kScreenWidth -50 - 20 - 150,	20 + 50 + 10,	GameData::kScreenWidth - 50 - 20 - 100,		20 + 10 + 100, GetColor(200, 200, 200), true);
	DrawBox(GameData::kScreenWidth -50 - 10 - 100,	20 + 50 + 10,	GameData::kScreenWidth - 50 - 10 - 50,		20 + 10 + 100, GetColor(200, 200, 200), true);
	DrawBox(GameData::kScreenWidth -50 - 50,		20 + 50 + 10,	GameData::kScreenWidth - 50,				20 + 10 + 100, GetColor(200, 200, 200), true);
																																		
	DrawBox(GameData::kScreenWidth - 50 - 20 - 150, 20  + 100+ 20,	GameData::kScreenWidth - 50 - 20 - 100,		20 + 20 + 150, GetColor(200, 200, 200), true);
	DrawBox(GameData::kScreenWidth - 50 - 10 - 100, 20  + 100+ 20,	GameData::kScreenWidth - 50 - 10 - 50,		20 + 20 + 150, GetColor(200, 200, 200), true);
	DrawBox(GameData::kScreenWidth - 50 - 50,		20  + 100+ 20,	GameData::kScreenWidth - 50,				20 + 20 + 150, GetColor(200, 200, 200), true);
																																		
	DrawBox(GameData::kScreenWidth - 50 - 20 - 150, 20 + 150 + 30,	GameData::kScreenWidth - 50 - 20 - 100,		20 + 30 + 200, GetColor(200, 200, 200), true);
	DrawBox(GameData::kScreenWidth - 50 - 10 - 100, 20 + 150 + 30,	GameData::kScreenWidth - 50 - 10 - 50,		20 + 30 + 200, GetColor(200, 200, 200), true);
	DrawBox(GameData::kScreenWidth - 50 - 50,		20 + 150 + 30,	GameData::kScreenWidth - 50,				20 + 30 + 200, GetColor(200, 200, 200), true);
																																		
	DrawBox(GameData::kScreenWidth - 50 - 10 - 100,	20 + 200 + 40,	GameData::kScreenWidth - 50 - 10 - 50,		20 + 40 + 250, GetColor(200, 200, 200), true);

	// アルファ値もどし
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

	float posX = GameData::kScreenWidth - 50 - (3 - m_nowStageNum[1]) * 50 - (2 - m_nowStageNum[1]) * 10 + 15;
	float posY = 20 + 60 * m_nowStageNum[0] + 15;

	DrawExtendGraph(posX,posY, posX + 50 - 30, posY + 50 - 25, m_tutorialHandle[9], true);

}

void GameScene::EndingUpdate()
{
	// エンディングの処理 相打ち時は勝ち優先
	// 共通処理
	if (m_isLoseEnd || m_isWinEnd)
	{
		if (m_endingAlpha < kEndingMaxAlpha)
		{
			m_endingAlpha += 2;
		}
		else
		{
			m_endingAlpha = kEndingMaxAlpha;
		}
	}

	if (m_isWinEnd)
	{
		if (m_endingTimer == kEndingTime)
		{
			if (m_controller.GetInputDown(Button::A))
			{
				ChangeState(SceneState::END);
				return;
			}
		}
		else
		{
			m_endingTimer++;
		}
	}
	else if (m_isLoseEnd)
	{
		if (m_endingTimer == kEndingTime)
		{
			if (m_controller.GetInputDown(Button::A))
			{
				ChangeState(SceneState::END);
				return;
			}
		}
		else
		{
			m_endingTimer++;
		}
	}
}

void GameScene::EndingDraw()
{
	auto timeRange = m_endingTimer;
	if (m_endingTimer > kEndingTime / 3)
	{
		timeRange = kEndingTime / 3;
	}
	auto range = static_cast<float>(GameData::kScreenHalfHeight / (kEndingTime * 3)) * timeRange;

	// エンディングの描画
	if (m_isWinEnd)
	{
		// アルファ値の設定
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_endingAlpha);
		DrawBox(0, 0, GameData::kScreenWidth + 1, GameData::kScreenHeight + 1, 0x000000, true);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

		DrawBoxAA(0, GameData::kScreenHalfHeight - range - 2, GameData::kScreenWidth + 1, GameData::kScreenHalfHeight + range + 2, 0xf0f0f0, true);
		DrawBoxAA(0, GameData::kScreenHalfHeight - range, GameData::kScreenWidth+1, GameData::kScreenHalfHeight + range, 0xa0a000, true);
		if(m_endingTimer == kEndingTime)
			DrawFormatString2F(GameData::kScreenWidth * 0.45f, GameData::kScreenHeight * 0.47f, 0xffffff, 0x202020, "  WIN");
	}
	else if (m_isLoseEnd)
	{
		// アルファ値の設定
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_endingAlpha);
		DrawBox(0, 0, GameData::kScreenWidth + 1, GameData::kScreenHeight + 1, 0x000000, true);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

		DrawBoxAA(0, GameData::kScreenHalfHeight - range - 2, GameData::kScreenWidth + 1, GameData::kScreenHalfHeight + range + 2, 0xf0f0f0, true);
		DrawBoxAA(0, GameData::kScreenHalfHeight - range, GameData::kScreenWidth + 1, GameData::kScreenHalfHeight + range, 0x000030, true);
		if (m_endingTimer == kEndingTime)
			DrawFormatString2F(GameData::kScreenWidth * 0.45f, GameData::kScreenHeight * 0.47f, 0x101010, 0xffffff, "Lose...");
	}
}
