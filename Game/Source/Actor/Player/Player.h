#pragma once
#include "PlayerModel.h"
#include "PlayerController.h"
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

	/** プレイヤーの座標を取得 */
	Vector3 GetPosition() const
	{
		/** プレイヤーコントローラーから座標を取得 */
		return m_controller.GetPosition();
	}

	/** 現在プレイヤーが向いている方向を取得 */
	float GetYaw() const
	{
		return m_controller.GetYaw();
	}

	/** プレイヤーの位置を設定 */
	void SetPosition(const Vector3& position)
	{
		m_controller.SetPosition(position);
	}

	/** プレイヤーの向きを設定 */
	void SetRotation(const Quaternion& rotation)
	{
		m_controller.SetRotation(rotation);
	}

	/** 段ボールを持っているかのフラグを取得 */
	bool GetIsHoldingDanBall() const { return m_isHoldingDanBall; }

	/** 段ボールを持っているかのフラグを設定 */
	void SetIsHoldingDanBall(bool isHolding) { m_isHoldingDanBall = isHolding; }

	/** アクション入力(ボタン)が押されたか */
	bool IsInteractTriggered() const { return m_controller.IsInteractTriggered(); }

	/** ライト切り替え入力(ボタン)が押されたか */
	bool IsLightToggleTriggered() const { return m_controller.IsLightToggleTriggered(); }

	/** やめる・戻る入力(ボタン)が押されたか */
	bool IsCancelTriggered() const { return m_controller.IsCancelTriggered(); }

	/** ポーズ入力(ボタン)が押されたか */
	bool IsPauseTriggered() const { return m_controller.IsPauseTriggered(); }

	/** ライトが点いているか */
	bool IsLightOn() const { return m_isLightOn; }

	/** 入力の有効化の設定 */
	void SetInputEnabled(bool enabled) { m_controller.SetInputEnabled(enabled); }

	/** 入力の有効化の状態を取得 */
	bool GetIsInputEnabled() const { return m_controller.GetIsInputEnabled(); }

private:
	/** プレイヤーモデル */
	PlayerModel m_model;

	/** プレイヤーコントローラー */
	PlayerController m_controller;

	/** 段ボールを持っているか */
	bool m_isHoldingDanBall = false;

	/** ライトが点いているか */
	bool m_isLightOn = false;
};

