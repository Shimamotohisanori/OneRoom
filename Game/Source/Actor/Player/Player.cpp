#include "stdafx.h"
#include "Player.h"
#include "SoundManager/SoundManager.h"
Player::Player()
{}

Player::~Player()
{
	DeleteGO(m_lightInteractSE);
}

bool Player::Start()
{
	/** モデルの初期化 */
	m_model.Start();

	/** コントローラーの初期化 */
	m_controller.Start();

	/** 懐中電灯の初期化 */
	m_flashlight.Start();
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

	/** ライト切り替えボタンが押されたら、ライトのON/OFFを切り替える */
	if (m_controller.IsLightToggleTriggered())
	{
		/** ライトのON/OFFを切り替える */
		m_isLightOn = !m_isLightOn;

		/** ライト切り替えのSEを再生 */
		SoundManager* sound = FindGO<SoundManager>("soundManager");
		m_lightInteractSE = sound->PlayingSE(enLightOnOff, false);

		/** 懐中電灯にON/OFFを反映 */
		m_flashlight.SetOn(m_isLightOn);
	}

	/** 懐中電灯の更新 */
	m_flashlight.Update();
}

void Player::Render(RenderContext & rc)
{
	/** モデルの描画 */
	//m_model.Render(rc);

	/** 懐中電灯の描画 */
	m_flashlight.Render(rc);
}
