// KeyItem.cpp
#include "stdafx.h"
#include "KeyItem.h"
#include "Source/Actor/Player/Player.h"
#include "Key/Key.h"
#include "Key/KeyGetDirection/KeyGetDirector.h"
namespace
{
	/** 鍵モデルのファイルパス(最初は仮のモデルでもよい) */
	const char* KEY_MODEL_FILE_PATH = "Assets/modelData/Key/Key.tkm";

	/** 鍵のスケール */
	const Vector3 KEY_SCALE = Vector3(0.3f, 0.3f, 0.3f);

	/** 取れる距離 */
	constexpr float PICKUP_DISTANCE = 70.0f;

	/** 1秒あたりの回転角度(度) */
	constexpr float ROTATE_SPEED_DEG = 90.0f;

	/** 上下に揺れる幅と速さ */
	constexpr float BOB_HEIGHT = 5.0f;
	constexpr float BOB_SPEED = 2.0f;

	/** エフェクトの位置オフセットとスケール */
	const Vector3 EFFECT_OFFSET = Vector3(0.0f, 40.0f, 0.0f);
	const Vector3 EFFECT_SCALE = Vector3(7.0f, 7.0f, 7.0f);
}

bool KeyItem::Start()
{
	m_model.Init(KEY_MODEL_FILE_PATH);
	m_model.SetScale(KEY_SCALE);
	m_model.SetPosition(m_position);
	m_model.Update();

	m_player = FindGO<Player>("Player");
	m_key = FindGO<Key>("Key");
	m_director = FindGO<KeyGetDirector>("KeyGetDirector");
	return true;
}

void KeyItem::Update()
{
	/** くるくる回って、ふわふわ浮かせて目立たせる */
	m_time += g_gameTime->GetFrameDeltaTime();

	Quaternion rot;
	rot.SetRotationDeg(Vector3::AxisY, m_time * ROTATE_SPEED_DEG);

	Vector3 pos = m_position;
	pos.y += sinf(m_time * BOB_SPEED) * BOB_HEIGHT;

	m_model.SetPosition(pos);
	m_model.SetRotation(rot);
	m_model.Update();

	/** 演出中は取れない(エフェクトも出さない) */
	if (m_director != nullptr && m_director->IsPlaying())
	{
		m_canActionEffect.Hide();
		return;
	}

	/** エフェクトの表示/非表示(距離と壁を見て自動で切り替わる) */
	m_canActionEffect.UpdateVisibility(m_position + EFFECT_OFFSET, m_player->GetPosition(), EFFECT_SCALE);

	/** 取る */
	if (IsPlayerInRange() && m_player->IsInteractTriggered())
	{
		m_canActionEffect.Hide();
		m_key->AddKey();
		/** ここで取得SEを鳴らす */
		DeleteGO(this);
	}
}

void KeyItem::Render(RenderContext& rc)
{
	m_model.Draw(rc);
}

bool KeyItem::IsPlayerInRange() const
{
	Vector3 diff = m_player->GetPosition() - m_position;
	diff.y = 0.0f;
	return diff.Length() <= PICKUP_DISTANCE;
}