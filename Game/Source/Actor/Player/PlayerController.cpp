#include "stdafx.h"
#include "PlayerController.h"
namespace 
{
	/** 歩きの最大速度 */
	constexpr float WALK_MAX_SPEED = 20.0f;
	
	/** 走りの最大速度 */
	constexpr float RUN_MAX_SPEED = 50.0f;

	/** 左右の向きを変える速さ(度数/フレーム換算前の係数) */
	constexpr float YAW_ROTATE_SPEED = 2.0f;

	/** コントローラーのy軸入力 */
	constexpr float Y_AXIS_INPUT = 0.0f;

	/** 入力無しとみなす閾値 */
	constexpr float NO_INPUT_THRESHOLD = 0.01f;
}

PlayerController::PlayerController()
{}

PlayerController::~PlayerController()
{}

bool PlayerController::Start()
{
	return true;
}

void PlayerController::Update()
{
	UpdateInput();
	UpdateMoveState();
	UpdateMove();
}

void PlayerController::UpdateInput()
{
	/** ゲームパッドの左スティックの入力を取得 */
	m_moveInput.Set(g_pad[0]->GetLStickXF(), Y_AXIS_INPUT, g_pad[0]->GetLStickYF());

	/** 走るボタン */
	m_isRunButtonPressed = g_pad[0]->IsPress(enButtonRB1);

	/** 見ている方向と前進する方向の元になる角度が
	  * PlayerControllerで管理する */
	/** 右スティックのX入力で、左右の向きを更新 */
	float rStickX = g_pad[0]->GetRStickXF();

	/** ヨーを更新 */
	m_yaw += rStickX * YAW_ROTATE_SPEED;
}

void PlayerController::UpdateMoveState()
{
	/** 入力があるかどうかを判定 */
	/** 数値がほとんどなしは「入力なし」とみなす */
	bool isMoving = m_moveInput.Length() > NO_INPUT_THRESHOLD;

	if (!isMoving) {
		m_moveState = EnMoveState::enMoveState_Idle;
	}
	else if (m_isRunButtonPressed) {
		m_moveState = EnMoveState::enMoveState_Run;
	}
	else {
		m_moveState = EnMoveState::enMoveState_Walk;
	}
}

float PlayerController::DecideTargetSpeed() const
{
	/** 状態に応じて目標速度を決定 */
	float targetSpeed = 0.0f;

	/** Switch文で状態ごとの速度を決定 */
	switch (m_moveState)
	{
	case EnMoveState::enMoveState_Idle:
		targetSpeed = 0.0f;
		break;
	case EnMoveState::enMoveState_Walk:
		targetSpeed = WALK_MAX_SPEED;
		break;
	case EnMoveState::enMoveState_Run:
		targetSpeed = RUN_MAX_SPEED;
		break;
	}

	return targetSpeed;
}

void PlayerController::UpdateMove()
{
	/** 状態に応じて目標速度を決定 */
	float targetSpeed = DecideTargetSpeed();
	
	/** 1フレームの経過時間を取得 */
	float deltaTime = g_gameTime->GetFrameDeltaTime();

	/** 現在速度を、目標最大速度に
	 * 少しずつ近づける(加速・減速) */
	if (m_currentSpeed < targetSpeed)
	{
		/** 現在の速度 + 加速度 * 経過時間 */
		m_currentSpeed += m_acceleration * deltaTime;

		/** 目標速度を超えないようにする */
		if (m_currentSpeed > targetSpeed) m_currentSpeed = targetSpeed;
	}

	/** 目標速度よりも速い場合は減速する */
	else if (m_currentSpeed > targetSpeed)
	{
		/** 現在の速度 - 加速度 * 経過時間 */
		m_currentSpeed -= m_acceleration * deltaTime;

		/** 目標速度を下回らないようにする */
		if (m_currentSpeed < targetSpeed) m_currentSpeed = targetSpeed;
	}

	/** 実際に座標を進める */
	if (m_moveInput.Length() > NO_INPUT_THRESHOLD)
	{
		/** 入力方向 */
		Vector3 moveDir = m_moveInput;
		moveDir.Normalize();
		
		/** 入力方向に今のyawを適用して
		 *  前進する方向を決定 */
		Quaternion yawRotation;

		/** Yawの回転を作る(Y軸) */
		yawRotation.SetRotationDeg(Vector3::AxisY, m_yaw);

		/** 入力方向にyawを適用 */
		yawRotation.Apply(moveDir);

		/** プレイヤーの向きを更新 */
		m_rotation = yawRotation;

		/** 入力方向 * 現在の速度 * 経過時間 */
		m_position += moveDir * m_currentSpeed * deltaTime;

	}
}
