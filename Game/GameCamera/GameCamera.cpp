#include "stdafx.h"
#include "GameCamera.h"
#include "Source/Actor/Player/Player.h"

/** カメラの注視点から視点までのベクトル */
#include <cmath>

namespace
{
	/** 度数をラジアンに変換する係数 */
	constexpr float DEG_TO_RAD = 3.14159265f / 180.0f;

	/** カメラの高さ調整用の係数 */
	constexpr float CAMERA_HEIGHT_ADJUSTMENT = 80.0f;

	/** カメラの制限に使うピッチ角 */
	constexpr float INITIAL_PITCH_LIMIT = 80.0f;

	/** 入力値を計算する際に使う係数 */
	constexpr float INPUT_COEFFICIENT = 2.0f;

	/** カメラのニアクリップ */
	constexpr float CAMERA_NEAR_CLIP = 1.0f;

	/** カメラのファークリップ */
	constexpr float CAMERA_FAR_CLIP = 100000.0f;

	/** 注視点から視点までのベクトル */
	const Vector3 TO_CAMERA_POSITION = {0.0f, 0.0f, -15.0f };
}

GameCamera::~GameCamera()
{}

bool GameCamera::Start()
{
	/** プレイヤーの取得 */
	m_player = FindGO<Player>("Player");

	/** 注視点から視点までのベクトルを設定 */
	m_toCameraPosition.Set(TO_CAMERA_POSITION);

	/** カメラのニアクリップとファークリップを設定 */
	nsK2EngineLow::g_camera3D->SetNear(CAMERA_NEAR_CLIP);
	nsK2EngineLow::g_camera3D->SetFar(CAMERA_FAR_CLIP);
	return true;
}

void GameCamera::Update()
{
	/** プレイヤーが存在しない場合または入力が無効な場合は処理を中断 */
	if (m_player == nullptr || m_player->GetIsInputEnabled() == false)
	{
		return;
	}

	/** ゲームパッドの右スティックのY入力を取得 */
	float y = g_pad[0]->GetRStickYF();

	/** 入力値が小さい場合は無視する */
	m_pitch += INPUT_COEFFICIENT * y;

	/** ピッチ角を直接クランプ(0.9のような閾値判定より意図が明確) */
	const float PITCH_LIMIT = INITIAL_PITCH_LIMIT; /** 度数(要調整) */
	if (m_pitch > PITCH_LIMIT) m_pitch = PITCH_LIMIT;
	if (m_pitch < -PITCH_LIMIT) m_pitch = -PITCH_LIMIT;

	/** プレイヤーの向き(ヨー角)を取得 */
	float yaw = m_player->GetYaw();

	/** yaw・pitchをラジアンに変換 */
	float yawRad = yaw * DEG_TO_RAD;
	float pitchRad = m_pitch * DEG_TO_RAD;

	/** 三角関数を使い、カメラの向きを計算 */
	Vector3 forward;
	forward.x = sinf(yawRad) * cosf(pitchRad);
	forward.y = sinf(pitchRad);
	forward.z = cosf(yawRad) * cosf(pitchRad);
	forward.Normalize();

	/** カメラの位置を計算 */
	Vector3 eyePosition = m_player->GetPosition();
	
	/** 目の高さに合わせる */
	eyePosition.y += CAMERA_HEIGHT_ADJUSTMENT;

	/** 注視点を計算 */
	Vector3 target = eyePosition + forward;

	Vector3 up = Vector3::AxisY;

	nsK2EngineLow::g_camera3D->SetTarget(target);
	nsK2EngineLow::g_camera3D->SetPosition(eyePosition);
	nsK2EngineLow::g_camera3D->SetUp(up);
	nsK2EngineLow::g_camera3D->Update();
}
