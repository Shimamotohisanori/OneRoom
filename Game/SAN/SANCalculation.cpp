#include "stdafx.h"
#include "SANCalculation.h"
#include "Source/Actor/Stage/LightObject/LightObject.h"
#include <cstdio>

namespace
{
	/** SAN値の初期値 */
	constexpr float SAN_VALUE_INITIAL = 100.0f;

	/** SAN値の最大値 */
	constexpr float SAN_VALUE_MAX = 100.0f;

	/** SAN値の最小値 */
	constexpr float SAN_VALUE_MIN = 0.0f;

	/** 音1回で減る量 */
	constexpr float NOISE_DAMAGE = 15.0f;

	/** この明るさ以上で回復 */
	constexpr float LIGHT_BRIGHT_THRESHOLD = 0.2f;

	/** この明るさ以下で「暗い」と判定(間は変化なし) */
	constexpr float LIGHT_DARK_THRESHOLD = 0.05f;

	/** 1秒あたりの回復量 */
	constexpr float LIGHT_RECOVER_PER_SEC = 2.5f;

	/** 暗いときの1秒あたりの減少量 */
	constexpr float DARK_DAMAGE_PER_SEC = 2.0f;

	/** 暗い状態がこの秒数続いてから減り始める */
	constexpr float DARK_GRACE_TIME = 3.0f;

	/** 明るさをなめらかにする速さ(大きいほど素早く追従) */
	constexpr float BRIGHTNESS_SMOOTH_SPEED = 4.0f;

	/** 明るさをデバッグ出力する間隔(秒) */
	constexpr float DEBUG_PRINT_INTERVAL = 0.5f;

	/** SAN値の文字色 */
	const Vector4 COLOR_GREEN = Vector4(0.2f, 1.0f, 0.2f, 1.0f);
	const Vector4 COLOR_WHITE = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	const Vector4 COLOR_RED = Vector4(1.0f, 0.1f, 0.1f, 1.0f);

	/** 色の線形補間 */
	Vector4 LerpColor(const Vector4& a, const Vector4& b, float t)
	{
		return Vector4(
			a.x + (b.x - a.x) * t,
			a.y + (b.y - a.y) * t,
			a.z + (b.z - a.z) * t,
			a.w + (b.w - a.w) * t);
	}
}

SANCalculation::~SANCalculation()
{
}

bool SANCalculation::Start()
{
	m_sanValue = SAN_VALUE_INITIAL;
	return true;
}

void SANCalculation::Update()
{
	/** SAN値を計算する */
	CalculateSAN();
}

void SANCalculation::CalculateSAN()
{
	UpdateByLight(g_gameTime->GetFrameDeltaTime());
	ClampSAN();
}

void SANCalculation::UpdateByLight(float deltaTime)
{
	/** プレイヤーの目の位置での明るさを調べる */
	const float rawBrightness = LightObject::CalculateBrightnessAt(g_camera3D->GetPosition());

	/** 明るさをなめらかにする(最初の1回は現在値で初期化) */
	if (!m_isBrightnessInitialized) {
		m_smoothedBrightness = rawBrightness;
		m_isBrightnessInitialized = true;
	}
	else {
		const float t = min(BRIGHTNESS_SMOOTH_SPEED * deltaTime, 1.0f);
		m_smoothedBrightness += (rawBrightness - m_smoothedBrightness) * t;
	}

#ifdef _DEBUG
	/** Visual Studio の「出力」ウィンドウに明るさを表示(しきい値調整用) */
	m_debugTimer += deltaTime;
	if (m_debugTimer >= DEBUG_PRINT_INTERVAL) {
		m_debugTimer = 0.0f;
		char buf[128];
		sprintf_s(buf, "[SAN] brightness raw=%.3f smooth=%.3f san=%.1f dark=%.1fs\n",
			rawBrightness, m_smoothedBrightness, m_sanValue, m_darkTimer);
		OutputDebugStringA(buf);
	}
#endif

	if (m_smoothedBrightness >= LIGHT_BRIGHT_THRESHOLD) {
		/** 明るい場所: 回復 */
		m_darkTimer = 0.0f;
		m_sanValue += LIGHT_RECOVER_PER_SEC * deltaTime;
	}
	else if (m_smoothedBrightness <= LIGHT_DARK_THRESHOLD) {
		/** 暗い場所: 猶予時間を過ぎてから減少 */
		m_darkTimer += deltaTime;
		if (m_darkTimer >= DARK_GRACE_TIME) {
			m_sanValue -= DARK_DAMAGE_PER_SEC * deltaTime;
		}
	}
	else {
		/** 中間の明るさ: 変化なし。暗い時間のカウントは戻す */
		m_darkTimer = 0.0f;
	}
}

void SANCalculation::OnNoiseHeard()
{
	/** 音が鳴った回数分だけ減る */
	m_sanValue -= NOISE_DAMAGE;
	ClampSAN();
}

void SANCalculation::ClampSAN()
{
	m_sanValue = max(SAN_VALUE_MIN, min(SAN_VALUE_MAX, m_sanValue));
}

Vector4 SANCalculation::GetSANColor() const
{
	if (m_sanValue >= 50.0f) {
		/** 100→50: 緑→白 */
		return LerpColor(COLOR_GREEN, COLOR_WHITE, (SAN_VALUE_MAX - m_sanValue) / 50.0f);
	}
	/** 50→0: 白→赤 */
	return LerpColor(COLOR_WHITE, COLOR_RED, (50.0f - m_sanValue) / 50.0f);
}