#pragma once
/**
 * Plate.h
 * プレートクラス
 * ここでプレートを管理する。
 */
class Plate : public IGameObject
{
public:
	Plate();
	~Plate();
	bool Start();
	void Update();
	void Render(RenderContext& renderContext);

	/** プレートを踏んだかどうかを取得 */
	bool IsSteppedOn() const
	{
		return m_isSteppedOn;
	}

	/** プレートの座標を取得 */
	Vector3 GetPosition() const
	{
		return m_position;
	}

	/** 指定座標がプレートの設置範囲内か(高さは無視) */
	bool IsInPlacementRange(const Vector3& position) const;

private:
	/** プレートを踏んだときの処理 */
	void OnSteppedOn();

	/** 発光の更新 */
	void UpdateGlow(float deltaTime);


private:
	/** 発光シェーダーへ渡すデータ。emissiveLight.fx の EmissiveCb と並びを合わせること */
	struct SEmissiveData
	{
		Vector3 emissiveColor;	// 発光色(明るさ倍率を含む)
		float maskThreshold;	// 光らせる部分の輝度しきい値
		float baseBrightness;	// 光らない部分の明るさ
		float pad[3];			// 16バイト境界に合わせるための詰め物
	};

	/** プレートのモデル */
	ModelRender m_plateModel;

	/** プレートの位置 */
	Vector3 m_position = Vector3::Zero;

	/** 発光シェーダーへ渡すデータの実体(描画のたびに読まれるのでメンバとして持ち続ける) */
	SEmissiveData m_emissiveData;

	/** 明滅用のタイマー */
	float m_glowTimer = 0.0f;

	/** プレートを(段ボールが)踏んだかどうか */
	bool m_isSteppedOn = false;

};

