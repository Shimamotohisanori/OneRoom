#include "stdafx.h"
#include "MainStage.h"

MainStage::MainStage()
{}

MainStage::~MainStage()
{}

bool MainStage::Start()
{
	m_mainStageRender.Init("Assets/modelData/Stage/MainStage.tkm");
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
