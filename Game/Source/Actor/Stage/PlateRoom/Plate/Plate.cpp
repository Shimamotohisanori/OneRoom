#include "stdafx.h"
#include "Plate.h"
namespace
{
	/** プレートモデルデータのパス */
	const char* PLATE_MODEL_PATH = "Assets/ModelData/Plate/Plate.tkm";

	/** 発光シェーダーのファイルパス */
	const char* EMISSIVE_FX_FILE_PATH = "Assets/shader/emissiveLight.fx";

	/** プレートの座標 */
	const Vector3 PLATE_POSITION = Vector3(1200.0f, -140.0f, -260.0f);

	/** プレートのスケール */
	const Vector3 PLATE_SCALE = Vector3(1.0f, 1.0f, 1.0f);

	/** 発光色 */
	const Vector3 PLATE_GLOW_COLOR = Vector3(1.0f, 1.0f, 1.0f);

	/** 段ボールを置ける距離 */
	constexpr float PLACEMENT_DISTANCE = 150.0f;

	/** 発光の強さの範囲(この間を往復する) */
	constexpr float GLOW_POWER_MIN = 0.01f;
	constexpr float GLOW_POWER_MAX = 0.05f;

	/** 明滅の速さ(ラジアン/秒) */
	constexpr float GLOW_PULSE_SPEED = 5.0f;

	/** 光らせる部分の輝度しきい値(0以下でモデル全体) */
	constexpr float EMISSIVE_MASK_THRESHOLD = 0.0f;

	/** 光らない部分の明るさ */
	constexpr float EMISSIVE_BASE_BRIGHTNESS = 0.0f;
}

Plate::Plate()
{}

Plate::~Plate()
{}

bool Plate::Start()
{
	/** プレートの座標を設定 */
	m_position = PLATE_POSITION;

	/** 発光データに初期値を入れる */
	UpdateGlow(0.0f);

	/** 発光シェーダーでモデルを初期化(フォワードレンダリング) */
	ModelInitData initData;
	initData.m_tkmFilePath = PLATE_MODEL_PATH;
	initData.m_fxFilePath = EMISSIVE_FX_FILE_PATH;
	initData.m_vsEntryPointFunc = "VSMain";
	initData.m_psEntryPointFunc = "PSMain";
	initData.m_modelUpAxis = enModelUpAxisZ;   // Plate.tkm の上軸に合わせる
	initData.m_expandConstantBuffer = &m_emissiveData;
	initData.m_expandConstantBufferSize = sizeof(m_emissiveData);
	m_plateModel.InitForwardRendering(initData);

	m_plateModel.SetPosition(m_position);
	m_plateModel.SetScale(PLATE_SCALE);
	return true;
}

void Plate::Update()
{
	/** 発光の更新 */
	UpdateGlow(g_gameTime->GetFrameDeltaTime());

	m_plateModel.Update();
}

void Plate::Render(RenderContext & renderContext)
{
	m_plateModel.Draw(renderContext);
}

bool Plate::IsInPlacementRange(const Vector3& position) const
{
	/** 指定座標がプレートの設置範囲内か(高さは無視) */
	Vector3 diff = position - m_position;
	diff.y = 0.0f;

	/** 設置範囲内ならtrueを返す */
	return diff.Length() <= PLACEMENT_DISTANCE;
}

void Plate::OnSteppedOn()
{
	/** プレートを踏んだときの処理 */
	m_isSteppedOn = true;
}

void Plate::UpdateGlow(float deltaTime)
{
	m_glowTimer += deltaTime;

	/** sinを0~1に直して、MIN~MAXの間を往復させる */
	const float wave = 1.5f + 1.5f * sinf(m_glowTimer * GLOW_PULSE_SPEED);
	float power = GLOW_POWER_MIN + (GLOW_POWER_MAX - GLOW_POWER_MIN) * wave;

	/** 踏んだ後は光らせない(操作済みが分かる) */
	if (m_isSteppedOn) {
		power = 0.0f;
	}

	m_emissiveData.emissiveColor = PLATE_GLOW_COLOR;
	m_emissiveData.emissiveColor *= power;
	m_emissiveData.maskThreshold = EMISSIVE_MASK_THRESHOLD;
	m_emissiveData.baseBrightness = EMISSIVE_BASE_BRIGHTNESS;
}
