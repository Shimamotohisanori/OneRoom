#include "stdafx.h"
#include "LoadingScene.h"
#include "Source/Actor/Player/Player.h"
#include "Source/Actor/Stage/MainStage.h"
#include "GameCamera/GameCamera.h"
#include "Game.h"
#include "Source/Actor/Stage/PlateRoom/Plate/Plate.h"
#include "Source/Actor/Stage/PlateRoom/DanBall/DanBall.h"
namespace
{
	/** ローディング画面のファイルパス */
	constexpr const char* LOADING_FILE_PATH = "Assets/sprite/Loading/Loading.dds";

	/** ローディング画面の横幅 */
	constexpr float LOADING_WIDTH = 1920.0f;

	/** ローディング画面の縦幅 */
	constexpr float LOADING_HEIGHT = 1080.0f;

	/** 空の大きさ */
	constexpr float SKY_CUBE_SIZE = 10000.0f;

	/** IBLの強さ */
	constexpr float IBL_STRENGTH = 0.95f;

	/** ブルームの強さ */
	constexpr float BLOOM_STRENGTH = 3.0f;

	/** ローディングの進捗状況の初期値 */
	constexpr int LOADING_PROGRESS_INITIAL = 0;

	/** 光の方向 */
	const Vector3 LIGHT_DIRECTION = { 0.0f, -1.0f, 0.0f };

	/** 光の色 */
	const Vector3 LIGHT_COLOR = { 5.0f, 5.0f, 5.0f };
}

LoadingScene::~LoadingScene()
{}

bool LoadingScene::Start()
{
	m_loadingSprite.Init(LOADING_FILE_PATH, LOADING_WIDTH, LOADING_HEIGHT);
	m_loadingSprite.SetPosition(Vector3::Zero);
	m_loadingProgress = LOADING_PROGRESS_INITIAL;

	return true;
}

void LoadingScene::Update()
{
	m_loadingSprite.Update();

	/** ゲームをロードする */
	LoadGame();
}

void LoadingScene::Render(RenderContext & rc)
{
	m_loadingSprite.Draw(rc);
}

void LoadingScene::LoadGame()
{
	switch (m_loadingProgress)
	{
	case 0:
		m_mainStage = NewGO<MainStage>(0, "MainStage");
		break;

	case 1:
		m_player = NewGO<Player>(0, "Player");
		break;

	case 2:
		m_gameCamera = NewGO<GameCamera>(0, "GameCamera");
		break;

	case 3:
		/** 空の初期化 */
		m_skyCube = NewGO<SkyCube>(0, "SkyCube");
		m_skyCube->SetType(EnSkyCubeType::enSkyCubeType_Night);
		m_skyCube->SetScale(SKY_CUBE_SIZE);
		break;

	case 4:
	{
		/** 光の方向及び影の設定 */
		Vector3 sunDirection = LIGHT_DIRECTION;
		sunDirection.Normalize();
		g_renderingEngine->SetDirectionLight(0, sunDirection, LIGHT_COLOR);

		/** IBL 設定 */
		g_renderingEngine->SetAmbientByIBLTexture(m_skyCube->GetTextureFilePath(), IBL_STRENGTH);

		/** ブルームを抑制 */
		g_renderingEngine->SetBloomThreshold(BLOOM_STRENGTH);
		break;
	}

	case 5:
	{		
		/** プレートの生成 */
		NewGO<Plate>(0, "Plate");
		break;
	}

	case 6:
	{
		/** 段ボールの生成 */
		NewGO<DanBall>(0, "DanBall");
		break;
	}

	case 7:

		NewGO<Game>(0, "Game");

		/** ローディング完了 */
		DeleteGO(this);
		return;
		break;
	}

	/** ローディングの進捗状況を更新 */
	m_loadingProgress++;
}
