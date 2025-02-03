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
    
    // 現在ステージ番号
    int m_nowStageNum[2];

    // 移動方向
    int m_moveRoomDir = -1;


    // 配置するオブジェクト
    std::map<std::string, std::unique_ptr<ObjectBase>> m_objects;

    std::map<std::string, std::shared_ptr<Stage>> m_stage;

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
};

