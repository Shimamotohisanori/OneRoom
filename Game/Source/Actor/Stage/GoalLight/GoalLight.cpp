#include "stdafx.h"
#include "GoalLight.h"
#include "Source/Actor/Player/Player.h"

namespace
{
	constexpr float WHITEOUT_SPEED = 0.8f;

	/** 発光シェーダーのファイルパス */
	const char* EMISSIVE_FX_FILE_PATH = "Assets/shader/emissiveLight.fx";

	/** ゴールの光のモデル */
	const char* GOAL_LIGHT_MODEL_PATH = "Assets/modelData/LightObject/GoalLight/GoalLight.tkm";

	/** 発光の色。LoadingSceneのブルームのしきい値(3.0)を超えると、にじんで見える */
	const Vector3 EMISSIVE_COLOR = Vector3(3.0f, 3.0f, 3.0f);

	/** 光らせる部分の輝度しきい値。0以下ならモデル全体が光る */
	constexpr float EMISSIVE_MASK_THRESHOLD = 0.0f;

	/** 光らない部分の明るさ */
	constexpr float EMISSIVE_BASE_BRIGHTNESS = 0.05f;

	/** 差し込む光の色。扉を開けるまでは0(消灯) */
	const Vector3 SPOT_COLOR = Vector3(3.0f, 3.0f, 3.0f);

	/** 光の向き。扉の手前(部屋側)へ向ける */
	const Vector3 SPOT_DIRECTION = Vector3(0.0f, 0.0f, -1.0f);

	/** 板の枚数 */
	constexpr int LIGHT_MODEL_NUM = 4;

	/** 各板の、基準位置からのずれ。順番は 前・左・右・上。値は仮。 */
	const Vector3 LIGHT_OFFSETS[LIGHT_MODEL_NUM] =
	{
		Vector3(0.0f,   0.0f,  200.0f),	// 前(奥側)
		Vector3(-200.0f,   -50.0f,    0.0f),	// 左
		Vector3(200.0f,   -50.0f,    10.0f),	// 右
		Vector3(0.0f, 250.0f,   -300.0f),	// 上
	};

	/** 各板の向き(度)。X, Y, Z 軸まわり。値は仮。 */
	const Vector3 LIGHT_ROTATIONS[LIGHT_MODEL_NUM] =
	{
		Vector3(0.0f,   0.0f, 0.0f),		// 前
		Vector3(0.0f,  90.0f, 0.0f),		// 左
		Vector3(0.0f, -90.0f, 0.0f),		// 右
		Vector3(90.0f,   0.0f, 0.0f),		// 上
	};

	/** 各壁のライトの向き。順番は 前・左・右・上。壁から通路の内側へ向ける */
	const Vector3 SPOT_DIRECTIONS[LIGHT_MODEL_NUM] =
	{
		Vector3(0.0f,  0.0f, -1.0f),	// 前の壁 → 部屋側へ
		Vector3(1.0f,  0.0f,  0.0f),	// 左の壁 → 右へ
		Vector3(-1.0f,  0.0f,  0.0f),	// 右の壁 → 左へ
		Vector3(0.0f, -1.0f,  0.0f),	// 上の壁 → 下へ
	};

	/** 光の届く距離 */
	constexpr float SPOT_RANGE = 500.0f;

	/** 光の射出角度(度) */
	constexpr float SPOT_ANGLE_DEG = 30.0f;

	/** 減衰の累乗(1.0で線形) */
	constexpr float SPOT_AFFECT_POW = 1.0f;
}

GoalLight::GoalLight()
{
}

GoalLight::~GoalLight()
{
	for (int i = 0; i < LIGHT_MODEL_NUM; i++) {
		m_spotLight[i].SetColor(g_vec3Zero);
		m_spotLight[i].SetColor2(g_vec3Zero);
		m_spotLight[i].SetColor3(g_vec3Zero);
		m_spotLight[i].Update();
	}
}

