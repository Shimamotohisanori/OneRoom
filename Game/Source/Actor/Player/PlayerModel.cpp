#include "stdafx.h"
#include "PlayerModel.h"

namespace
{
	/** モデルデータのパス */
	const char* PLAYER_MODEL_PATH = "Assets/modelData/unityChan.tkm";
}

PlayerModel::PlayerModel()
{}

PlayerModel::~PlayerModel()
{}

bool PlayerModel::Start()
{
	m_modelRender.Init(PLAYER_MODEL_PATH);
	return true;
}

void PlayerModel::Update()
{
	m_modelRender.Update();
	m_modelRender.SetPosition(m_position);
	m_modelRender.SetRotation(m_rotation);
}

void PlayerModel::Render(RenderContext & rc)
{
	m_modelRender.Draw(rc);
}
