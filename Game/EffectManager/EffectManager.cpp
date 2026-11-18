#include "stdafx.h"
#include "EffectManager.h"

bool EffectManager::Start()
{
    LoadEffects();
    return true;
}

void EffectManager::LoadEffects()
{
	/** アクション可能な事を知らせるエフェクト */
	nsK2EngineLow::EffectEngine::GetInstance()->ResistEffect(
		(int)EffectID::EffectID_CanAction,
		u"Assets/Effects/CanActionEffect.efk"
	);
}
