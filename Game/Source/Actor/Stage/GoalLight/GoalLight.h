#pragma once
/** ゴールの先にある光る板オブジェクトのクラス */
class Player;

enum class EnGoalLightState
{
	enGoalLightState_Hidden,	  /** 非表示 */
	enGoalLightState_Shining,	  /** 表示 */
	enGoalLightState_WhiteOut,    /** 画面が白くなっていく */
	enGoalLightState_Cleared,	  /** 画面が白くなりきった */
};

class GoalLight : public IGameObject
{
public:
	GoalLight();
	~GoalLight();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	/** 光を見えるようにする(GoalDoorが扉を開けた時に呼ぶ) */
	void Show();

	/** 光の位置と、判定の大きさを設定する */
	void SetLight(const Vector3& position, const Vector3& halfSize)
	{
		m_lightPosition = position;
		m_lightHalfSize = halfSize;
	}

	/** 光る板の大きさ(拡大率)を設定する。Start()より前に呼ぶこと */
	void SetLightScale(const Vector3& scale)
	{
		m_lightScale = scale;
	}

	/** クリアしたかどうか(Gameクラスが、クリア画面へ切り替える判断に使う) */
	bool IsCleared() const
	{
		return m_state == EnGoalLightState::enGoalLightState_Cleared
			&& m_clearedTimer >= 0.5f;
	}

private:
	/** プレイヤーが光る箱の中に入ったか判定 */
	bool IsPlayerInLight() const;

private:
	/** 発光シェーダーへ渡すデータ。emissiveLight.fx の EmissiveCb と並びを合わせること */
	struct SEmissiveData
	{
		Vector3 emissiveColor;	// 発光色
		float maskThreshold;	// 光らせる部分の輝度しきい値
		float baseBrightness;	// 光らない部分の明るさ
		float pad[3];			// 16バイト境界に合わせる詰め物
	};

	/** 発光シェーダーへ渡すデータの実体。メンバとして持ち続けること */
	SEmissiveData m_emissiveData;

	/** プレイヤー */
	Player* m_player = nullptr;

	/** 光る板の拡大率 */
	Vector3 m_lightScale = Vector3(1.0f, 1.0f, 1.0f);

	/** 画面全体を覆う白い画像 */
	SpriteRender m_whiteSprite;

	/** 光る箱の中心と、大きさ(半分の長さ) */
	Vector3 m_lightPosition = Vector3::Zero;
	Vector3 m_lightHalfSize = Vector3::Zero;

	EnGoalLightState m_state = EnGoalLightState::enGoalLightState_Hidden;

	/** 白の濃さ(0.0〜1.0) */
	float m_whiteAlpha = 0.0f;

	/** ホワイトアウトの速さ(1秒で1.0になる) */
	float m_clearedTimer = 0.0f;

	/** 光る板の枚数(前・左・右・上) */
	static const int LIGHT_MODEL_NUM = 4;

	/** 光る板(前・左・右・上) */
	ModelRender m_lightModel[LIGHT_MODEL_NUM];

	/** 扉の先から差し込む光(前・左・右・上の壁ごとに1つ) */
	SpotLight m_spotLight[LIGHT_MODEL_NUM];

};

