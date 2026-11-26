#include "stdafx.h"
#include "SANCalculation.h"
#include "Source/Actor/Stage/LightObject/LightObject.h"
#include "GameScene/Pause.h"
#include "Key/KeyGetDirection/KeyGetDirector.h"
#include "Source/Actor/Player/Player.h"
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
	constexpr float LIGHT_RECOVER_PER_SEC = 3.0f;

	/** 暗いときの1秒あたりの減少量 */
	constexpr float DARK_DAMAGE_PER_SEC = 2.0f;

	/** 暗い状態がこの秒数続いてから減り始める */
	constexpr float DARK_GRACE_TIME = 3.0f;

	/** 明るさをなめらかにする速さ(大きいほど素早く追従) */
	constexpr float BRIGHTNESS_SMOOTH_SPEED = 4.0f;

	/** 明るさをデバッグ出力する間隔(秒) */
	constexpr float DEBUG_PRINT_INTERVAL = 0.5f;

	/** 表示が追いつく基本の速さ(1秒あたりの量) */
	constexpr float DISPLAY_SPEED_MIN = 20.0f;

	/** 差が大きい時に、差に応じて速くする倍率 */
	constexpr float DISPLAY_SPEED_RATE = 3.0f;

	/** 走り続けて、この秒数が経ってから減り始める */
	constexpr float RUN_GRACE_TIME = 3.0f;

	/** 走りで減る間隔(秒) */
	constexpr float RUN_DAMAGE_INTERVAL = 1.0f;

	/** 1回で減る量 */
	constexpr float RUN_DAMAGE_AMOUNT = 1.0f;

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
	m_displaySANValue = SAN_VALUE_INITIAL;

	/** 鍵取得時の演出の参照 */
	m_keyGetDirector = FindGO<KeyGetDirector>("KeyGetDirector");

	/** プレイヤーの参照 */
	m_player = FindGO<Player>("Player");

	return true;
}

void SANCalculation::Update()
{
	if (m_pause == nullptr) {
		m_pause = FindGO<Pause>("Pause");
		return;
	}

	if (m_keyGetDirector->IsPlaying())
	{
		/** 鍵取得演出中はSAN値の計算を止める */
		return;
	}

	if (m_pause->GetIsPaused())
	{
		/** ポーズ中はSAN値の計算を止める */
		return;
	}

	/** SAN値を計算する */
	CalculateSAN();
}

void SANCalculation::UpdateDisplayValue(float deltaTime)
{
	const float diff = m_sanValue - m_displaySANValue;
	if (fabsf(diff) < 0.01f)
	{
		m_displaySANValue = m_sanValue;
		return;
	}

	/** 差が大きいほど速く動く。差が1や2でも、最低速度で必ず追いつく */
	const float speed = max(DISPLAY_SPEED_MIN, fabsf(diff) * DISPLAY_SPEED_RATE);
	const float step = speed * deltaTime;

	if (fabsf(diff) <= step) {
		m_displaySANValue = m_sanValue;
	}
	else {
		m_displaySANValue += (diff > 0.0f ? step : -step);
	}
}

void SANCalculation::CalculateSAN()
{
	const float deltaTime = g_gameTime->GetFrameDeltaTime();
	UpdateByLight(deltaTime);
	UpdateByRun(deltaTime);
	ClampSAN();
	UpdateDisplayValue(deltaTime);
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

void SANCalculation::UpdateByRun(float deltaTime)
{
	if (m_player == nullptr) { return; }

	/** 走っていなければタイマーをリセット */
	if (m_player->GetMoveState() != EnMoveState::enMoveState_Run)
	{
		m_runTimer = 0.0f;
		m_runDamageTimer = 0.0f;
		return;
	}

	/** 猶予時間が過ぎるまでは減らさない */
	m_runTimer += deltaTime;
	if (m_runTimer < RUN_GRACE_TIME) { return; }

	/** 一定間隔ごとに1ずつ減らす(明るさに関係なく) */
	m_runDamageTimer += deltaTime;
	while (m_runDamageTimer >= RUN_DAMAGE_INTERVAL)
	{
		m_runDamageTimer -= RUN_DAMAGE_INTERVAL;
		m_sanValue -= RUN_DAMAGE_AMOUNT;
	}
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