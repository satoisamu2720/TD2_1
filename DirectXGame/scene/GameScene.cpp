#include "GameScene.h"
#include "MathUtilityForText.h"
#include "TextureManager.h"
#include "time.h"
#include <cassert>
// コンストラクタ
GameScene::GameScene() {}

// デストラクタ
GameScene::~GameScene() {
	delete spriteBG_;
	delete modelStage_;
	delete modelPlayer_;
	delete modelBeam_;
	delete modelEnemy_;
	delete spriteTitle_;
	delete spriteScore_;
}

// 初期化
void GameScene::Initialize() {

	srand((unsigned int)time(NULL));

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	textureHandleBG_ = TextureManager::Load("bg.jpg");
	spriteBG_ = Sprite::Create(textureHandleBG_, {0, 0});

	viewProjection_.Initialize();

	textureHandleStage_ = TextureManager::Load("stage2.jpg");
	modelStage_ = Model::Create();
	for (int i = 0; i < 20; i++) {
		worldTransformStage_[i].Initialize();
	}

	// ビュープロジェクションの初期化
	viewProjection_.translation_.y = 1;
	viewProjection_.translation_.z = -6;
	viewProjection_.Initialize();

	for (int i = 0; i < 20; i++) {
		// ステージの位置を変更
		worldTransformStage_[i].translation_ = {0, -1.5f, 2.0f * i - 5};
		worldTransformStage_[i].scale_ = {4.5f, 1, 1};
		// 変更行列を更新
		worldTransformStage_[i].matWorld_ = MakeAffineMatrix(
		    worldTransformStage_[i].scale_, worldTransformStage_[i].rotation_,
		    worldTransformStage_[i].translation_);
		// 変更行列を定数バッファに転送
		worldTransformStage_[i].TransferMatrix();
	}

	// プレイヤー
	textureHandlePlayer_ = TextureManager::Load("player.png");
	modelPlayer_ = Model::Create();
	worldTransformPlayer_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformPlayer_.Initialize();

	// ビーム
	textureHandleBeam_ = TextureManager::Load("beam.png");
	modelBeam_ = Model::Create();

	for (int b = 0; b < 10; b++) {
		worldTransformBeam_[b].scale_ = {0.3f, 0.3f, 0.3f};
		worldTransformBeam_[b].Initialize();
	}

	// 敵
	textureHandleEnemy_ = TextureManager::Load("enemy.png");
	modelEnemy_ = Model::Create();

	for (int e = 0; e < 10; e++) {
		worldTransformEnemy_[e].scale_ = {0.5f, 0.5f, 0.5f};
		worldTransformEnemy_[e].Initialize();
	}

	// タイトル
	textureHandleTitle_ = TextureManager::Load("title.png");
	spriteTitle_ = Sprite::Create(textureHandleTitle_, {0, 0});

	// タイトル
	textureHandleTitleEnter_ = TextureManager::Load("enter.png");
	spriteEnter_ = Sprite::Create(textureHandleTitleEnter_, {400, 450});

	// ゲームオーバー
	textureHandleGameover_ = TextureManager::Load("gameover.png");
	spriteGameover_ = Sprite::Create(textureHandleGameover_, {0, 0});

	// デバックテキスト
	debugText_ = DebugText::GetInstance();
	debugText_->Initialize();

	// サウンドデータの読み込み
	soundDataHandleTitleBGM_ = audio_->LoadWave("Audio/Ring05.wav");
	soundDataHandleGamePlayBGM_ = audio_->LoadWave("Audio/Ring08.wav");
	soundDataHandleGameOverBGM_ = audio_->LoadWave("Audio/Ring09.wav");
	soundDataHandleEnemyHitSE_ = audio_->LoadWave("Audio/chord.wav");
	soundDataHandlePlayerHitSE_ = audio_->LoadWave("Audio/tada.wav");

	// タイトルBGMを再生
	voiceHandleBGM_ = audio_->PlayWave(soundDataHandleTitleBGM_, true);
	// スコア数値(スプライト)
	textureHandleNumber_ = TextureManager::Load("number.png");
	for (int i = 0; i < 5; i++) {
		spriteNumber_[i] = Sprite::Create(textureHandleNumber_, {300.0f + i * 26, 0});
	}

	// スコア文字
	textureHandleScore_ = TextureManager::Load("score.png");
	spriteScore_ = Sprite::Create(
	    textureHandleScore_, {
	                             160,
	                             0,
	                         });

	textureHandleLife_ = TextureManager::Load("player.png");
}

