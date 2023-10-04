#pragma once

#include "Audio.h"
#include "DebugText.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

	DebugText* debugText_ = nullptr;

	// BG(スプライト)
	uint32_t textureHandleBG_ = 0;
	Sprite* spriteBG_ = nullptr;
	// スコア数値(スプライト)
	uint32_t textureHandleNumber_ = 0;
	Sprite* spriteNumber_[5] = {};

	int gameScore_ = 0;

	int gameTimer_ = 0;
	// プレイヤー
	uint32_t textureHandlePlayer_ = 0;
	Model* modelPlayer_ = nullptr;
	WorldTransform worldTransformPlayer_;

	int playerLife_ = 3;
	int playerTimer_ = 0;
	// プレイヤー更新
	void PlayerUpdate();

	// ビーム
	uint32_t textureHandleBeam_ = 0;
	Model* modelBeam_ = {};
	WorldTransform worldTransformBeam_[10];

	void BeamMove();
	void BeamUpdate();
	void BeamBorn();

	int beamFlag_[10] = {};
	int beamTimer_ = 0;

	// 敵
	uint32_t textureHandleEnemy_ = 0;
	Model* modelEnemy_ = 0;
	WorldTransform worldTransformEnemy_[10];

	void EnemyMove();
	void EnemyUpdete();
	void EnemyBorn();
	void EnemyJump();

	int EnemyFlag_[10] = {};
	float enemySpeed_[10] = {}; // 敵のスピード
	float enemyJumpSpeed_[10] = {};

	// プレイヤー衝突判定
	void Collision();
	void CollisionPlayerEnemy();
	void CollisionBeamEnemy();

	// シーン切り替え
	int sceneMode_ = 1;

	// タイトル
	void TitleUpdate();
	void TitleDraw2DNear();

	// タイトル (スプライト)
	uint32_t textureHandleTitle_ = 0;
	Sprite* spriteTitle_ = nullptr;

	// タイトル (文字)
	uint32_t textureHandleTitleEnter_ = 0;
	Sprite* spriteEnter_ = nullptr;

	// スコア(文字)
	uint32_t textureHandleScore_ = 0;
	Sprite* spriteScore_ = nullptr;
	// スコア(文字)
	uint32_t textureHandleLife_ = 0;
	Sprite* spriteLife_[3] = {};

	// ゲームオーバー
	void GameoverUpdate();

	// ゲームオーバー
	uint32_t textureHandleGameover_ = 0;
	Sprite* spriteGameover_ = nullptr;

	// サウンド
	uint32_t soundDataHandleTitleBGM_ = 0;
	uint32_t soundDataHandleGamePlayBGM_ = 0;
	uint32_t soundDataHandleGameOverBGM_ = 0;
	uint32_t soundDataHandleEnemyHitSE_ = 0;
	uint32_t soundDataHandlePlayerHitSE_ = 0;
	uint32_t voiceHandleBGM_ = 0;

	// ステージ
	void StageUpdate();

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	// ビュープロジェクション(共通)
	ViewProjection viewProjection_;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();
	void GamePlayUpdete();
	void GamePlayDraw3D();
	void GamePlayDraw2DBack();
	void GamePlayDraw2DNear();
	void GameOverDraw2DNear();
	void DrawScore();
	void DrawLife();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// //ステージ
	uint32_t textureHandleStage_ = 0;
	Model* modelStage_ = nullptr;
	WorldTransform worldTransformStage_[20];

	///
	/// </summary>
};
