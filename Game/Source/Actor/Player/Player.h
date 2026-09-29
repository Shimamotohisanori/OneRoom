#pragma once
#include "PlayerModel.h"
/**
 * Player.h
 * プレイヤークラス
 * ここでプレイヤーの操作や状態を管理する。
 */
class Player : public IGameObject
{
public:
	Player();
	~Player();
	bool Start();
	void Update();
	void Render(RenderContext& rc);


private:
	/** プレイヤーモデル */
	PlayerModel model;
};

