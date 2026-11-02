#pragma once
/**
 * SANCalculation.h
 * SAN計算クラス
 * ここでSAN計算の操作や状態を管理する。
 *  - 明るい場所にいると回復、真っ暗な場所に一定時間いると減少
 *  - 音が鳴ると一定量減少(OnNoiseHeard を呼ぶ)
 */
class SANCalculation : public IGameObject
{
public:
	SANCalculation() {}
	~SANCalculation();
	bool Start();
	void Update();

	/** SAN値を増やす */
	void AddSANValue(int value)
	{
		m_sanValue += static_cast<float>(value);
		ClampSAN();
	}

	/** SAN値を減らす */
	void SubtractSANValue(int value)
	{
		m_sanValue -= static_cast<float>(value);
		ClampSAN();
	}

	/** SAN値を設定する */
	void SetSANValue(int value)
	{
		m_sanValue = static_cast<float>(value);
		ClampSAN();
	}

	/** SAN値を取得する(表示用に四捨五入した整数) */
	int GetSANValue() const
	{
		return static_cast<int>(m_sanValue + 0.5f);
	}

	/** 音が鳴ったときに呼ぶ(約15減る) */
	void OnNoiseHeard();

	/** SAN値に応じた文字色(100=緑 / 50=白 / 0=赤) */
	Vector4 GetSANColor() const;

	/** 現在プレイヤーがいる場所の明るさ(なめらかにした値。デバッグ表示用) */
	float GetBrightness() const
	{
		return m_smoothedBrightness;
	}

private:
	/** SANを計算する */
	void CalculateSAN();

	/** 明るさによるSAN値の増減 */
	void UpdateByLight(float deltaTime);

	/** SAN値を0～100に収める */
	void ClampSAN();

private:
	/** SAN値(1フレームあたりの増減が小さいのでfloatで持つ) */
	float m_sanValue = 100.0f;

	/** なめらかにした明るさ(点滅ライトで回復/減少が細かく切り替わらないようにする) */
	float m_smoothedBrightness = 0.0f;

	/** m_smoothedBrightness を初期化済みか */
	bool m_isBrightnessInitialized = false;

	/** 暗い状態が続いている時間(秒) */
	float m_darkTimer = 0.0f;

	/** デバッグ出力用タイマー */
	float m_debugTimer = 0.0f;
};