#include "stdafx.h"
#include "GameOver.h"
#include "GameScene/Title.h"
#include "Source/Actor/Player/Player.h"
#include "Source/Actor/Stage/MainStage.h"
#include "GameCamera/GameCamera.h"
#include "SAN/SANUI.h"
#include "Source/Actor/Stage/PlateRoom/Plate/Plate.h"
#include "Source/Actor/Stage/PlateRoom/DanBall/DanBall.h"
#include "IntroductionUI/IntroductionUI.h"
#include "Key/Key.h"
#include "Key/KeyUI/KeyUI.h"
#include "SAN/SANCalculation.h"
#include "Source/Actor/Stage/DarkRoom/Paper/Paper.h"
#include "Source/Actor/Stage/DarkRoom/KeyPad/KeyPad.h"
namespace
{
	/** ゲームオーバー画面のファイルパス */
	constexpr const char* GAMEOVER_FILE_PATH = "Assets/sprite/GameOver/GameOver.dds";
	/** ゲームオーバー画面の横幅 */
	constexpr float GAMEOVER_WIDTH = 1920.0f;
	/** ゲームオーバー画面の縦幅 */
	constexpr float GAMEOVER_HEIGHT = 1080.0f;
}
GameOver::GameOver()
{}
GameOver::~GameOver()
{}

bool GameOver::Start()
{
	/** メインステージを取得 */
	m_mainStage = FindGO<MainStage>("MainStage");
	
	/** メインステージを削除 */
	DeleteGO(m_mainStage);

	/** プレイヤーを取得 */
	m_player = FindGO<Player>("Player");
	
	/** プレイヤーを削除 */
	DeleteGO(m_player);

	/** ゲームカメラを取得 */
	m_gameCamera = FindGO<GameCamera>("GameCamera");
	
	/** ゲームカメラを削除 */
	DeleteGO(m_gameCamera);
	
	/** 空を取得 */
	m_skyCube = FindGO<SkyCube>("SkyCube");

	/** 空を削除 */
	DeleteGO(m_skyCube);

	/** SAN UIを取得 */
	m_sanUI = FindGO<SANUI>("sanUI");	

	/** SAN UIを削除 */
	DeleteGO(m_sanUI);

	/** プレートを取得 */
	m_plate = FindGO<Plate>("Plate");
	
	/** プレートを削除 */
	DeleteGO(m_plate);

	/** 段ボールを取得 */
	m_danBall = FindGO<DanBall>("DanBall");

	/** 段ボールを削除 */
	DeleteGO(m_danBall);

	/** 操作紹介UIを取得 */
	m_introductionUI = FindGO<IntroductionUI>("introductionUI");

	/** 操作紹介UIを削除 */
	DeleteGO(m_introductionUI);

	/** キーを取得 */
	m_key = FindGO<Key>("Key");

	/** キーを削除 */
	DeleteGO(m_key);

	/** キーUIを取得 */
	m_keyUI = FindGO<KeyUI>("keyUI");

	/** キーUIを削除 */
	DeleteGO(m_keyUI);

	/** SAN計算クラスを取得 */
	m_sanCalculation = FindGO<SANCalculation>("SANCalculation");

	/** SAN計算クラスを削除 */
	DeleteGO(m_sanCalculation);

	/** 紙を取得 */
	m_paper = FindGO<Paper>("Paper");

	/** 紙を削除 */
	DeleteGO(m_paper);

	/** キーパッドを取得 */
	m_keyPad = FindGO<KeyPad>("KeyPad");

	/** キーパッドを削除 */
	DeleteGO(m_keyPad);

	m_gameOverSprite.Init(GAMEOVER_FILE_PATH, GAMEOVER_WIDTH, GAMEOVER_HEIGHT);
	return true;
}

void GameOver::Update()
{
	if (g_pad[0]->IsTrigger(enButtonA))
	{
		NewGO<Title>(0, "title");
		DeleteGO(this);
		return;
	}

	m_gameOverSprite.Update();
}

void GameOver::Render(RenderContext & rc)
{
	m_gameOverSprite.Draw(rc);
}
