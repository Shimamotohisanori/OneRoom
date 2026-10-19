#include "stdafx.h"
#include "Pause.h"
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
#include "Game.h"
#include "Source/Actor/Stage/Door/PlateRoomDoor/PlateRoomDoor.h"
#include "Source/Actor/Stage/Door/DarkRoomDoor/DarkRoomDoor.h"
#include "Source/Actor/Stage/Door/HandSetRoomDoor/HandSetRoomDoor.h"

#include "VolumeAdjustment/VolumeAdjustment.h"
namespace
{
	/** 一時停止画面のファイルパス */
	constexpr const char* PAUSE_FILE_PATH = "Assets/sprite/Pause/Pause.dds";

	/** 一時停止画面の横幅 */
	constexpr float PAUSE_WIDTH = 1920.0f;

	/** 一時停止画面の縦幅 */
	constexpr float PAUSE_HEIGHT = 1080.0f;

	/** ゲームに戻る文字の位置 */
	const Vector3 RESUME_FONT_POSITION = { -135.0f, 100.0f, 0.0f };

	/** タイトルに戻る文字の位置 */
	const Vector3 TITLE_FONT_POSITION = { -150.0f, 0.0f, 0.0f };

	/** 音量調節文字の位置 */
	const Vector3 VOLUME_FONT_POSITION = { -100.0f, -100.0f, 0.0f };

	/** ゲームを閉じる文字の位置 */
	const Vector3 EXIT_FONT_POSITION = { -150.0f, -200.0f, 0.0f };
}

Pause::Pause()
{}

Pause::~Pause()
{}

bool Pause::Start()
{
	/** 一時停止画面のスプライトの初期化 */
	m_pauseSprite.Init(PAUSE_FILE_PATH, PAUSE_WIDTH, PAUSE_HEIGHT);

	/** フォントの初期化 */
	m_resumeFont.SetText(L"ゲームに戻る");
	m_resumeFont.SetPosition(RESUME_FONT_POSITION);

	m_titleFont.SetText(L"タイトルに戻る");
	m_titleFont.SetPosition(TITLE_FONT_POSITION);

	m_volumeFont.SetText(L"音量調節");
	m_volumeFont.SetPosition(VOLUME_FONT_POSITION);

	m_exitFont.SetText(L"ゲームを閉じる");
	m_exitFont.SetPosition(EXIT_FONT_POSITION);

	m_player = FindGO<Player>("Player");

	return true;
}

void Pause::Update()
{
	/** ポーズ中じゃない場合は処理を終了 */
	if (m_isPaused == false)
	{
		return;
	}

	/** 音量調節画面が開いている間は操作を受け付けない */
	if (VolumeAdjustment::IsOpen())
	{
		return;
	}

	HandleInput();

	HandleSelection();

	UpdateFontColor();

	m_pauseSprite.Update();
}

void Pause::Render(RenderContext& rc)
{
	/** ポーズ中じゃない場合は処理を終了 */
	if (m_isPaused == false)
	{
		return;
	}

	m_pauseSprite.Draw(rc);
	
	if (VolumeAdjustment::IsOpen())
	{
		return;
	
	}
	m_resumeFont.Draw(rc);
	m_titleFont.Draw(rc);
	m_volumeFont.Draw(rc);
	m_exitFont.Draw(rc);
}

void Pause::HandleInput()
{
	if (!g_pad[0]->IsTrigger(enButtonA))
	{
		return;
	}

	switch (m_selectedFontIndex)
	{
	case PauseState::PauseResume:
		/** ゲームに戻る処理 */
		m_isPaused = false;
		/** プレイヤーの操作を有効化 */
		m_player->SetInputEnabled(true);
		break;

	case PauseState::PauseTitle:
		/** タイトルに戻る処理 */
		DeleteAllGameObjects();
		NewGO<Title>(0, "title");
		break;

	case PauseState::PauseVolume:
		/** 音量調節画面に遷移する処理 */
		VolumeAdjustment::Open();
		break;

	case PauseState::PauseExit:
		/** ゲームを閉じる処理 */
		PostQuitMessage(0);
		break;

	default:
		break;
	}
}

void Pause::HandleSelection()
{
	/** 上キーで選択中のインデックスを1つ前に移動（先頭からは末尾へ） */
	if (g_pad[0]->IsTrigger(enButtonUp))
	{
		m_selectedFontIndex--;
		if (m_selectedFontIndex < PauseState::PauseResume)
		{
			m_selectedFontIndex = PauseState::PauseExit;
		}
	}

	/** 下キーで選択中のインデックスを1つ後ろに移動（末尾からは先頭へ） */
	if (g_pad[0]->IsTrigger(enButtonDown))
	{
		m_selectedFontIndex++;
		if (m_selectedFontIndex > PauseState::PauseExit)
		{
			m_selectedFontIndex = PauseState::PauseResume;
		}
	}
}

void Pause::UpdateFontColor()
{
	/** 選択中のフォントだけ白、それ以外はグレーにする */
	m_resumeFont.SetColor(m_selectedFontIndex == PauseState::PauseResume ? Vector4::White : Vector4::Gray);
	m_titleFont.SetColor(m_selectedFontIndex == PauseState::PauseTitle ? Vector4::White : Vector4::Gray);
	m_volumeFont.SetColor(m_selectedFontIndex == PauseState::PauseVolume ? Vector4::White : Vector4::Gray);
	m_exitFont.SetColor(m_selectedFontIndex == PauseState::PauseExit ? Vector4::White : Vector4::Gray);
}

void Pause::DeleteAllGameObjects()
{
	/** メインステージを削除 */
	DeleteGO(FindGO<MainStage>("MainStage"));

	/** プレイヤーを削除 */
	DeleteGO(FindGO<Player>("Player"));

	/** ゲームカメラを削除 */
	DeleteGO(FindGO<GameCamera>("GameCamera"));

	/** 空を削除 */
	DeleteGO(FindGO<SkyCube>("SkyCube"));

	/** SAN UIを削除 */
	DeleteGO(FindGO<SANUI>("SanUI"));

	/** プレートを削除 */
	DeleteGO(FindGO<Plate>("Plate"));

	/** 段ボールを削除 */
	DeleteGO(FindGO<DanBall>("DanBall"));

	/** 操作紹介UIを削除 */
	DeleteGO(FindGO<IntroductionUI>("IntroductionUI"));

	/** キーを削除 */
	DeleteGO(FindGO<Key>("Key"));

	/** キーUIを削除 */
	DeleteGO(FindGO<KeyUI>("KeyUI"));

	/** SAN計算クラスを削除 */
	DeleteGO(FindGO<SANCalculation>("SANCalculation"));

	/** 紙を削除 */
	DeleteGO(FindGO<Paper>("Paper"));

	/** キーパッドを削除 */
	DeleteGO(FindGO<KeyPad>("KeyPad"));

	/** 電話をすべて削除 */
	QueryGOs<Phone>("Phone", [](Phone* phone) -> bool
		{
			DeleteGO(phone);
			return true;
		});

	/** 電話の紙を削除 */
	DeleteGO(FindGO<PhonePaper>("PhonePaper"));

	/** プレートルームのドアを削除 */
	DeleteGO(FindGO<PlateRoomDoor>("PlateRoomDoor"));

	/** ダークルームのドアを削除 */
	DeleteGO(FindGO<DarkRoomDoor>("DarkRoomDoor"));

	/** ハンドセットルームのドアを削除 */
	DeleteGO(FindGO<HandSetRoomDoor>("HandSetRoomDoor"));

	/** ゲームを消去 */
	DeleteGO(FindGO<Game>("Game"));
}
