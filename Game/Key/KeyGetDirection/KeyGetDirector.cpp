#include "stdafx.h"
#include "KeyGetDirector.h"
#include "Source/Actor/Player/Player.h"
#include "GameCamera/GameCamera.h"
#include "Key/KeyItem/KeyItem.h"
namespace
{
	
}

bool KeyGetDirector::Start()
{
	m_player = FindGO<Player>("Player");
	m_camera = FindGO<GameCamera>("GameCamera");
	return true;
}

Vector3 KeyGetDirector::Blend(const Vector3& a, const Vector3& b, float t)
{
	/** 最初と最後がゆっくりになる補間 */
	t = t * t * (3.0f - 2.0f * t);
	return a + (b - a) * t;
}

void KeyGetDirector::Play(const KeyGetDirection& direction)
{
	if (IsPlaying()) { return; }

	m_direction = direction;
	m_startPos = g_camera3D->GetPosition();
	m_startTarget = g_camera3D->GetTarget();

	m_player->SetInputEnabled(false);
	m_phase = EnPhase::MoveIn;
	m_timer = 0.0f;
}

void KeyGetDirector::Update()
{
	/** 演出中でなければ何もしない */
	if (!IsPlaying()) { return; }

	/** タイマーを進める */
	m_timer += g_gameTime->GetFrameDeltaTime();

	switch (m_phase)
	{
	/** カメラを演出用の位置に移動させる */
	case EnPhase::MoveIn:
	{
		const float t = min(m_timer / m_direction.moveTime, 1.0f);
		m_camera->SetDirectionView(
			Blend(m_startPos, m_direction.cameraPosition, t),
			Blend(m_startTarget, m_direction.cameraTarget, t));
		if (t >= 1.0f) {
			m_phase = EnPhase::Hold;
			m_timer = 0.0f; 
		
			/** カメラが着いたら、鍵を出す */
			KeyItem* keyItem = NewGO<KeyItem>(0, "KeyItem");

			/** 鍵の位置はそれぞれ違う */
			keyItem->SetPosition(m_direction.KeyPosition);
		}
		break;
	}
	case EnPhase::Hold:
		/** ここで鍵を出す・SEを鳴らすなどの演出を足せる */
		if (m_timer >= m_direction.holdTime) 
		{ 
			m_phase = EnPhase::MoveOut;
			m_timer = 0.0f; 
		}
		break;

	case EnPhase::MoveOut:
	{
		/** カメラを元の位置に戻す */
		const float t = min(m_timer / m_direction.moveTime, 1.0f);
		m_camera->SetDirectionView(
			Blend(m_direction.cameraPosition, m_startPos, t),
			Blend(m_direction.cameraTarget, m_startTarget, t));
		if (t >= 1.0f)
		{
			m_camera->EndDirection();
			m_player->SetInputEnabled(true);
			m_phase = EnPhase::Idle;
		}
		break;
	}
	}
}