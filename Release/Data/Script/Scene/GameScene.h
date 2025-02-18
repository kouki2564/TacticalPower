﻿#pragma once
#include "SceneBase.h"
#include "EnemyBase.h"
#include <vector>


class GameScene :
    public SceneBase
{
public:
    GameScene();
    ~GameScene();

    void InitScene();
    // 全体処理
    void UpdateScene();

    // ステージの更新
    void UpdateStage();

private:
    // オブジェクトの更新
    void ObjectsUpdate();

    // 当たり判定
    void CollisionUpdate();

    // チュートリアルのアップデート
	void TutorialUpdate();

	// チュートリアルの描画
	void TutorialDraw();

    // カメラ移動
    void CameraUpdate();

    // マップ移動
    void ChackMoveMap();

    // 現行中のステージデータの削除
    void DeleteStatgeData();

    // 移行先のステージデータの準備
    void InitStageData(int stageLine, int stageRow);

    // ステージ内に置くエネミーの確認
    void InitEnemyData(int stageLine, int stageRow);

    // ステージ内に置くエネミーの実準備
    void SetEnemyData(int stageLine, int stageRow);
    
	// プレイヤーのHPバーの位置調整
	void PlayerStatusPop();

	void DrawParameter();

    void DrawFloor();

	void EndingUpdate();

	void EndingDraw();

    // 現在ステージ番号
    int m_nowStageNum[2];

    // 移動方向
    int m_moveRoomDir = -1;


    // 配置するオブジェクト
    std::map<std::string, std::unique_ptr<ObjectBase>> m_objects;

    std::map<std::string, std::unique_ptr<Stage>> m_stage;

    // プレイヤーの配置座標
	VECTOR m_nextPlayerPos;
	VECTOR m_nextPlayerDir;

    // ステージ移動フラグ
    bool m_isMoveMap[GameData::kStageMaxLine][GameData::kStageMaxRow];

    // ステージモデルハンドル
	std::vector<int> m_stageHandle;

    // エネミーデータ
    std::vector<EnemyKind> m_enemyData;

    // プレイヤーの強化ポイント
	int m_upgradePoint = 0;

	std::vector<int> m_objectHandle;

    // チュートリアルの画像ハンドル
	std::vector<int> m_tutorialHandle;
	// チュートリアルの表示フラグ
	bool m_isTutorial = false;
    // チュートリアルの表示アルファ
	int m_tutorialAlpha = 0;
	// チュートリアルの表示番号
	int m_tutorialNum = 0;
	// チュートリアルの表示時間
	int m_tutorialTimer = 0;
    // チュートリアル表示可能フラグ
    bool m_isTutorialPop = true;
    // ゲーム終了フラグ
	bool m_isLoseEnd = false;
	bool m_isWinEnd = false;

	int m_endingTimer = 0;
	int m_endingAlpha = 0;
};

