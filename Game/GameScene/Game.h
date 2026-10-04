#pragma once
#include "SAN/SANCalculation.h"
/**
 * Game.h
 * ゲームクラス
 * ここは他のクラスと違い、神クラスであるので、ゲーム全体の操作や状態を管理する。
 * ここでゲームの操作や状態を管理する。
 */
class SANUI;
class KeyUI;
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

	/** SANUI */
	SANUI* m_sanUI;

	/** KeyUI */
	KeyUI* m_keyUI;

	/** SAN計算クラス */
	SANCalculation m_sanCalculation;
};