bool GoalLight::Start()
{
	m_player = FindGO<Player>("Player");

	/** 発光シェーダーに渡すデータを入れる */
	m_emissiveData.emissiveColor = EMISSIVE_COLOR;
	m_emissiveData.maskThreshold = EMISSIVE_MASK_THRESHOLD;
	m_emissiveData.baseBrightness = EMISSIVE_BASE_BRIGHTNESS;

	/** 発光シェーダーでモデルを初期化(フォワードレンダリング) */
	ModelInitData initData;
	initData.m_tkmFilePath = GOAL_LIGHT_MODEL_PATH;
	initData.m_fxFilePath = EMISSIVE_FX_FILE_PATH;
	initData.m_vsEntryPointFunc = "VSMain";
	initData.m_psEntryPointFunc = "PSMain";
	initData.m_modelUpAxis = enModelUpAxisZ;
	initData.m_expandConstantBuffer = &m_emissiveData;
	initData.m_expandConstantBufferSize = sizeof(m_emissiveData);
	/** 発光シェーダーでモデルを初期化(4枚とも同じ設定) */
	for (int i = 0; i < LIGHT_MODEL_NUM; i++)
	{
		m_lightModel[i].InitForwardRendering(initData);

		Quaternion rot;
		rot.SetRotationDegX(LIGHT_ROTATIONS[i].x);
		Quaternion rotY;
		rotY.SetRotationDegY(LIGHT_ROTATIONS[i].y);
		rot.Multiply(rotY);

		m_lightModel[i].SetPosition(m_lightPosition + LIGHT_OFFSETS[i]);
		m_lightModel[i].SetRotation(rot);
		m_lightModel[i].SetScale(m_lightScale);
		m_lightModel[i].Update();
	}

	/** 真っ白な画像(画面全体のサイズ) */
	m_whiteSprite.Init("Assets/sprite/White/White.dds", 1920.0f, 1080.0f);
	m_whiteSprite.SetPosition(Vector3(0.0f, 0.0f, 0.0f));
	m_whiteSprite.SetMulColor(Vector4(1.0f, 1.0f, 1.0f, 0.0f));
	m_whiteSprite.Update();

	/** スポットライトを初期化。最初は消灯 */
	const float angleRad = Math::DegToRad(SPOT_ANGLE_DEG);
	for (int i = 0; i < LIGHT_MODEL_NUM; i++)
	{
		m_spotLight[i].Init();
		m_spotLight[i].SetPosition(m_lightPosition + LIGHT_OFFSETS[i]);
		m_spotLight[i].SetDirection(SPOT_DIRECTIONS[i]);
		m_spotLight[i].SetColor(g_vec3Zero);
		m_spotLight[i].SetColor2(g_vec3Zero);
		m_spotLight[i].SetColor3(g_vec3Zero);
		m_spotLight[i].SetRange(SPOT_RANGE);
		m_spotLight[i].SetAngle(angleRad);
		m_spotLight[i].SetAngle2(angleRad);
		m_spotLight[i].SetAngle3(angleRad);
		m_spotLight[i].SetRangeAffectPowParam(SPOT_AFFECT_POW);
		m_spotLight[i].SetRangeAffectPowParam2(SPOT_AFFECT_POW);
		m_spotLight[i].SetRangeAffectPowParam3(SPOT_AFFECT_POW);
		m_spotLight[i].SetAngleAffectPowParam(SPOT_AFFECT_POW);
		m_spotLight[i].SetAngleAffectPowParam2(SPOT_AFFECT_POW);
		m_spotLight[i].SetAngleAffectPowParam3(SPOT_AFFECT_POW);
		m_spotLight[i].Update();
	}
	return true;
}

void GoalLight::Show()
{
	if (m_state == EnGoalLightState::enGoalLightState_Hidden) {
		m_state = EnGoalLightState::enGoalLightState_Shining;

		/** 扉を開けた瞬間に、4つのライトを点ける */
		for (int i = 0; i < LIGHT_MODEL_NUM; i++) {
			m_spotLight[i].SetColor(SPOT_COLOR);
			m_spotLight[i].Update();
		}
	}
}

bool GoalLight::IsPlayerInLight() const
{
	/** 光る箱の範囲(AABB)に、プレイヤーの座標が入っているか調べる */
	Vector3 diff = m_player->GetPosition() - m_lightPosition;
	return fabsf(diff.x) < m_lightHalfSize.x
		&& fabsf(diff.y) < m_lightHalfSize.y
		&& fabsf(diff.z) < m_lightHalfSize.z;
}

void GoalLight::Update()
{
	switch (m_state)
	{
	case EnGoalLightState::enGoalLightState_Hidden:
		/** 扉が閉まっている間は、光る板も見えない */
		break;

	case EnGoalLightState::enGoalLightState_Shining:
		for (int i = 0; i < LIGHT_MODEL_NUM; i++) {
			m_lightModel[i].Update();
		}
		if (IsPlayerInLight()) {
			/** プレイヤーが光る箱の中に入った → ホワイトアウト開始 */
			m_player->SetInputEnabled(false);
			m_state = EnGoalLightState::enGoalLightState_WhiteOut;
		}
		break;

	case EnGoalLightState::enGoalLightState_WhiteOut:
		for (int i = 0; i < LIGHT_MODEL_NUM; i++) {
			m_lightModel[i].Update();
		}
		m_whiteAlpha += WHITEOUT_SPEED * g_gameTime->GetFrameDeltaTime();
		if (m_whiteAlpha >= 1.0f) {
			m_whiteAlpha = 1.0f;
			m_state = EnGoalLightState::enGoalLightState_Cleared;
		}
		m_whiteSprite.SetMulColor(Vector4(1.0f, 1.0f, 1.0f, m_whiteAlpha));
		m_whiteSprite.Update();
		break;

	case EnGoalLightState::enGoalLightState_Cleared:
		m_clearedTimer += g_gameTime->GetFrameDeltaTime();
		break;
	}
}

void GoalLight::Render(RenderContext& rc)
{
	/** 光る板は、扉が開いてから表示する */
	if (m_state != EnGoalLightState::enGoalLightState_Hidden) {
		for (int i = 0; i < LIGHT_MODEL_NUM; i++) {
			m_lightModel[i].Draw(rc);
		}
	}

	/** 白い画像は、ホワイトアウト中とクリア後だけ描く */
	if (m_state == EnGoalLightState::enGoalLightState_WhiteOut
		|| m_state == EnGoalLightState::enGoalLightState_Cleared) {
		m_whiteSprite.Draw(rc);
	}
}