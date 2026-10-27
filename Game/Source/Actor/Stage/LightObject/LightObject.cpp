#include "stdafx.h"
#include "LightObject.h"
#include <random>

namespace
{

	/** ファイルパス */
	const char* LIGHTOBJECT_MODEL_PATH = "Assets/modelData/LightObject/LightObject.tkm";

	/** 射出角度の最小値(度)。0度だとシェーダー側で0除算になりうるため下限を設ける */
	constexpr float MIN_SPOT_ANGLE_DEG = 1.0f;

	/** 明るさ倍率:全点灯 */
	constexpr float INTENSITY_FULL = 1.0f;

	/** 距離・角度による減衰の累乗(1.0で線形) */
	constexpr float AFFECT_POW_LINEAR = 1.0f;

	/** チカチカの回数の範囲 */
	constexpr int BURST_COUNT_MIN = 3;
	constexpr int BURST_COUNT_MAX = 8;

	/** チカチカ開始直後(消灯)の明るさ倍率の範囲 */
	constexpr float BURST_START_INTENSITY_MIN = 0.0f;
	constexpr float BURST_START_INTENSITY_MAX = 0.15f;

	/** チカチカ開始直後(消灯)の継続時間(秒)の範囲 */
	constexpr float BURST_START_TIME_MIN = 0.03f;
	constexpr float BURST_START_TIME_MAX = 0.12f;

	/** チカチカ中の点灯時の明るさ倍率の範囲 */
	constexpr float BURST_ON_INTENSITY_MIN = 0.1f;
	constexpr float BURST_ON_INTENSITY_MAX = 1.0f;

	/** チカチカ中の消灯時の明るさ倍率の範囲(完全には消さず薄く残す) */
	constexpr float BURST_OFF_INTENSITY_MIN = 0.0f;
	constexpr float BURST_OFF_INTENSITY_MAX = 0.2f;

	/** チカチカ中の1回あたりの継続時間(秒)の範囲 */
	constexpr float BURST_TIME_MIN = 0.03f;
	constexpr float BURST_TIME_MAX = 0.55f;

	/** 発光シェーダーのファイルパス */
	const char* EMISSIVE_FX_FILE_PATH = "Assets/shader/emissiveLight.fx";

	/** 発光の強さ(HDR)。LoadingScene のブルームのしきい値(3.0)を超えるとにじむ。
	 *  蛍光灯はカメラに近いと画面いっぱいに広がるので、にじませたくないならしきい値より小さくする */
	constexpr float EMISSIVE_POWER = 0.5f;

	/** モデルの発光の点滅カーブ。大きいほど、暗いときにしっかり消えてチカチカが目立つ(1.0で明るさ倍率そのまま) */
	constexpr float EMISSIVE_INTENSITY_POW = 2.0f;

	/** 光らせる部分の輝度しきい値。0以下ならモデル全体が光る(管だけなら 0.6 前後) */
	constexpr float EMISSIVE_MASK_THRESHOLD = 0.0f;

	/** 光らない部分の明るさ(アルベドに掛ける) */
	constexpr float EMISSIVE_BASE_BRIGHTNESS = 0.05f;

	/** 乱数エンジンを返す */
	std::mt19937& GetRandomEngine()
	{
		static std::mt19937 engine{ std::random_device{}() };
		return engine;
	}
}

LightObject::LightObject()
{}

LightObject::~LightObject()
{
	/** 削除前にライトを消して、スロットに色が残らないようにする */
	if (m_hasLight) {
		m_spotLight.SetColor(g_vec3Zero);
		m_spotLight.SetColor2(g_vec3Zero);
		m_spotLight.SetColor3(g_vec3Zero);
		m_spotLight.Update();
	}
}

