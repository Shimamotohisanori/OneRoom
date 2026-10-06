#pragma once
/**
 * PlayerController.h
 * プレイヤーコントローラークラス
 * ここでプレイヤーの操作を管理する。
 */
enum class EnMoveState
{
	enMoveState_Idle, //待機
	enMoveState_Walk, //歩き
	enMoveState_Run,  //走り
};

class PlayerController : public IGameObject
{
public:
	PlayerController();
	~PlayerController();
	bool Start();
	void Update();

	/** プレイヤーの座標を取得(GameCameraなどから呼ばれる) */
	Vector3 GetPosition() const
	{
		return m_position;
	}

	/** 位置を設定 */
	void SetPosition(const Vector3& position) { m_position = position; }

	/** 現在の移動状態を取得(アニメーション再生などから呼ばれる想定) */
	EnMoveState GetMoveState() const
	{
		return m_moveState;
	}

	/** 現在の左右の向き(度数)を取得。GameCameraから呼ばれる */
	float GetYaw() const { return m_yaw; }

	/** 現在の向きを取得 */
	Quaternion GetRotation() const { return m_rotation; }

	/** 現在の向きを設定 */
	void SetRotation(const Quaternion& rotation) { m_rotation = rotation; }

	/** このフレームにインタラクト(Aボタン)が押されたか */
	bool IsInteractTriggered() const { return m_isInteractTriggered; }

	/** このフレームにライト切り替え(Yボタン)が押されたか */
	bool IsLightToggleTriggered() const { return m_isLightToggleTriggered; }

	/** このフレームに戻る・やめる(Bボタン)が押されたか */
	bool IsCancelTriggered() const { return m_isCancelTriggered; }

	/** このフレームにポーズ(セレクト)が押されたか */
	bool IsPauseTriggered() const { return m_isPauseTriggered; }

	/** 入力を受け付けるかどうかの関数 */
	void SetInputEnabled(bool enabled) { m_isInputEnabled = enabled; }

	/** 入力の有効化の状態を取得 */
	bool GetIsInputEnabled() const { return m_isInputEnabled; }

private:
	/** 入力を読み取るだけの処理 */
	void UpdateInput();

	/** 入力結果から、状態(Idle/Walk/Run)を決める */
	void UpdateMoveState();
	
	/** 状態に応じて速度を近づけながら、座標を進める */
	void UpdateMove();
	
	/** 状態ごとの目標最大速度を返す(Switch文で管理) */
	float DecideTargetSpeed() const;

	/** アクション入力(ボタン)を読み取る */
	void UpdateActionInput();
	

private:
	/** プレイヤーの座標 */
	Vector3 m_position = Vector3::Zero;

	/** 入力の生データ */
	Vector3 m_moveInput = Vector3::Zero;

	/** プレイヤーの向き */
	Quaternion m_rotation = Quaternion::Identity;

	/** 走るボタンが押されているか */
	bool m_isRunButtonPressed = false;

	/** インタラクトボタンのフラグ */
	bool m_isInteractTriggered = false;

	/** ライト切り替えボタンのフラグ */
	bool m_isLightToggleTriggered = false;

	/** やめる・戻るボタンのフラグ */
	bool m_isCancelTriggered = false;

	/** ポーズボタンのフラグ */
	bool m_isPauseTriggered = false;

	/** 入力を受け付けるかどうか */
	bool m_isInputEnabled = true;

	/** 状態と速度 */
	EnMoveState m_moveState = EnMoveState::enMoveState_Idle;
	
	/** 現在の速度 */
	float m_currentSpeed = 0.0f;
	
	/** 一秒あたりの加速量 */
	float m_acceleration = 30.0f;	

	/** 累積される左右の向き(度数)。
	 * 移動方向にも、カメラの左右向きにも使う */
	float m_yaw = 0.0f;
};

