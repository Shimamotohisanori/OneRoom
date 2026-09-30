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


private:
	/** プレイヤーモデル */
	PlayerModel m_model;

	/** プレイヤーコントローラー */
	PlayerController m_controller;
};