// 更新
void GameScene::GamePlayUpdete() {
	PlayerUpdate();
	BeamUpdate();
	EnemyUpdete();
	Collision();
	StageUpdate();
}

void GameScene::StageUpdate() {
	// 各ステージでループ
	for (int i = 0; i < 20; i++) {
		// 手前に移動
		worldTransformStage_[i].translation_.z -= 0.1f;
		// 端まで来たら調べる
		if (worldTransformStage_[i].translation_.z < -5) {
			worldTransformStage_[i].translation_.z += 40;
		}
		// 変換行列を更新
		worldTransformStage_[i].matWorld_ = MakeAffineMatrix(
		    worldTransformStage_[i].scale_, worldTransformStage_[i].rotation_,
		    worldTransformStage_[i].translation_);
		// 変換行列を定数バッファに転送
		worldTransformStage_[i].TransferMatrix();
	}
}
// ゲーム
void GameScene::Update() {

	switch (sceneMode_) {
	case 0:
		GamePlayUpdete();
		break;
	case 1:
		TitleUpdate();
		break;
	case 2:
		GameoverUpdate();
		break;
	}

	gameTimer_ += 1;
}

// プレイヤー更新処理
void GameScene::PlayerUpdate() {

	// 移動

	// 右へ移動
	if (input_->PushKey(DIK_RIGHT)) {
		worldTransformPlayer_.translation_.x += 0.1f;
	}
	// 左へ移動
	if (input_->PushKey(DIK_LEFT)) {
		worldTransformPlayer_.translation_.x -= 0.1f;
	}

	if (worldTransformPlayer_.translation_.x > 4) {
		worldTransformPlayer_.translation_.x = 4;
	}
	if (worldTransformPlayer_.translation_.x < -4) {
		worldTransformPlayer_.translation_.x = -4;
	}

	// 変換行列を更新
	worldTransformPlayer_.matWorld_ = MakeAffineMatrix(
	    worldTransformPlayer_.scale_, worldTransformPlayer_.rotation_,
	    worldTransformPlayer_.translation_);
	// 変換行列を定数バッファに転送
	worldTransformPlayer_.TransferMatrix();

	if (playerTimer_ > 0) {
		playerTimer_ -= 1;
	}
}

// ビーム更新処理
void GameScene::BeamUpdate() {
	BeamMove();
	BeamBorn();
	// 変換行列を更新
	for (int i = 0; i < 10; i++) {
		if (beamFlag_[i] == 1)

			worldTransformBeam_[i].matWorld_ = MakeAffineMatrix(
			    worldTransformBeam_[i].scale_, worldTransformBeam_[i].rotation_,
			    worldTransformBeam_[i].translation_);
		// 変換行列を定数バッファに転送
		worldTransformBeam_[i].TransferMatrix();
	}
}

// ビーム移動
void GameScene::BeamMove() {

	for (int i = 0; i < 10; i++) {
		if (beamFlag_[i] == 1) {
			worldTransformBeam_[i].translation_.z += 0.5f;
			// 回転　
			worldTransformBeam_[i].rotation_.x += 0.1f;
		}
	}
}

// ビーム
void GameScene::BeamBorn() {
	// スペースキーで発射
	for (int b = 0; b < 10; b++) {
		if (beamTimer_ == 0) {

			if (input_->PushKey(DIK_SPACE) && beamFlag_[b] == 0) {
				worldTransformBeam_[b].translation_.x = worldTransformPlayer_.translation_.x;
				worldTransformBeam_[b].translation_.y = worldTransformPlayer_.translation_.y;
				worldTransformBeam_[b].translation_.z = worldTransformPlayer_.translation_.z;

				beamTimer_ = 1;
				beamFlag_[b] = 1;
				break;
			}

		} else {

			if (beamTimer_ >= 1) {
				beamTimer_++;

				if (beamTimer_ > 10) {
					beamTimer_ = 0;
				}
				break;
			}
		}

		// // 40以上で削除
		if (worldTransformBeam_[b].translation_.z > 40) {
			beamFlag_[b] = 0;
			beamTimer_ = 0;
		}
	}
}

// 敵の更新処理
void GameScene::EnemyUpdete() {
	EnemyMove();
	EnemyBorn();
	EnemyJump();
	for (int e = 0; e < 10; e++) {
		if (EnemyFlag_[e] != 0)

			worldTransformEnemy_[e].matWorld_ = MakeAffineMatrix(
			    worldTransformEnemy_[e].scale_, worldTransformEnemy_[e].rotation_,
			    worldTransformEnemy_[e].translation_);

		// 変換行列を定数バッファに転送
		worldTransformEnemy_[e].TransferMatrix();
	}
}

