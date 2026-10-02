#pragma once
/**
 * GameOver.h
 * ゲームオーバー画面クラス
 * ここでゲームオーバー画面の操作や状態を管理する。
 */
class GameCamera;
class SANUI;
class Player;
class MainStage;
class GameOver : public IGameObject
{
public:
	GameOver() {}
	~GameOver();
	bool Start();
	void Update();
	void Render(RenderContext& rc);


private:
	/** ゲームオーバースプライト */
	SpriteRender m_gameOverSprite;

	/** プレイヤー */
	Player* m_player;
	
	/** SAN UI */
	SANUI* m_sanUI;

	/** メインステージ */
	MainStage* m_mainStage;

	/** ゲームカメラ */
	GameCamera* m_gameCamera;

	/** 空 */
	SkyCube* m_skyCube;
};

