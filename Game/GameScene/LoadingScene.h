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
};

