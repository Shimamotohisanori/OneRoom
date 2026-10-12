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
	void Death();

	/** ゲームクリア処理 */
	void Clear();

	/** SANUI */
	SANUI* m_sanUI;

	/** KeyUI */
	KeyUI* m_keyUI;

	/** Key */
	Key* m_key;

	/** SAN計算クラス */
	SANCalculation* m_sanCalculation;

	/** 操作紹介UI */
	IntroductionUI* m_introductionUI;

	/** プレイヤー */
	Player* m_player;
};

