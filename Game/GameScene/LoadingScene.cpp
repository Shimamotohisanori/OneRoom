#include "stdafx.h"
#include "LoadingScene.h"
#include "Source/Actor/Player/Player.h"
#include "Source/Actor/Stage/MainStage.h"
#include "GameCamera/GameCamera.h"
#include "Game.h"
#include "Source/Actor/Stage/PlateRoom/Plate/Plate.h"
#include "Source/Actor/Stage/PlateRoom/DanBall/DanBall.h"
#include "Source/Actor/Stage/Darkroom/Paper/Paper.h"
#include "Source/Actor/Stage/Darkroom/KeyPad/KeyPad.h"
#include "Key/Key.h"
#include "SAN/SANCalculation.h"
#include "Source/Actor/Stage/HandSetRoom/Phone/Phone.h"
#include "Source/Actor/Stage/HandSetRoom/PhonePaper/PhonePaper.h"
#include "Source/Actor/Stage/Door/PlateRoomDoor/PlateRoomDoor.h"
#include "Source/Actor/Stage/Door/HandSetRoomDoor/HandSetRoomDoor.h"
#include "Source/Actor/Stage/Door/DarkRoomDoor/DarkRoomDoor.h"
#include "Source/Actor/Stage/Door/GoalDoor/GoalDoor.h"
#include "Source/Actor/Stage/LightObject/LightObject.h"
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
	constexpr float IBL_STRENGTH = 0.001f;

	/** ブルームの強さ */
	constexpr float BLOOM_STRENGTH = 3.0f;

	/** ローディングの進捗状況の初期値 */
	constexpr int LOADING_PROGRESS_INITIAL = 0;

	/** 光の方向 */
	const Vector3 LIGHT_DIRECTION = { 0.0f, -1.0f, 0.0f };

	/** 光の色 */
	const Vector3 LIGHT_COLOR = { 0.001f, 0.001f, 0.001f };

	/** 受話器の数 */
	constexpr int PHONE_COUNT = 3;

	/** 正解の受話器の番号(0〜2) */
	constexpr int CORRECT_PHONE_INDEX = 2;

	/** 受話器の設置位置(ステージに合わせて調整) */
	const Vector3 PHONE_POSITIONS[PHONE_COUNT] =
	{
		Vector3(850.0f, 0.0f, 890.0f),
		Vector3(900.0f, -10.0f, 390.0f),
		Vector3(1200.0f,-90.0f, 600.0f),
	};

	/** 受話器の向き(ステージに合わせて調整) */
	const Vector3 PHONE_ROTATIONS[PHONE_COUNT] =
	{
		Vector3(0.0f, 180.0f, 0.0f),
		Vector3(0.0f, 0.0f, 0.0f),
		Vector3(0.0f, -90.0f, 0.0f),
	};

	/** 環境光の色 */
	const Vector3 AMBIENT_LIGHT_COLOR = { 0.001f, 0.001f, 0.001f };

	/** 天井のライトの高さ */
	constexpr float CEILING_LIGHT_HEIGHT = 120.0f;

	/** 天井のライトの色 */
	const Vector3 CEILING_LIGHT_COLOR = { 1.0f, 1.0f, 1.0f };

	/** 天井のライトの届く距離 */
	constexpr float CEILING_LIGHT_RANGE = 500.0f;

	/** 天井のライトの射出角度(度) */
	constexpr float CEILING_LIGHT_ANGLE = 45.0f;

	/** 天井のライト1つ分の設定 */
	struct CeilingLightSetting
	{
		/** ライトの種類 */
		LightObject::EnLightType type;

		/** ライトの位置(XとZ。高さは CEILING_LIGHT_HEIGHT) */
		float x;
		float z;

		/** 点滅の安定時間(秒)の範囲。Flicker のときだけ使う */
		float flickerMin;
		float flickerMax;
	};

	/** 天井のライトの一覧 */
	const CeilingLightSetting CEILING_LIGHT_SETTINGS[] =
	{
		/** 初期部屋 */
		{ LightObject::enLightType_AlwaysOn,   0.0f, -1400.0f,  0.0f,  0.0f },
		/** プレート部屋のドア前 */
		{ LightObject::enLightType_Flicker,  800.0f,  -450.0f,  1.5f,  5.0f },
		/** 暗室のドア前 */
		{ LightObject::enLightType_ModelOnly, -800.0f, -250.0f,  0.0f,  0.0f },
		/** 受話器部屋のドア前 */
		{ LightObject::enLightType_Flicker,  800.0f,   600.0f,  1.0f,  3.0f },
		/** 廊下(手前) */
		{ LightObject::enLightType_Flicker,   50.0f,  -450.0f, 10.0f, 30.0f },
		/** 廊下(中央) */
		{ LightObject::enLightType_ModelOnly,  50.0f,   150.0f,  0.0f,  0.0f },
		/** 廊下(奥) */
		{ LightObject::enLightType_Flicker,   50.0f,   800.0f,  0.5f,  1.5f },
	};

	/** ロードの手順。LoadGame() の switch の case に対応する */
	enum EnLoadStep
	{
		enLoadStep_MainStage,
		enLoadStep_Player,
		enLoadStep_GameCamera,
		enLoadStep_SkyCube,
		enLoadStep_LightSetting,
		enLoadStep_Plate,
		enLoadStep_Key,
		enLoadStep_SANCalculation,
		enLoadStep_Paper,
		enLoadStep_PhonePaper,
		enLoadStep_KeyPad,
		enLoadStep_DanBall,
		enLoadStep_Phone,
		enLoadStep_Door,
		enLoadStep_LightObject,
		enLoadStep_Game,
	};
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

