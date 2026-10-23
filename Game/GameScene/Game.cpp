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
#include "SoundManager/SoundManager.h"
#include "Pause.h"
#include "Source/Actor/Stage/Door/GoalDoor/GoalDoor.h"
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
{
	if(m_gameBGM != nullptr)
	{
		DeleteGO(m_gameBGM);
	}

	if (m_pause != nullptr)
	{
		DeleteGO(m_pause);
	}
}

bool Game::Start()
{
	/** SANUIの生成 */
	m_sanUI = NewGO<SANUI>(0, "SanUI");

	/** KeyUIの生成 */
	m_keyUI = NewGO<KeyUI>(0, "KeyUI");
	
	/** 操作紹介UIの生成 */
	m_introductionUI = NewGO<IntroductionUI>(0, "IntroductionUI");

	/** SAN計算クラスの取得 */
	m_sanCalculation = FindGO<SANCalculation>("SANCalculation");

	/** Keyの取得 */
	m_key = FindGO<Key>("Key");

	/** プレイヤーの取得 */
	m_player = FindGO<Player>("Player");

	/** 一時停止クラスの生成 */
	m_pause = NewGO<Pause>(0, "Pause");

	/** ゴールのドアの取得 */
	m_goalDoor = FindGO<GoalDoor>("GoalDoor");

	/** ゲーム中BGMの再生 */
	SoundManager* sound = FindGO<SoundManager>("soundManager");
	m_gameBGM = sound->PlayingBGM(enGame, true);

	return true;
}

void Game::Update()
{
	if (Death()) { return; }
	if (Clear()) { return; }
	GamePause();
	//g_renderingEngine->DisableRaytracing();
}

void Game::Render(RenderContext& rc)
{
	
}

bool Game::Death()
{
	/** SAN値が0以下になった場合、ゲームオーバー処理を行う */
	if (m_sanCalculation->GetSANValue() <= SAN_VALUE_ZERO)
	{
		NewGO<GameOver>(0,"GameOver");
		DeleteGO(this);
		return true;
	}
	return false;
}

bool Game::Clear()
{
	/** ゴールのドアが開き切るまではクリアにしない */
	if (m_goalDoor == nullptr || !m_goalDoor->IsOpenCompleted())
	{
		return false;
	}

	/** 鍵を三つ持った状態で特定の場所に到達した場合、ゲームクリア処理を行う */
	if (m_key->GetKeyCount() >= KEY_COUNT &&
		m_player->GetPosition().z >= CLEAR_POSITION_Z)
	{
		NewGO<GameClear>(0,"GameClear");
		DeleteGO(this);
		return true;
	}

	return false;
}

void Game::GamePause()
{
	if(m_player->IsPauseTriggered())
	{
		/** 一時停止状態にする */
		m_pause->SetIsPaused(true);

		/** プレイヤーの操作を無効化 */
		m_player->SetInputEnabled(false);
	}
}
