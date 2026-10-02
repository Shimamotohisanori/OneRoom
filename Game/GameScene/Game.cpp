#include "stdafx.h"
#include "Game.h"
#include "SAN/SANUI.h"
bool Game::Start()
{
	m_sanUI = NewGO<SANUI>(0, "sanUI");
	return true;
}

void Game::Update()
{
	// g_renderingEngine->DisableRaytracing();
}

void Game::Render(RenderContext& rc)
{
	
}