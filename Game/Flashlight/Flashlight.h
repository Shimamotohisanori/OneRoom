#pragma once
#include "graphics/light/SpotLight.h"

/**
 * Flashlight.h
 * フラッシュライトクラス
 * モデル(右手側に表示)とスポットライトをまとめて管理する。
 * Playerが所有し、Start/Update/Renderを呼ぶ。
 */
class Flashlight
{
public:
	Flashlight();
	~Flashlight();

	/** 初期化(モデルとスポットライトを作る) */
	void Start();

	/** 更新。毎フレーム、一人称カメラの情報を元に位置を合わせる */
	void Update();

	/** 描画(モデル) */
	void Render(RenderContext& rc);

	/** ON/OFFを設定 */
	void SetOn(bool isOn) { m_isOn = isOn; }

	/** ONかどうか(目標状態) */
	bool IsOn() const { return m_isOn; }

private:
	/** カメラ基準でモデルの位置・向きを決める */
	void UpdateTransform();

	/** 点灯の強さ(フェード、ちらつき)を更新する */
	void UpdateIntensity();

	/** スポットライトのパラメータを反映する */
	void UpdateSpotLight();

private:
	/** モデルレンダラー */
	ModelRender m_model;

	/** スポットライト */
	nsK2Engine::SpotLight m_spotLight;

	/** ON/OFF状態(目標状態) */
	bool m_isOn = false;

	/** 現在の明るさ倍率(0〜1)。ON/OFFで滑らかに変化 */
	float m_intensity = 0.0f;

	/** 揺れ(ライトの向きがカメラに少し遅れて付いてくる) */
	Vector3 m_smoothedForward = Vector3::AxisZ;

	/** ちらつき用タイマー */
	float m_flickerTimer = 0.0f;

	/** ライトの位置 */
	Vector3 m_lightPosition = Vector3::Zero;

	/** ライトの向き */
	Vector3 m_lightDirection = Vector3::AxisZ;
};
