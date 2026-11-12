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
class GoalLight;
class KeyPad;
class Paper;
class Phone;
class PhonePaper;
class Game : public IGameObject
{
public:
	Game();
	~Game();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	/** ゲームが開始できる状態かを取得する */
	bool IsReady() const { return m_isReady; }

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

	/** ゴールの光 */
	GoalLight* m_goalLight = nullptr;

	/** キーパッド */
	KeyPad* m_keyPad = nullptr;

	/** 紙 */
	Paper* m_paper = nullptr;

	/** 電話 */
	Phone* m_phone = nullptr;

	/** 電話の紙 */
	PhonePaper* m_phonePaper = nullptr;

	/** ゲームが開始できる状態か */
	bool m_isReady = false;

	/** 死んだかどうか */
	bool m_isDying = false;
};

