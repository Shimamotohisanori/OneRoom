#pragma once

enum class EffectID
{
	EffectID_CanAction,/** アクションを起こせるエフェクト */
	EffectID_Num/** エフェクトの数 */
};

/** 
 * EffectManager 
 * エフェクトの読み込みを一括で行ったり、管理するクラス
 */
class EffectManager : public IGameObject
{
public:
	EffectManager() {}
	~EffectManager() {}

	/** エフェクトの初期化 */
	bool Start() override;

	/** エフェクトの読み込み関数 */
	void LoadEffects();
};

