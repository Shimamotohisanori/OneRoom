#include "stdafx.h"
#include "MainStage.h"

namespace
{
	/** モデルデータのパス */
	const char* MAIN_STAGE_MODEL_PATH = "Assets/modelData/Stage/MainStage.tkm";
}

MainStage::MainStage()
{}

MainStage::~MainStage()
{}

bool MainStage::Start()
{
	m_mainStageRender.Init(MAIN_STAGE_MODEL_PATH);
	return true;
}

void MainStage::Update()
{
	m_mainStageRender.Update();
}

void MainStage::Render(RenderContext & rc)
{
	m_mainStageRender.Draw(rc);
}