// 　敵の移動
void GameScene::EnemyMove() {

	for (int e = 0; e < 10; e++) {
		if (EnemyFlag_[e] == 1) {
			worldTransformEnemy_[e].translation_.x += enemySpeed_[e];
			worldTransformEnemy_[e].translation_.z -= 0.1f;
			worldTransformEnemy_[e].translation_.z -= gameTimer_ / 1000.0f;
			worldTransformEnemy_[e].rotation_.x -= 0.1f;

			if (worldTransformEnemy_[e].translation_.x < -4) {
				enemySpeed_[e] = 0.1f;
			}
			if (worldTransformEnemy_[e].translation_.x > 4) {
				enemySpeed_[e] = -0.1f;
			}
		}
	}
}

// 敵
void GameScene::EnemyBorn() {

	for (int e = 0; e < 10; e++) {

		if (EnemyFlag_[e] == 0) {
			if (rand() % 10 == 0) {

				int x = rand() % 80;
				float x2 = (float)x / 10 - 4;
				worldTransformEnemy_[e].translation_.x = x2;
				worldTransformEnemy_[e].translation_.z = 40;
				worldTransformEnemy_[e].translation_.y = 0;

				if (rand() % 2 == 0) {
					enemySpeed_[e] = 0.1f;

				} else {
					enemySpeed_[e] = -0.1f;
				}
				EnemyFlag_[e] = 1;
				break;
			}
		}
		//-5にいったら削除
		if (worldTransformEnemy_[e].translation_.z < -5) {
			EnemyFlag_[e] = 0;
			break;
		}
	}
}

// 衝突判定
void GameScene::Collision() {
	// 衝突判定（プレイヤーと敵）
	CollisionPlayerEnemy();
	// 衝突判定（ビームと敵）
	CollisionBeamEnemy();
}

// プレイヤーと敵の当たり判定
void GameScene::CollisionPlayerEnemy() {
	// 敵が存在すれば
	for (int i = 0; i < 10; i++) {
		if (EnemyFlag_[i] == 1) {
			// 差を求める
			float dx =
			    abs(worldTransformPlayer_.translation_.x - worldTransformEnemy_[i].translation_.x);
			float dz =
			    abs(worldTransformPlayer_.translation_.z - worldTransformEnemy_[i].translation_.z);
			// 衝突したら
			if (dx < 1 && dz < 1) {
				// 存在しない
				EnemyFlag_[i] = 0;
				playerLife_ -= 1;
				playerTimer_ = 60;
				audio_->PlayWave(soundDataHandlePlayerHitSE_);
				// BGM切り替え
				audio_->StopWave(voiceHandleBGM_);
				voiceHandleBGM_ = audio_->PlayWave(soundDataHandleGameOverBGM_, true);
			}
			if (playerLife_ <= 0) {
				sceneMode_ = 2;
				beamFlag_[i] = 0;
				EnemyFlag_[i] = 0;
			}
		}
	}
}

// 敵とビームの当たり判定
void GameScene::CollisionBeamEnemy() {
	// 敵が存在すれば
	for (int i = 0; i < 10; i++) {
		if (EnemyFlag_[i] != 0) {
			for (int b = 0; b < 10; b++) {
				if (beamFlag_[b] == 1) {
					// 差を求める
					float dx =
					    abs(worldTransformBeam_[b].translation_.x -
					        worldTransformEnemy_[i].translation_.x);
					float dz =
					    abs(worldTransformBeam_[b].translation_.z -
					        worldTransformEnemy_[i].translation_.z);
					// 衝突したら
					if (dx < 1 && dz < 1) {
						// 存在しない
						enemyJumpSpeed_[i] = 1;
						EnemyFlag_[i] = 2;
						beamFlag_[b] = 0;
						gameScore_ += 20;
						audio_->PlayWave(soundDataHandleEnemyHitSE_);
					}
				}
			}
		}
	}
}

void GameScene::EnemyJump() {
	for (int i = 0; i < 10; i++) {
		if (EnemyFlag_[i] == 2) {
			//
			worldTransformEnemy_[i].translation_.y += enemyJumpSpeed_[i];
			//
			enemyJumpSpeed_[i] -= 0.1f;
			//
			worldTransformEnemy_[i].translation_.x += enemySpeed_[i] * 4;
			//

			if (worldTransformEnemy_[i].translation_.y < -3) {
				EnemyFlag_[i] = 0;
			}
		}
	}
}

