#include "stdafx.h"
#include "GameClear.h"
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
#include "Source/Actor/Stage/HandSetRoom/Phone/Phone.h"
#include "Source/Actor/Stage/HandSetRoom/PhonePaper/PhonePaper.h"
namespace
{
	/** ゲームクリア画面のファイルパス */
	const char* GAMECLEAR_FILE_PATH = "Assets/sprite/GameClear/GameClear.dds";

	/** ゲームクリア画面の横幅 */
	constexpr float GAMECLEAR_WIDTH = 1920.0f;

	/** ゲームクリア画面の縦幅 */
	constexpr float GAMECLEAR_HEIGHT = 1080.0f;
}

GameClear::GameClear()
{}

GameClear::~GameClear()
{}

bool GameClear::Start()
{
	/** メインステージを削除 */
	m_mainStage = FindGO<MainStage>("MainStage");
	DeleteGO(m_mainStage);

	/** プレイヤーを削除 */
	m_player = FindGO<Player>("Player");
	DeleteGO(m_player);

	/** ゲームカメラを削除 */
	m_gameCamera = FindGO<GameCamera>("GameCamera");
	DeleteGO(m_gameCamera);

	/** 空を削除 */
	m_skyCube = FindGO<SkyCube>("SkyCube");
	DeleteGO(m_skyCube);

	/** SAN UIを削除 */
	m_sanUI = FindGO<SANUI>("sanUI");
	DeleteGO(m_sanUI);

	/** プレートを削除 */
	m_plate = FindGO<Plate>("Plate");
	DeleteGO(m_plate);

	/** 段ボールを削除 */
	m_danBall = FindGO<DanBall>("DanBall");
	DeleteGO(m_danBall);

	/** 操作紹介UIを削除 */
	m_introductionUI = FindGO<IntroductionUI>("introductionUI");
	DeleteGO(m_introductionUI);

	/** キーを削除 */
	m_key = FindGO<Key>("Key");
	DeleteGO(m_key);

	/** キーUIを削除 */
	m_keyUI = FindGO<KeyUI>("keyUI");
	DeleteGO(m_keyUI);

	/** SAN計算クラスを削除 */
	m_sanCalculation = FindGO<SANCalculation>("SANCalculation");
	DeleteGO(m_sanCalculation);

	/** 紙を削除 */
	m_paper = FindGO<Paper>("Paper");
	DeleteGO(m_paper);

	/** キーパッドを削除 */
	m_keyPad = FindGO<KeyPad>("KeyPad");
	DeleteGO(m_keyPad);

	/** 電話をすべて削除 */
	QueryGOs<Phone>("Phone", [](Phone* phone) -> bool
		{
			DeleteGO(phone);
			return true;
		});

	/** 電話の紙取得 */
	m_phonePaper = FindGO<PhonePaper>("PhonePaper");

	/** 電話の紙を削除 */
	DeleteGO(m_phonePaper);

	/** ゲームクリア画像を初期化 */
	m_clearSprite.Init(
		GAMECLEAR_FILE_PATH,
		GAMECLEAR_WIDTH,
		GAMECLEAR_HEIGHT
	);
	return true;
}

void GameClear::Update()
{
	/** Aボタンでタイトルへ戻る */
	if (g_pad[0]->IsTrigger(enButtonA))
	{
		NewGO<Title>(0, "title");
		DeleteGO(this);
		return;
	}
	m_clearSprite.Update();
}

void GameClear::Render(RenderContext & rc)
{
	m_clearSprite.Draw(rc);
}