bool LightObject::Start()
{
	/** モデルを初期化。光を出すライトは発光シェーダー、ModelOnly は通常描画 */
	if (m_lightType == enLightType_ModelOnly) {
		m_modelRender.Init(LIGHTOBJECT_MODEL_PATH);
	}
	else {
		InitEmissiveModel();
	}
	m_modelRender.SetPosition(m_position);
	m_modelRender.Update();

	/** ModelOnly 以外はスポットライトを確保する */
	if (m_lightType != enLightType_ModelOnly) {

		/** スポットライトを初期化 */
		m_spotLight.Init();

		/** ライトを有効化 */
		m_hasLight = true;

		/** 明るさ倍率を初期化 */
		m_intensity = INTENSITY_FULL;

		/** スポットライトのパラメータを初期化
		 *  SSpotLight の angle / range などは初期値が無いので、
		 *  使わない2番目・3番目の光も含めて全て明示的に設定する */
		const float angleRad = Math::DegToRad(max(m_angle, MIN_SPOT_ANGLE_DEG));
		m_spotLight.SetPosition(m_position);
		m_spotLight.SetDirection(m_direction);
		m_spotLight.SetColor(g_vec3Zero);
		m_spotLight.SetColor2(g_vec3Zero);
		m_spotLight.SetColor3(g_vec3Zero);
		m_spotLight.SetRange(m_range);
		m_spotLight.SetAngle(angleRad);
		m_spotLight.SetAngle2(angleRad);
		m_spotLight.SetAngle3(angleRad);
		m_spotLight.SetRangeAffectPowParam(AFFECT_POW_LINEAR);
		m_spotLight.SetRangeAffectPowParam2(AFFECT_POW_LINEAR);
		m_spotLight.SetRangeAffectPowParam3(AFFECT_POW_LINEAR);
		m_spotLight.SetAngleAffectPowParam(AFFECT_POW_LINEAR);
		m_spotLight.SetAngleAffectPowParam2(AFFECT_POW_LINEAR);
		m_spotLight.SetAngleAffectPowParam3(AFFECT_POW_LINEAR);
		m_spotLight.Update();

		/** ライトのパラメータをスポットライトへ反映 */
		ApplyLight();
	}

	/** 点滅ライトは最初の安定時間を設定 */
	if (m_lightType == enLightType_Flicker) {

		/** 点滅状態を安定状態にする */
		m_flickerState = enFlickerState_Stable;

		/** 安定時間をランダムに決める */
		m_timer = RandomRange(m_stableMin, m_stableMax);
	}

	/** Start() 済みフラグを立てる */
	m_isStarted = true;
	return true;
}

void LightObject::Update()
{
	/** 光を出さないライトは何もしない */
	if (m_lightType == enLightType_ModelOnly) {
		return;
	}

	/** 点滅ライトは点滅の更新を行う */
	if (m_lightType == enLightType_Flicker) {
		UpdateFlicker(g_gameTime->GetFrameDeltaTime());
	}

	/** スポットライトへ反映 */
	ApplyLight();

	/** モデルの発光へ反映(ライトと同じ明るさ倍率なので、点滅が同期する) */
	UpdateEmissive();
}

void LightObject::InitEmissiveModel()
{
	/** 発光シェーダーへ渡すデータに初期値を入れる */
	UpdateEmissive();

	/** 発光シェーダーでモデルを初期化(フォワードレンダリング) */
	ModelInitData initData;
	initData.m_tkmFilePath = LIGHTOBJECT_MODEL_PATH;
	initData.m_fxFilePath = EMISSIVE_FX_FILE_PATH;
	initData.m_vsEntryPointFunc = "VSMain";
	initData.m_psEntryPointFunc = "PSMain";
	initData.m_modelUpAxis = enModelUpAxisZ;
	/** データの実体(メンバ変数)を渡す。ConstantBuffer のオブジェクトを渡してはいけない */
	initData.m_expandConstantBuffer = &m_emissiveData;
	initData.m_expandConstantBufferSize = sizeof(m_emissiveData);
	m_modelRender.InitForwardRendering(initData);
}

void LightObject::UpdateEmissive()
{
	/** 発光色 = ライトの色 × 明るさ倍率(カーブ補正) × 発光の強さ
	 *  点滅の消灯時(明るさ倍率が小さい)は、カーブでほぼ真っ暗になるので、モデルも点滅して見える */
	const float intensity = powf(m_intensity, EMISSIVE_INTENSITY_POW);
	m_emissiveData.emissiveColor = m_color;
	m_emissiveData.emissiveColor *= intensity * EMISSIVE_POWER;
	m_emissiveData.maskThreshold = EMISSIVE_MASK_THRESHOLD;
	m_emissiveData.baseBrightness = EMISSIVE_BASE_BRIGHTNESS;
}

