#pragma once
class SANCalculation;

/**
 * SANRedEffect.h
 * SAN値が低いほど画面を赤くするクラス
 */
class SANRedEffect : public IGameObject
{
public:
	SANRedEffect() {}
	~SANRedEffect();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

private:
	/** 赤いエフェクトのスプライト */
	SpriteRender m_redEffectSprite;

	/** SAN計算クラス */
	SANCalculation* m_san = nullptr;
	
	/** 心臓の音 */
	SoundSource* m_heartSound = nullptr;

	/** 脈動用の位相 */
	float m_pulsePhase = 0.0f;
};