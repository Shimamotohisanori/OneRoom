#include "stdafx.h"
#include "Player.h"

Player::Player()
{}

Player::~Player()
{}

bool Player::Start()
{
	/** モデルの初期化 */
	model.Start();
	return true;
}

void Player::Update()
{
	/** モデルの更新 */
	model.Update();
}

void Player::Render(RenderContext & rc)
{
	/** モデルの描画 */
	model.Render(rc);
}
