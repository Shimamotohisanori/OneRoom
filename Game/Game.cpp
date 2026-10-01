#include "stdafx.h"
#include "Game.h"
#include "Source/Actor/Player/Player.h"
#include "Source/Actor/Stage/MainStage.h"
#include "GameCamera/GameCamera.h"
bool Game::Start()
{
	m_mainStage = NewGO<MainStage>(0, "MainStage");
	
	m_player = NewGO<Player>(0, "Player");
	
	m_gameCamera = NewGO<GameCamera>(0, "GameCamera");
	
	/** 空の初期化 */
	m_skyCube = NewGO<SkyCube>(0, "SkyCube");
	m_skyCube->SetType(EnSkyCubeType::enSkyCubeType_Night);
	m_skyCube->SetScale(10000.0f);

	/** 光の方向及び影の設定 */
	Vector3 sunDirection = { 0.0f, -1.0f, 0.0f };
	sunDirection.Normalize();
	g_renderingEngine->SetDirectionLight(0,sunDirection,Vector3(5.0f, 5.0f, 5.0f));

	/** IBL 設定 */
	g_renderingEngine->SetAmbientByIBLTexture(m_skyCube->GetTextureFilePath(), 0.95f);

	/** ブルームを抑制 */
	g_renderingEngine->SetBloomThreshold(3.0f);

	return true;
}

void Game::Update()
{
	// g_renderingEngine->DisableRaytracing();
}

void Game::Render(RenderContext& rc)
{
	
}