void LoadingScene::Render(RenderContext& rc)
{
	m_loadingSprite.Draw(rc);
}

void LoadingScene::LoadGame()
{
	switch (m_loadingProgress)
	{
	case enLoadStep_MainStage:
		m_mainStage = NewGO<MainStage>(0, "MainStage");
		break;

	case enLoadStep_Player:
		m_player = NewGO<Player>(0, "Player");
		break;

	case enLoadStep_GameCamera:
		m_gameCamera = NewGO<GameCamera>(0, "GameCamera");
		break;

	case enLoadStep_SkyCube:
		/** 空の初期化 */
		m_skyCube = NewGO<SkyCube>(0, "SkyCube");
		m_skyCube->SetType(EnSkyCubeType::enSkyCubeType_Night);
		m_skyCube->SetScale(SKY_CUBE_SIZE);
		break;

	case enLoadStep_LightSetting:
	{
		/** 光の方向及び影の設定 */
		Vector3 sunDirection = LIGHT_DIRECTION;
		sunDirection.Normalize();
		g_renderingEngine->SetDirectionLight(0, sunDirection, LIGHT_COLOR);

		/** IBL 設定 */
		g_renderingEngine->SetAmbientByIBLTexture(m_skyCube->GetTextureFilePath(), IBL_STRENGTH);

		/** ブルームを抑制 */
		g_renderingEngine->SetBloomThreshold(BLOOM_STRENGTH);

		/** 環境光の設定 */
		g_sceneLight->SetAmbinet(AMBIENT_LIGHT_COLOR);
		break;
	}

	case enLoadStep_Plate:
	{
		/** プレートの生成 */
		NewGO<Plate>(0, "Plate");
		break;
	}

	case enLoadStep_Key:
	{
		/** キーの生成 */
		NewGO<Key>(0, "Key");
		break;
	}

	case enLoadStep_SANCalculation:
	{
		/** SAN計算クラスの生成 */
		NewGO<SANCalculation>(0, "SANCalculation");
		break;
	}

	case enLoadStep_Paper:
	{
		/** 紙の生成 */
		NewGO<Paper>(0, "Paper");
		break;
	}

	case enLoadStep_PhonePaper:
	{
		/** 電話の紙の生成 */
		NewGO<PhonePaper>(0, "PhonePaper");
		break;
	}

	case enLoadStep_KeyPad:
	{
		/** キーパッドの生成 */
		NewGO<KeyPad>(0, "KeyPad");
		break;
	}

	case enLoadStep_DanBall:
	{
		/** 段ボールの生成 */
		NewGO<DanBall>(0, "DanBall");
		break;
	}

	case enLoadStep_Phone:
	{
		/** 電話の生成(複数作る) */
		for (int i = 0; i < PHONE_COUNT; i++)
		{
			Phone* phone = NewGO<Phone>(0, "Phone");
			phone->SetPosition(PHONE_POSITIONS[i]);

			phone->SetRotation(PHONE_ROTATIONS[i]);

			/** 正解の番号と同じときだけ、正解の受話器にする */
			phone->SetIsCorrectPhone(i == CORRECT_PHONE_INDEX);
		}
		break;
	}

	case enLoadStep_Door:
	{
		/** ドアの生成 */
		NewGO<PlateRoomDoor>(0, "PlateRoomDoor");
		NewGO<HandSetRoomDoor>(0, "HandSetRoomDoor");
		NewGO<DarkRoomDoor>(0, "DarkRoomDoor");
		NewGO<GoalDoor>(0, "GoalDoor");
		break;
	}

	case enLoadStep_LightObject:
	{
		/** 天井のライトを一覧の設定どおりに生成する */
		/** 設定したライトの数分だけ生成する */
		for (const CeilingLightSetting& setting : CEILING_LIGHT_SETTINGS)
		{
			auto* light = NewGO<LightObject>(0, "LightObject");
			light->SetLightType(setting.type);
			light->SetLightPosition({ setting.x, CEILING_LIGHT_HEIGHT, setting.z });

			/** 光を出さないライトは、位置以外の設定が要らない */
			if (setting.type == LightObject::enLightType_ModelOnly) {
				continue;
			}

			light->SetLightColor(CEILING_LIGHT_COLOR);
			light->SetLightRange(CEILING_LIGHT_RANGE);
			light->SetLightAngle(CEILING_LIGHT_ANGLE);

			/** 点滅するライトだけ、点滅の間隔を設定する */
			if (setting.type == LightObject::enLightType_Flicker) {
				light->SetFlickerInterval(setting.flickerMin, setting.flickerMax);
			}
		}
		break;
	}

	case enLoadStep_Game:

		/** ゲームの生成 */
		NewGO<Game>(0, "Game");

		/** ローディング完了 */
		DeleteGO(this);
		return;
		break;
	}

	/** ローディングの進捗状況を更新 */
	m_loadingProgress++;
}