#pragma once
/**
 * Game.h
 * ゲームクラス
 * ここは他のクラスと違い、神クラスであるので、ゲーム全体の操作や状態を管理する。
 * ここでゲームの操作や状態を管理する。
 */
class SANUI;
class KeyUI;
class Key;
class IntroductionUI;
class SANCalculation;
class Player;
class Pause;
class GoalDoor;
class Game : public IGameObject
{
public:
	Game();
	~Game();
	bool Start();
	void Update();
	void Render(RenderContext& rc);


private:
	/** ゲームオーバー処理 */
	bool Death();

	/** ゲームクリア処理 */
	bool Clear();

	/** 一時停止処理 */
	void GamePause();

	/** SANUI */
	SANUI* m_sanUI = nullptr;

	/** KeyUI */
	KeyUI* m_keyUI = nullptr;

	/** Key */
	Key* m_key = nullptr;

	/** SAN計算クラス */
	SANCalculation* m_sanCalculation = nullptr;

	/** 操作紹介UI */
	IntroductionUI* m_introductionUI = nullptr;

	/** プレイヤー */
	Player* m_player = nullptr;

	/** 一時停止クラス */
	Pause* m_pause = nullptr;
	
	/** ゲーム中BGM */
	SoundSource* m_gameBGM = nullptr;

	/** ゴールのドア */
	GoalDoor* m_goalDoor = nullptr;
};

