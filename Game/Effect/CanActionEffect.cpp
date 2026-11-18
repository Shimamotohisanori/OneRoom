#include "stdafx.h"
#include "CanActionEffect.h"
#include "EffectManager/EffectManager.h"
namespace
{
	/** 全てのアクション可能エフェクトで共通の表示距離 */
	constexpr float SHOW_DISTANCE = 300.0f;
}

void CanActionEffect::Show(const Vector3& pos, const Vector3& scale)
{
	/** すでにエフェクトが存在する場合は位置を更新する */
	if (m_emitter != nullptr) {
		m_emitter->SetPosition(pos);
		return;
	}
	m_emitter = NewGO<nsK2EngineLow::EffectEmitter>(0);
	m_emitter->Init((int)EffectID::EffectID_CanAction);
	m_emitter->SetPosition(pos);
	m_emitter->SetScale(scale);
	m_emitter->Play();
}

void CanActionEffect::Hide()
{
	if (m_emitter == nullptr) { return; }
	/** この手順で止めて削除する */
	if (!m_emitter->IsDead()) {
		m_emitter->Stop();
		DeleteGO(m_emitter);
	}
	m_emitter = nullptr;
}

void CanActionEffect::SetPosition(const Vector3& pos)
{
	if (m_emitter != nullptr) { m_emitter->SetPosition(pos); }
}

void CanActionEffect::UpdateVisibility(const Vector3& effectPos, const Vector3& playerPos, const Vector3& scale)
{
	/** 高さは無視して距離を測る */
	Vector3 diff = playerPos - effectPos;
	diff.y = 0.0f;
	const bool inRange = diff.Length() <= SHOW_DISTANCE;

	/** 範囲内で、カメラとの間に壁がないときだけ表示 */
	if (inRange && !IsBlockedByWall(g_camera3D->GetPosition(), effectPos))
	{
		Show(effectPos, scale);
	}
	else
	{
		Hide();
	}
}

bool CanActionEffect::IsBlockedByWall(const Vector3& from, const Vector3& to)
{
	/** エフェクト位置のすぐ手前までを判定する(対象オブジェクト自身の当たり判定を拾わないため) */
	constexpr float END_MARGIN = 10.0f;

	Vector3 dir = to - from;
	const float length = dir.Length();
	if (length <= END_MARGIN)
	{
		/** ほぼ重なっているなら遮られていない */
		return false;
	}
	dir /= length;
	const Vector3 rayEnd = to - dir * END_MARGIN;

	/** 交点は使わないが、引数として必要 */
	Vector3 hitPos;

	/** trueなら、カメラとエフェクトの間に何かがある */
	return nsK2EngineLow::PhysicsWorld::GetInstance()->RayTest(from, rayEnd, hitPos);
}
