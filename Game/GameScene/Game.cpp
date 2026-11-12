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
#include "Source/Actor/Stage/GoalLight/GoalLight.h"
#include "Source/Actor/Stage/DarkRoom/KeyPad/KeyPad.h"
#include "Source/Actor/Stage/DarkRoom/Paper/Paper.h"
#include "Source/Actor/Stage/HandSetRoom/Phone/Phone.h"
#include "Source/Actor/Stage/HandSetRoom/PhonePaper/PhonePaper.h"
#include "Fade.h"
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

	/** ゴールの光の取得 */
	m_goalLight = FindGO<GoalLight>("GoalLight");

	/** ゲーム中BGMの再生 */
	SoundManager* sound = FindGO<SoundManager>("soundManager");
	m_gameBGM = sound->PlayingBGM(enGame, true);

	/** キーパッドの取得 */
	m_keyPad = FindGO<KeyPad>("KeyPad");

	/** 紙の取得 */
	m_paper = FindGO<Paper>("Paper");

	/** 電話の取得 */
	m_phone = FindGO<Phone>("Phone");

	/** 電話の紙の取得 */
	m_phonePaper = FindGO<PhonePaper>("PhonePaper");

	return true;
}

void Game::Update()
{

	/** Start()が呼ばれたので、ゲームが開始できる状態にする */
	m_isReady = true;

	if (Death()) { return; }
	if (Clear()) { return; }

	/** キーパッド、紙、電話、電話の紙を開いてる間はポーズしない */
	if (m_keyPad->GetIsKeypadOpen() || m_paper->GetIsViewing() ||
		m_phone->GetIsPhoneOpen() || m_phonePaper->GetIsViewing())
	{
		return;
	}
		GamePause();
	//g_renderingEngine->DisableRaytracing();
}

void Game::Render(RenderContext& rc)
{
	
}

bool Game::Death()
{

	/** 死んでなくて */
	if (!m_isDying)
	{
		/** SAN値が0より大きい場合、ゲームオーバー処理を行わない */
		if (m_sanCalculation->GetSANValue() > SAN_VALUE_ZERO) { return false; }

		/** プレイヤーの操作を無効化 */
		m_player->SetInputEnabled(false);

		/** 画面をブラックアウト */
		FindGO<Fade>("Fade")->StartFadeOut(Vector3(0.0f, 0.0f, 0.0f),3.0f);
		
		/** 死んだ */
		m_isDying = true;
	}

	/** 完全に暗くなったら、ゲームオーバー画面へ */
	if (FindGO<Fade>("Fade")->IsFullyCovered())
	{
		NewGO<GameOver>(0, "GameOver");
		DeleteGO(this);
	}
	return true;
}

bool Game::Clear()
{
	/** ゴールのドアが開き切るまではクリアにしない */
	if (m_goalDoor == nullptr || !m_goalDoor->IsOpenCompleted())
	{
		return false;
	}

	/** ゴールの光がクリア状態になるまではクリアにしない */
	if (m_goalLight == nullptr || !m_goalLight->IsCleared())
	{
		return false;
	}

	/** 鍵を三つ持った状態で扉を開け、光がクリア状態になる場合、ゲームクリア処理を行う */
	NewGO<GameClear>(0,"GameClear");
	DeleteGO(this);
	return true;
}

void Game::GamePause()
{
	if (m_pause->GetIsPaused()) { return; }

	if (!m_player->IsPauseTriggered()) { return; }

	/** フェード中は、ポーズを二重に開かない */
	Fade* fade = FindGO<Fade>("Fade");
	if (fade->IsBusy()) { return; }

	/** プレイヤーの操作を無効化 */
	m_player->SetInputEnabled(false);

	/** 暗転して、真っ暗になった瞬間にポーズ画面へ切り替え、明転する */
	fade->StartTransition(Vector3(0.0f, 0.0f, 0.0f), 0.3f, 0.3f, [this]()
		{
			m_pause->SetIsPaused(true);
		});
}
