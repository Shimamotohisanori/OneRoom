#include "stdafx.h"
#include "Plate.h"
namespace
{
	/** プレートモデルデータのパス */
	const char* PLATE_MODEL_PATH = "Assets/ModelData/Plate/Plate.tkm";

	/** プレートの座標 */
	const Vector3 PLATE_POSITION = Vector3(1200.0f, -140.0f, -260.0f);

	/** プレートのスケール */
	const Vector3 PLATE_SCALE = Vector3(1.0f, 1.0f, 1.0f);

	/** 段ボールを置ける距離 */
	constexpr float PLACEMENT_DISTANCE = 150.0f;
}

Plate::Plate()
{}

Plate::~Plate()
{}

bool Plate::Start()
{
	/** プレートの座標を設定 */
	m_position = PLATE_POSITION;

	/** プレートモデルの初期化 */
	m_plateModel.Init(PLATE_MODEL_PATH);

	m_plateModel.SetPosition(m_position);

	m_plateModel.SetScale(PLATE_SCALE);
	return true;
}

void Plate::Update()
{
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
