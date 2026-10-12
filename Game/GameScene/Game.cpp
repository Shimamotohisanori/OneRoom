#include "stdafx.h"
#include "Game.h"
#include "SAN/SANUI.h"
#include "GameScene/GameOver.h"
#include "Key/KeyUI/KeyUI.h"
#include "IntroductionUI/IntroductionUI.h"
#include "SAN/SANCalculation.h"
#include "Key/Key.h"
#include "GameScene/GameClear.h"
#include "Source/Actor/Player/Player.h"
namespace
{
	/** ゲームクリアになる場所 */
	constexpr float CLEAR_POSITION_Z = 1300.0f;

	/** SAN値が0 */
	constexpr int SAN_VALUE_ZERO = 0;

	/** 鍵の数 */
	constexpr int KEY_COUNT = 3;
}

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

	/** Keyの取得 */
	m_key = FindGO<Key>("Key");

	/** プレイヤーの取得 */
	m_player = FindGO<Player>("Player");

	return true;
}

void Game::Update()
{
	Death();

	Clear();
	// g_renderingEngine->DisableRaytracing();
}

void Game::Render(RenderContext& rc)
{
	
}

void Game::Death()
{
	/** SAN値が0以下になった場合、ゲームオーバー処理を行う */
	if (m_sanCalculation->GetSANValue() <= SAN_VALUE_ZERO)
	{
		NewGO<GameOver>(0,"GameOver");
		DeleteGO(this);
	}
}

void Game::Clear()
{
	/** 鍵を三つ持った状態で特定の場所に到達した場合、ゲームクリア処理を行う */
	if (m_key->GetKeyCount() >= KEY_COUNT &&
		m_player->GetPosition().z >= CLEAR_POSITION_Z)
	{
		NewGO<GameClear>(0,"GameClear");
		DeleteGO(this);
	}
}
