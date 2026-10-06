#include "stdafx.h"
#include "Game.h"
#include "SAN/SANUI.h"
#include "GameScene/GameOver.h"
#include "Key/KeyUI/KeyUI.h"
#include "IntroductionUI/IntroductionUI.h"
#include "SAN/SANCalculation.h"

Game::Game()
{}
Game::~Game()
{}

bool Game::Start()
{
	/** SANUIの生成 */
	m_sanUI = NewGO<SANUI>(0, "sanUI");

	/** KeyUIの生成 */
	m_keyUI = NewGO<KeyUI>(0, "keyUI");
	
	/** 操作紹介UIの生成 */
	m_introductionUI = NewGO<IntroductionUI>(0, "introductionUI");

	/** SAN計算クラスの取得 */
	m_sanCalculation = FindGO<SANCalculation>("SANCalculation");

	return true;
}

void Game::Update()
{
	Death();
	// g_renderingEngine->DisableRaytracing();
}

void Game::Render(RenderContext& rc)
{
	
}

void Game::Death()
{
	/** SAN値が0以下になった場合、ゲームオーバー処理を行う */
	if (m_sanCalculation->GetSANValue() <= 0)
	{
		NewGO<GameOver>(0,"GameOver");
		DeleteGO(this);
	}
}
