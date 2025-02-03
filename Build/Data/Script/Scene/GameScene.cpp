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

namespace
{
	constexpr int kEnemyMaxNum = 3;
}

GameScene::GameScene()
{
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
	// ステージハンドルの初期化
	m_stageHandle.push_back(MV1LoadModel("Data/Model/StandardMap.mv1"));

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
}

void GameScene::InitScene()
{

	m_camera.Init(VGet(0, 80, -180));

	m_nowStageNum[0] = 4;
	m_nowStageNum[1] = 1;

	// プレイヤー情報
	m_objects.insert(std::make_pair("Player", std::make_unique<Player>()));
	m_objects["Player"] ->Init("Player", m_nextPlayerPos, m_nextPlayerDir);
	auto statusHandle = LoadGraph("Data/Image/Player_StatusBar.png");
	PanelManager::Instance().RegisterImage("Status", statusHandle, 10, 10, GameData::kScreenWidth * 0.3f, GameData::kScreenHeight * 0.25f);

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
		// ポーズ画面への移行
		if (m_controller.GetInputDown(Button::START))
		{
			ChangeState(SceneState::PAUSE);

			// ポーズ画面側に現在の強化ポイント情報を渡す
			m_pPause->SetUpgradePoint(m_upgradePoint);

			// ポーズ画面展開
			m_pPause->SwitchPause();
		}
		else
		{
			// 各オブジェクトの入力や行動の登録
			ObjectsUpdate();

			// 行動結果による当たり判定を測定
			CollisionUpdate();

			// 当たり判定を加味した上での最終行動結果の登録
			for (auto it = m_objects.begin(); it != m_objects.end(); )
			{
				auto& t = *it;
				t.second ->ApplyUpdating();
				if (t.second ->GetIsDelete())
				{
					// 描画モデルの削除
					DrawingManager::Instance().DeleteModel(t.first);
					m_enemyData.pop_back();
					it = m_objects.erase(it);
				}
				else
				{
					++it;
				}
			}

			// 敵の残存数確認
			if (m_enemyData.size() <= 0)
			{
				m_isMoveMap[m_nowStageNum[0]][m_nowStageNum[1]] = true;
				// ポイント追加
				m_upgradePoint++;
			}

			// マップ移動処理
			ChackMoveMap();
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
			ChangeState(SceneState::MAIN);
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

	// カメラの更新
	CameraUpdate();

	// 描画
	DrawingManager::Instance().Draw();
	PanelManager::Instance().Draw();
	m_pPause->Draw();

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
				if (m_nowStageNum[0] == 0 && m_nowStageNum[1] == 1)
				{
					// エンディング移行
					m_nextScene = Scene::TITLE;	// かりおき
					m_isMoveScene = true;
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
					m_objects.insert(std::make_pair("Player", std::make_unique<Player>()));
					m_objects["Player"] ->Init("Player", m_nextPlayerPos, m_nextPlayerDir);

					// 敵の準備
					if (m_isMoveMap[m_nowStageNum[0]][m_nowStageNum[1]] == false)
					{
						InitEnemyData(m_nowStageNum[0], m_nowStageNum[1]);
						SetEnemyData(m_nowStageNum[0], m_nowStageNum[1]);
					}

					ChangeState(SceneState::START);
				}

				// 
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
			float enemyRange = 1000.0f;

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
		if (t1.second ->GetIsDelete())
		{
			// 描画モデルの削除
			DrawingManager::Instance().DeleteModel(t1.first);
			m_enemyData.pop_back();
			it = m_objects.erase(it);
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
						t2.second ->SetDamagePoint(t1.second ->GetDamagePoint());
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

			// 通常ステージの壁当たり判定
			auto move = VZero();
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

			colA = t1.second ->GetCollider();
			auto colB = t2.second->GetCollider();
			auto colAPreMoveVec = t1.second ->GetSemiUpdateVec();
			auto pushVec = VAdd(ChackCollision::Instance().GetPushVec(colA, colB, colAPreMoveVec), move);
			t1.second ->SetFloorPushVec(pushVec);
			// 次のフレームで床にあたる判定の獲得
			t1.second ->SetIsToGround(ChackCollision::Instance().GetIsToGround(colA, colB, colAPreMoveVec));
		}

		// 死んだキャラクターの削除
		if (t1.second ->GetIsDelete())
		{
			// 描画モデルの削除
			DrawingManager::Instance().DeleteModel(t1.first);
			m_enemyData.pop_back();
			it1 = m_objects.erase(it1);
		}
		else
		{
			++it1;
		}
	}
}

void GameScene::CameraUpdate()
{

	// カメラ関係
	int size = m_objects.size();
	m_camera.SetTargetPos(m_objects["Player"] ->GetPos(), VZero());
	m_objects["Player"] ->SetCameraDir(m_camera.GetCameraDir());


	m_camera.Update();
}

void GameScene::ChackMoveMap()
{
	// 現在マップの敵が全滅してマップ移動可能かどうかのチェック
	if (m_isMoveMap[m_nowStageNum[0]][m_nowStageNum[1]])
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
	DrawingManager::Instance().DeleteModel("Player");

	// オブジェクトの削除
	m_objects.clear();
	// std::map<std::string, std::shared_ptr<ObjectBase>>().swap(m_objects);


	m_setEnemyNum = 0;
}

void GameScene::InitStageData(int stageLine, int stageRow)
{
	// ステージ本体の準備
	// ボスステージ
	if (stageLine == 0 && stageRow == 1)
	{

	}
	// それ以外のステージ
	else
	{
		auto stageData = std::make_shared<Stage>();
		stageData->SetStageHandle(m_stageHandle[0]);

		m_stage.insert(std::make_pair("Stage", stageData));
		m_stage["Stage"]->Init("Stage", VGet(0, 0, 0), VGet(0, 0, 1));
		stageData.reset();
		stageData = nullptr;
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
			m_enemyData.push_back(EnemyKind::TANK);
		}
	}
}

void GameScene::SetEnemyData(int stageLine, int stageRow)
{
	auto enemyNum = m_enemyData.size();

	for (int i = 0; i < kEnemyMaxNum; i++)
	{
		VECTOR enemyPos;

		// エネミーの配置座標
		if (i == 0)
		{
			enemyPos = VGet(0, 10, 0);
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
				m_objects[name]->Init(name, enemyPos, VGet(0, 0, -1));
			}
			else if (m_enemyData[i] == EnemyKind::TANK)
			{
				m_setEnemyNum++;
				std::string name = "Enemy_" + std::to_string(m_setEnemyNum);
				m_objects.insert(std::make_pair(name, std::make_unique<EnemyTank>()));
				m_objects[name]->Init(name, enemyPos, VGet(0, 0, -1));
			}
			else
			{
				m_setEnemyNum++;
				std::string name = "Enemy_" + std::to_string(m_setEnemyNum);
				m_objects.insert(std::make_pair(name, std::make_unique<EnemyMagic>()));
				m_objects[name]->Init(name, enemyPos, VGet(0, 0, -1));
			}
		}
	}
	
}
