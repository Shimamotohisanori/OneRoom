#pragma once
/**
 * LoadingScene.h
 * ローディング画面クラス
 * ここでローディング画面の操作や状態を管理する。
 */
class Game;
class Player;
class MainStage;
class GameCamera;
class LoadingScene : public IGameObject
{
public:
	LoadingScene() {}
	~LoadingScene();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	/** ゲームをロードする */
	void LoadGame();

private:
	/** スピナーの更新 */
	void UpdateSpinner();

private:
	/** ローディングスプライト */
	SpriteRender m_loadingSprite;

	/** ローディングの進捗状況 */
	int m_loadingProgress = 0;

	/** プレイヤー */
	Player* m_player;

	/** メインステージ */
	MainStage* m_mainStage;

	/** ゲームカメラ */
	GameCamera* m_gameCamera;

	/** スカイキューブ */
	SkyCube* m_skyCube;

	/** ゲーム */
	Game* m_game = nullptr;

	/** スピナーの点の数 */
	static const int SPINNER_DOT_NUM = 12;

	/** スピナーの点 */
	SpriteRender m_spinnerDot[SPINNER_DOT_NUM];

	/** 先頭の点を進めるためのタイマー */
	float m_spinnerTimer = 0.0f;

	/** 先頭(一番明るい)の点の番号 */
	int m_spinnerHead = 0;

	/** ライトのインデックス */
	int m_lightIndex = 0;

	/** 電話のインデックス */
	int m_phoneIndex = 0;

	/** ドアのインデックス */
	int m_doorIndex = 0;
};

