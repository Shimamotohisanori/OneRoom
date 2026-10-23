#include "stdafx.h"
#include "PlayerController.h"
namespace 
{
	/** 歩きの最大速度 */
	constexpr float WALK_MAX_SPEED = 850.0f;
	
	/** 走りの最大速度 */
	constexpr float RUN_MAX_SPEED = 170.0f;

	/** 左右の向きを変える速さ(度数/フレーム換算前の係数) */
	constexpr float YAW_ROTATE_SPEED = 2.0f;

	/** コントローラーのy軸入力 */
	constexpr float Y_AXIS_INPUT = 0.0f;

	/** 入力無しとみなす閾値 */
	constexpr float NO_INPUT_THRESHOLD = 0.01f;

	/** 当たり判定の半径 */
	constexpr float COLLISION_RADIUS = 25.0f;

	/** 当たり判定の高さ */
	constexpr float COLLISION_HEIGHT = 100.0f;

	/** プレイヤー座標(目線)から、当たり判定(足元)までの高さの差 */
	constexpr float FOOT_OFFSET = 100.0f;
}

PlayerController::PlayerController()
{}

PlayerController::~PlayerController()
{}

bool PlayerController::Start()
{
	m_position = Vector3::Zero;
	/** 当たり判定は足元の座標で作る */
	Vector3 footPosition = m_position;
	footPosition.y -= FOOT_OFFSET;
	m_characterController.Init(COLLISION_RADIUS, COLLISION_HEIGHT, footPosition);
	return true;
}

void PlayerController::Update()
{
	UpdateActionInput();
	/** 入力を受け付けない場合は、入力を無視する */
	if (!m_isInputEnabled)
	{
		m_moveInput = Vector3::Zero;
		m_isRunButtonPressed = false;
		return;
	}

	UpdateInput();
	UpdateMoveState();
	UpdateMove();
}

void PlayerController::SetPosition(const Vector3& position)
{
	m_position = position;
	/** 当たり判定側にも、足元の座標で反映する */
	Vector3 footPosition = position;
	footPosition.y -= FOOT_OFFSET;
	m_characterController.SetPosition(footPosition);
}

void PlayerController::UpdateInput()
{
	/** 入力を受け付けない場合は、入力を無視する */
	if (!m_isInputEnabled)
	{
		m_moveInput = Vector3::Zero;
		m_isRunButtonPressed = false;
		return;
	}

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

void PlayerController::UpdateActionInput()
{
	/** ボタンの割り当てはここだけで管理する */
	m_isInteractTriggered = g_pad[0]->IsTrigger(enButtonA);
	m_isLightToggleTriggered = g_pad[0]->IsTrigger(enButtonY);
	m_isCancelTriggered = g_pad[0]->IsTrigger(enButtonB);
	m_isPauseTriggered = g_pad[0]->IsTrigger(enButtonSelect);
}

void PlayerController::UpdateMove()
{
	/** 1フレームの経過時間を取得 */
	float deltaTime = g_gameTime->GetFrameDeltaTime();

	/** 入力がないときは移動量ゼロ */
	Vector3 moveSpeed = Vector3::Zero;

	if (m_moveInput.Length() > NO_INPUT_THRESHOLD)
	{
		/** 入力方向 */
		Vector3 moveDir = m_moveInput;
		moveDir.Normalize();

		/** 入力方向に今のyawを適用して、前進する方向を決定 */
		Quaternion yawRotation;
		yawRotation.SetRotationDeg(Vector3::AxisY, m_yaw);
		yawRotation.Apply(moveDir);

		/** プレイヤーの向きを更新 */
		m_rotation = yawRotation;

		/** 入力方向 * 状態ごとの速度(加速なしで、すぐ最大速度) */
		moveSpeed = moveDir * DecideTargetSpeed();
	}

	/** 壁などにぶつかりながら移動し、結果(足元の座標)を受け取る */
	Vector3 footPosition = m_characterController.Execute(moveSpeed, deltaTime);

	/** プレイヤー座標は、足元からオフセット分上にする */
	m_position = footPosition;
	m_position.y += FOOT_OFFSET;
}
