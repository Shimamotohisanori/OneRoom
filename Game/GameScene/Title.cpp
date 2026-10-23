#include "stdafx.h"
#include "Title.h"
#include "LoadingScene.h"
#include "SoundManager/SoundManager.h"
#include "VolumeAdjustment/VolumeAdjustment.h"
namespace
{
	/** タイトル画面のファイルパス */
	constexpr const char* TITLE_FILE_PATH = "Assets/sprite/Title/Title.dds";
	
	/** タイトル画面の横幅 */
	constexpr float TITLE_WIDTH = 1920.0f;
	
	/** タイトル画面の縦幅 */
	constexpr float TITLE_HEIGHT = 1080.0f;

	/** スタート文字の位置 */
	const Vector3 START_FONT_POSITION = { -150.0f, 0.0f, 0.0f };

	/** 音量調節文字の位置 */
	const Vector3 VOLUME_FONT_POSITION = { -100.0f, -100.0f, 0.0f };

	/** ゲームを閉じる文字の位置 */
	const Vector3 EXIT_FONT_POSITION = { -150.0f, -200.0f, 0.0f };
}

Title::Title()
{
	
}

Title::~Title()
{
	if (m_selectionSE != nullptr)
	{
		DeleteGO(m_selectionSE);
	}

	if (m_startSE != nullptr)
	{
		DeleteGO(m_startSE);
	}
}

bool Title::Start()
{
	/** タイトルスプライトの初期化 */
	m_titleSprite.Init(TITLE_FILE_PATH, TITLE_WIDTH, TITLE_HEIGHT);
	m_titleSprite.SetPosition(Vector3::Zero);

	/** サウンドマネージャーの取得 */
	SoundManager* sound = FindGO<SoundManager>("soundManager");
	m_titleBGM = sound->PlayingBGM(enTitle, true);

	/** フォントの初期化 */
	m_startFont.SetText(L"ゲームスタート");
	m_startFont.SetPosition(START_FONT_POSITION);

	m_volumeFont.SetText(L"音量調節");
	m_volumeFont.SetPosition(VOLUME_FONT_POSITION);

	m_exitFont.SetText(L"ゲームを閉じる");
	m_exitFont.SetPosition(EXIT_FONT_POSITION);
	return true;
}

void Title::Update()
{
	
	/** 音量調節画面が開いている間は操作を受け付けない */
	if (VolumeAdjustment::IsOpen())
	{
		return;
	}

	/** スタート決定後は、SEの再生が終わるのを待つ */
	if (m_isStarting)
	{
		UpdateStartWait();
		return;
	}

	HandleInput();

	HandleSelection();

	m_titleSprite.Update();
}

void Title::Render(RenderContext & rc)
{
	/** 現在選択しているインデックスによって描画するフォントの色を変える */
	switch(m_selectedFontIndex)
	{
	case TitleState::Start:
		m_startFont.SetColor(Vector4::White);
		m_volumeFont.SetColor(Vector4::Gray);
		m_exitFont.SetColor(Vector4::Gray);
		break;
	case TitleState::Volume:
		m_startFont.SetColor(Vector4::Gray);
		m_volumeFont.SetColor(Vector4::White);
		m_exitFont.SetColor(Vector4::Gray);
		break;
	case TitleState::Exit:
		m_startFont.SetColor(Vector4::Gray);
		m_volumeFont.SetColor(Vector4::Gray);
		m_exitFont.SetColor(Vector4::White);
		break;
	default:
		break;
	}

	m_titleSprite.Draw(rc);

	/** 音量調節画面が開いている間は、メニュー文字を隠す */
	if (VolumeAdjustment::IsOpen())
	{
		return;
	}

	m_startFont.Draw(rc);
	m_volumeFont.Draw(rc);
	m_exitFont.Draw(rc);
}

void Title::HandleInput()
{
	if (g_pad[0]->IsTrigger(enButtonA) && m_selectedFontIndex == TitleState::Start)
	{
		SoundManager* sound = FindGO<SoundManager>("soundManager");
		m_startSE = sound->PlayingSE(enStartGame, false);

		DeleteGO(m_titleBGM);

		/** ゲームスタートが決定されたことを示すフラグを立てる */
		m_isStarting = true;

		return;
	}

	if (g_pad[0]->IsTrigger(enButtonA) && m_selectedFontIndex == TitleState::Volume)
	{
		/** 音量調節画面を開く */
		VolumeAdjustment::Open();
	}

	if (g_pad[0]->IsTrigger(enButtonA) && m_selectedFontIndex == TitleState::Exit)
	{
		/** ゲームを閉じる処理 */
		PostQuitMessage(0);
	}
}

void Title::HandleSelection()
{
	/** 上下キーの入力に応じて選択中のフォントを切り替える */
	/** 選択中のフォントのインデックスを更新する */
	if (g_pad[0]->IsTrigger(enButtonUp))
	{
		m_selectedFontIndex--;

		SoundManager* sound = FindGO<SoundManager>("soundManager");
		m_selectionSE = sound->PlayingSE(enOutGameSelect, false);

		if (m_selectedFontIndex < TitleState::Start)
		{
			m_selectedFontIndex = TitleState::Exit;
		}
	}

	/** 選択中のフォントのインデックスを更新する */
	if (g_pad[0]->IsTrigger(enButtonDown))
	{
		m_selectedFontIndex++;

		SoundManager* sound = FindGO<SoundManager>("soundManager");
		m_selectionSE = sound->PlayingSE(enOutGameSelect, false);

		if (m_selectedFontIndex > TitleState::Exit)
		{
			m_selectedFontIndex = TitleState::Start;
		}

	}
}

void Title::UpdateStartWait()
{
	/** SEがまだ再生中なら待つ */
	if (m_startSE != nullptr && m_startSE->IsPlaying())
	{
		return;
	}

	/** 再生が終わったので、ローディング画面に遷移する */
	NewGO<LoadingScene>(0, "loading");
	DeleteGO(this);
}
