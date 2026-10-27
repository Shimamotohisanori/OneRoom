#pragma once
/** LightObject.h
 * 天井に貼り付けるライトのクラス
 *
 *  - AlwaysOn  : ずっと点灯しているライト(モデルも発光する)
 *  - Flicker   : お化けライトのように不定期に点滅するライト(モデルも発光する)
 *  - ModelOnly : 光は出さず、モデルだけ表示されているライト
 */
class LightObject : public IGameObject
{
public:
	/// ライトの挙動の種類
	enum EnLightType
	{
		enLightType_AlwaysOn,	// 常時点灯
		enLightType_Flicker,	// 不定期に点滅
		enLightType_ModelOnly,	// モデルのみ(光は出ない)
	};

	LightObject();
	~LightObject();
	bool Start() override;
	void Update() override;
	void Render(RenderContext& rc) override;

	/** ライトの種類を設定する(Start()より前に呼ぶこと) */
	void SetLightType(EnLightType type);

	/** ライトの色を設定する */
	void SetLightColor(const Vector3& color);

	/** ライトの位置を設定する */
	void SetLightPosition(const Vector3& position);

	/** ライトの方向を設定する */
	void SetLightDirection(const Vector3& direction);

	/** ライトの範囲を設定する */
	void SetLightRange(float range);

	/** 射出角度(度で指定。エンジンへはラジアンに変換して渡す) */
	void SetLightAngle(float angle);

	/** 点滅の調整用。stableMin / Max = 安定して点いている時間(秒)の範囲 */
	void SetFlickerInterval(float stableMin, float stableMax);

private:
	/** 点滅の状態 */
	enum EnFlickerState
	{
		enFlickerState_Stable,	// 安定して点灯中
		enFlickerState_Burst,	// チカチカしている最中
	};

	/** 点滅の更新処理 */
	void UpdateFlicker(float deltaTime);

	/** 保持しているパラメータと明るさ倍率(0.0～1.0)をスポットライトへ反映する */
	void ApplyLight();

	/** 発光するモデルを初期化する(AlwaysOn / Flicker 用) */
	void InitEmissiveModel();

	/** 発光シェーダーへ渡す定数バッファを更新する(明るさ倍率・色を反映) */
	void UpdateEmissive();

	/** min～max の乱数(浮動小数点) */
	static float RandomRange(float min, float max);

	/** min～max の乱数(整数) */
	static int   RandomRangeInt(int min, int max);

private:
	/** ライトの種類 */
	EnLightType		m_lightType = enLightType_AlwaysOn;

	/** ライトの色 */
	Vector3			m_color = Vector3(1.0f, 1.0f, 1.0f);

	/** ライトの位置 */
	Vector3			m_position = g_vec3Zero;

	/** ライトの方向 */
	Vector3			m_direction = Vector3(0.0f, -1.0f, 0.0f);

	/** ライトの範囲 */
	float			m_range = 600.0f;

	/** ライトの射出角度(度) */
	float			m_angle = 60.0f;

	/** ライトのモデル */
	ModelRender		m_modelRender;

	/** 発光シェーダーへ渡すデータ。emissiveLight.fx の EmissiveCb と並びを合わせること */
	struct SEmissiveData
	{
		Vector3 emissiveColor;	// 発光色(明るさ倍率を含む)
		float maskThreshold;	// 光らせる部分の輝度しきい値
		float baseBrightness;	// 光らない部分の明るさ
		float pad[3];			// 16バイト境界に合わせるための詰め物
	};

	/** 発光シェーダーへ渡すデータの実体。
	 *  エンジンは描画のたびにこのメモリを読んで定数バッファへコピーするので、メンバとして持ち続けること */
	SEmissiveData	m_emissiveData;

	/** スポットライト */
	SpotLight		m_spotLight;

	/** ライトが有効かどうか */
	bool			m_hasLight = false;

	/** Start() 済みか */
	bool			m_isStarted = false;

	/** 点滅の状態 */
	EnFlickerState	m_flickerState = enFlickerState_Stable;

	/** 点滅の状態の残り時間 */
	float			m_timer = 0.0f;

	/** 現在の明るさ倍率(0～1) */
	float			m_intensity = 1.0f;

	/** チカチカの回数 */
	int				m_burstCount = 0;

	/** チカチカするかどうか */
	bool			m_burstOn = true;

	/** チカチカの回数の最大値 */
	int				m_burstMax = 5;

	/** 安定して点いている時間(秒)の範囲 */
	float			m_stableMin = 1.5f;
	float			m_stableMax = 6.0f;
};