void LightObject::UpdateFlicker(float deltaTime)
{
	/** 点滅の状態の残り時間を減らす */
	m_timer -= deltaTime;

	/** 残り時間がまだある場合は何もしない */
	if (m_timer > 0.0f) {
		return;
	}

	/** 残り時間がなくなった → 状態を切り替える */
	switch (m_flickerState) {
	case enFlickerState_Stable:
		/** 安定時間が終わった → チカチカ開始 */
		m_flickerState = enFlickerState_Burst;

		/** チカチカの回数をランダムに決める */
		m_burstCount = RandomRangeInt(BURST_COUNT_MIN, BURST_COUNT_MAX);

		/** チカチカの最初は消灯から始める */
		m_burstOn = false;

		/** チカチカの最初の明るさ倍率をランダムに決める */
		m_intensity = RandomRange(BURST_START_INTENSITY_MIN, BURST_START_INTENSITY_MAX);

		/** チカチカの最初の時間をランダムに決める */
		m_timer = RandomRange(BURST_START_TIME_MIN, BURST_START_TIME_MAX);
		break;

		/** チカチカ中 */
	case enFlickerState_Burst:
		/** チカチカの回数を減らす */
		m_burstCount--;
		if (m_burstCount <= 0) {
			/** チカチカが終わった → 安定状態に戻す */
			m_flickerState = enFlickerState_Stable;

			/** 安定状態の明るさ倍率は常に1.0f */
			m_intensity = INTENSITY_FULL;

			/** 安定時間をランダムに決める */
			m_timer = RandomRange(m_stableMin, m_stableMax);
		}

		/** チカチカがまだ続く場合 */
		else {
			/** チカチカ中の状態を反転させる */
			m_burstOn = !m_burstOn;

			/** チカチカ中の明るさ倍率をランダムに決める */
			m_intensity = m_burstOn
				? RandomRange(BURST_ON_INTENSITY_MIN, BURST_ON_INTENSITY_MAX)
				: RandomRange(BURST_OFF_INTENSITY_MIN, BURST_OFF_INTENSITY_MAX);

			/** チカチカ中の時間をランダムに決める */
			m_timer = RandomRange(BURST_TIME_MIN, BURST_TIME_MAX);
		}
		break;
	}
}

void LightObject::ApplyLight()
{
	/** ライトが有効でない場合は何もしない */
	if (!m_hasLight) {
		return;
	}
	/** 明るさ倍率を色にかける */
	Vector3 color = m_color;
	color *= m_intensity;

	/** スポットライトのパラメータを反映 */
	m_spotLight.SetPosition(m_position);
	m_spotLight.SetDirection(m_direction);
	m_spotLight.SetColor(color);
	m_spotLight.SetRange(m_range);
	const float angleRad = Math::DegToRad(max(m_angle, MIN_SPOT_ANGLE_DEG));
	m_spotLight.SetAngle(angleRad);
	m_spotLight.SetAngle2(angleRad);
	m_spotLight.SetAngle3(angleRad);
	m_spotLight.Update();
}

void LightObject::Render(RenderContext& rc)
{
	/** モデルを描画 */
	m_modelRender.Draw(rc);
}

void LightObject::SetLightType(EnLightType type)
{
	/** ライトの種類を設定 */
	m_lightType = type;
}

void LightObject::SetLightColor(const Vector3& color)
{
	/** ライトの色を設定 */
	m_color = color;
}

void LightObject::SetLightPosition(const Vector3& position)
{
	/** ライトの位置を設定 */
	m_position = position;

	/** モデルの位置を更新 */
	if (m_isStarted) {
		m_modelRender.SetPosition(m_position);
		m_modelRender.Update();
	}
}

void LightObject::SetLightDirection(const Vector3& direction)
{
	/** ライトの方向を設定 */
	m_direction = direction;
	m_direction.Normalize();
}

void LightObject::SetLightRange(float range)
{
	/** ライトの範囲を設定 */
	m_range = range;
}

void LightObject::SetLightAngle(float angle)
{
	/** 射出角度(度)を設定 */
	m_angle = angle;
}

void LightObject::SetFlickerInterval(float stableMin, float stableMax)
{
	/** 安定して点いている時間(秒)の範囲を設定 */
	m_stableMin = stableMin;
	m_stableMax = stableMax;
}

float LightObject::RandomRange(float min, float max)
{
	/** min～max の乱数(浮動小数点) */
	std::uniform_real_distribution<float> dist(min, max);
	return dist(GetRandomEngine());
}

int LightObject::RandomRangeInt(int min, int max)
{
	/** min～max の乱数(整数) */
	std::uniform_int_distribution<int> dist(min, max);
	return dist(GetRandomEngine());
}