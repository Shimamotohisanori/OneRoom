#include "stdafx.h"
#include "PlayerModel.h"

PlayerModel::PlayerModel()
{}

PlayerModel::~PlayerModel()
{}

bool PlayerModel::Start()
{
	m_modelRender.Init("Assets/modelData/unityChan.tkm");
	return true;
}

void PlayerModel::Update()
{
	m_modelRender.Update();
}

void PlayerModel::Render(RenderContext & rc)
{
	m_modelRender.Draw(rc);
}
