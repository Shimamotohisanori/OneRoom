#include "stdafx.h"
#include "SANRedEffect.h"
#include "SANCalculation.h"
#include <cmath>

namespace
{
	constexpr const char* RED_FILE_PATH = "Assets/sprite/Scene/RedGradient.dds";
	constexpr float RED_WIDTH = 1920.0f;
	constexpr float RED_HEIGHT = 1080.0f;

	/** このSAN値以下から赤くなり始める */
	constexpr float EFFECT_START_SAN = 60.0f;

	/** SAN値0のときの最大の濃さ(0.0~1.0)。「程よく」はここで調整 */
	constexpr float MAX_ALPHA = 1.0f;

	/** 濃さの立ち上がり方(1より大きいほど、低SANまで薄いまま) */
	constexpr float RATE_CURVE = 0.8f;

	/** 脈動の速さ(rad/秒)。SAN値が高いときと低いときで変える */
	constexpr float PULSE_SPEED_MIN = 3.0f;   // rate=0 のとき
	constexpr float PULSE_SPEED_MAX = 10.0f;  // rate=1 のとき

	/** 脈動の速さをSAN値に応じて変える */
	constexpr float PULSE_AMOUNT = 0.3f;
}

bool SANRedEffect::Start()
{
	m_san = FindGO<SANCalculation>("SANCalculation");
	m_redEffectSprite.Init(RED_FILE_PATH, RED_WIDTH, RED_HEIGHT);
	return true;
}

void SANRedEffect::Update()
{
	/** SAN計算クラスが見つからなければ、何もしない */
	if (m_san == nullptr) { return; }

	/** フレームの経過時間を取得 */
	const float deltaTime = g_gameTime->GetFrameDeltaTime();

	/** SAN値に応じて赤くする */
	float rate = (EFFECT_START_SAN - static_cast<float>(m_san->GetSANValue())) / EFFECT_START_SAN;
	rate = max(0.0f, min(1.0f, rate));
	rate = powf(rate, RATE_CURVE);

	/** 低SANのときほど、早く脈打たせる(速さを位相に積算する) */
	const float speed = PULSE_SPEED_MIN + (PULSE_SPEED_MAX - PULSE_SPEED_MIN) * rate;
	m_pulsePhase += speed * deltaTime;

	/** 脈動の大きさをSAN値に応じて変える */
	float pulse = 1.0f + sinf(m_pulsePhase) * PULSE_AMOUNT * rate;

	/** 赤いエフェクトの濃さを設定 */
	float alpha = min(MAX_ALPHA * rate * pulse, 1.0f);
	m_redEffectSprite.SetMulColor(Vector4(1.0f, 1.0f, 1.0f, alpha));
	m_redEffectSprite.Update();
}

void SANRedEffect::Render(RenderContext& rc)
{
	m_redEffectSprite.Draw(rc);
}