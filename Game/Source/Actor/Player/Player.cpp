#include "stdafx.h"
#include "Player.h"

Player::Player()
{}

Player::~Player()
{}

bool Player::Start()
{
	/** モデルの初期化 */
	m_model.Start();

	/** コントローラーの初期化 */
	m_controller.Start();
	return true;
}

void Player::Update()
{
	/** コントローラーの更新 */
	m_controller.Update();

	/** モデルの更新 */
	m_model.Update();

	/** コントローラーの座標をモデルに反映 */
	m_model.SetPosition(m_controller.GetPosition());

	/** コントローラーの向きをモデルに反映 */
	m_model.SetRotation(m_controller.GetRotation());
}

void Player::Render(RenderContext & rc)
{
	/** モデルの描画 */
	//m_model.Render(rc);
}
