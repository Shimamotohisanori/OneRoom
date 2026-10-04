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
}

Plate::Plate()
{}

Plate::~Plate()
{}

bool Plate::Start()
{
	m_plateModel.Init(PLATE_MODEL_PATH);

	m_plateModel.SetPosition(PLATE_POSITION);

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

void Plate::OnSteppedOn()
{
	/** プレートを踏んだときの処理 */
	m_isSteppedOn = true;
}