// 3D 描画
void GameScene::GamePlayDraw3D() {

	/// ステージ描画
	for (int i = 0; i < 20; i++) {
		modelStage_->Draw(worldTransformStage_[i], viewProjection_, textureHandleStage_);
	}
	// プレイヤー描画
	if (playerTimer_ % 4 < 2) {
		modelPlayer_->Draw(worldTransformPlayer_, viewProjection_, textureHandlePlayer_);
	}
	// ビーム描画
	for (int b = 0; b < 10; b++) {
		if (beamFlag_[b] == 1) {
			modelBeam_->Draw(worldTransformBeam_[b], viewProjection_, textureHandleBeam_);
		}
	}
	// 敵描画　
	for (int e = 0; e < 10; e++) {
		if (EnemyFlag_[e] != 0) {
			modelEnemy_->Draw(worldTransformEnemy_[e], viewProjection_, textureHandleEnemy_);
		}
	}
}

// スコア数値の表示
void GameScene::DrawScore() {
	// 各桁の値を取り出す
	int eachNumber[5] = {};
	int number = gameScore_;

	int keta = 10000;
	for (int i = 0; i < 5; i++) {
		eachNumber[i] = number / keta;
		number = number % keta;
		keta = keta / 10;
	}

	// 各桁の数値を描画
	for (int i = 0; i < 5; i++) {
		spriteNumber_[i]->SetSize({32, 64});
		spriteNumber_[i]->SetTextureRect({32.0f * eachNumber[i], 0}, {32, 64});
		spriteNumber_[i]->Draw();
	}
}

void GameScene::DrawLife() {
	////ライフ
	for (int i = 0; i < playerLife_; i++) {
		spriteLife_[i] = Sprite::Create(textureHandlePlayer_, {800.0f + i * 60, 0});

		spriteLife_[i]->SetSize({40, 40});
		spriteLife_[i]->Draw();
	}
}

// 近景
void GameScene::GamePlayDraw2DNear() {
	// ゲームスコア

	spriteScore_->Draw();
	DrawScore();

	// ライフ
	DrawLife();
}

// 背景　
void GameScene::GamePlayDraw2DBack() { spriteBG_->Draw(); }

// タイトル
void GameScene::TitleUpdate() {
	for (int i = 0; i < 10; i++) {
		if (input_->TriggerKey(DIK_RETURN)) {
			// リセット
			worldTransformPlayer_.translation_.x = 0;
			gameScore_ = 0;
			beamFlag_[i] = 0;
			playerLife_ = 3;
			sceneMode_ = 0;
			EnemyFlag_[i] = 0;
			beamFlag_[i] = 0;
			gameTimer_ = 0;
			playerTimer_ = 0;
			GamePlayUpdete();
			// BGM切り替え
			audio_->StopWave(voiceHandleBGM_);
			voiceHandleBGM_ = audio_->PlayWave(soundDataHandleGamePlayBGM_, true);
		}
	}
}

void GameScene::GameoverUpdate() {
	for (int i = 0; i < 10; i++) {
		if (input_->TriggerKey(DIK_RETURN)) {
			sceneMode_ = 1;

			// BGM切り替え
			audio_->StopWave(voiceHandleBGM_);
			voiceHandleBGM_ = audio_->PlayWave(soundDataHandleTitleBGM_, true);
		}
	}
}

// タイトル
void GameScene::TitleDraw2DNear() {
	spriteTitle_->Draw();

	if (gameTimer_ % 40 >= 20) {
		spriteEnter_->Draw();
	}
}

void GameScene::GameOverDraw2DNear() {
	spriteGameover_->Draw();

	if (gameTimer_ % 40 >= 20) {
		spriteEnter_->Draw();
	}
}

// 描画

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// ここに背景スプライトの描画処理を追加できる

	// 各シーンの背景2D表示を呼び出す
	switch (sceneMode_) {
	case 0:
		GamePlayDraw2DBack();
		break;
	case 2:
		GamePlayDraw2DBack();
		break;
	}

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// ここに3Dオブジェクトの描画処理を追加できる

	// 3D描画
	switch (sceneMode_) {
	case 0:
		GamePlayDraw3D();
		break;
	case 2:
		GamePlayDraw3D();
		break;
	}
	// 3Dオブジェクト描画後処理
	Model::PostDraw();

#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	// 近景　2D
	/// ここに前景スプライトの描画処理を追加できる
	debugText_->Print("", 200, 10, 2);
	debugText_->DrawAll();

	// 各シーンの2D表示を呼び出す
	switch (sceneMode_) {
	case 0:
		GamePlayDraw2DNear();
		break;
	case 1:
		TitleDraw2DNear();
		break;
	case 2:
		GameOverDraw2DNear();
		break;
	}

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
