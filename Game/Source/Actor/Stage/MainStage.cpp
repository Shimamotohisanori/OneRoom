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
	
	m_mainStagePhysics.CreateFromModel(
		m_mainStageRender.GetModel(),
		m_mainStageRender.GetModel().GetWorldMatrix()
	);
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
