#include "stdafx.h"
#include "Game.h"
#include "Source/Actor/Player/Player.h"
#include "Source/Actor/Stage/MainStage.h"

bool Game::Start()
{
	m_mainStage = NewGO<MainStage>(0, "MainStage");
	
	m_player = NewGO<Player>(0, "Player");
	
	return true;
}

void Game::Update()
{
	// g_renderingEngine->DisableRaytracing();
}

void Game::Render(RenderContext& rc)
{
	
}