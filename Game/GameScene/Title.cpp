#include "stdafx.h"
#include "Title.h"
#include "LoadingScene.h"
#include "SoundManager/SoundManager.h"
#include "VolumeAdjustment/VolumeAdjustment.h"
#include "Fade.h"
#include <random>
namespace
{
	/** (普通の)タイトル画面のファイルパス */
	constexpr const char* TITLE_FILE_PATH = "Assets/sprite/Title/Title.dds";
	
	/** (少しくらい)タイトル画面のファイルパス */
	constexpr const char* LITLE_DARK_TITLE_FILE_PATH = "Assets/sprite/Title/LitleDarkTitle.dds";

	/** (かなり暗い)タイトル画面のファイルパス */
	constexpr const char* DARK_TITLE_FILE_PATH = "Assets/sprite/Title/DarkTitle.dds";

	/** タイトル画面の横幅 */
	constexpr float TITLE_WIDTH = 1920.0f;
	
	/** タイトル画面の縦幅 */
	constexpr float TITLE_HEIGHT = 1080.0f;

	/** 放置してから点滅を始めるまでの時間(秒) */
	constexpr float IDLE_START_TIME = 10.0f;

	/** 通常画像を保つ時間の範囲(秒) */
	constexpr float NORMAL_HOLD_MIN = 0.5f;
	constexpr float NORMAL_HOLD_MAX = 3.0f;

	/** (少し暗い)画像を保つ時間の範囲(秒) */
	constexpr float LITLE_DARK_HOLD_MIN = 0.05f;
	constexpr float LITLE_DARK_HOLD_MAX = 0.4f;

	/** (かなり暗い)画像を保つ時間の範囲(秒) */
	constexpr float DARK_HOLD_MIN = 0.03f;
	constexpr float DARK_HOLD_MAX = 0.25f;

	/** 次の状態として「かなり暗い」が選ばれる確率(0.0〜1.0) */
	constexpr float DARK_PROBABILITY = 0.4f;


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

	/** (少しくらい)タイトルスプライトの初期化 */
	m_litleDarkTitleSprite.Init(LITLE_DARK_TITLE_FILE_PATH, TITLE_WIDTH, TITLE_HEIGHT);
	m_litleDarkTitleSprite.SetPosition(Vector3::Zero);

	/** (かなり暗い)タイトルスプライトの初期化 */
	m_darkTitleSprite.Init(DARK_TITLE_FILE_PATH, TITLE_WIDTH, TITLE_HEIGHT);
	m_darkTitleSprite.SetPosition(Vector3::Zero);

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
	/** フェード中は入力を受け付けない */
	if (FindGO<Fade>("Fade")->IsBusy())
	{
		UpdateSprites();
		return;
	}

	/** 音量調節画面が開いている間は操作を受け付けない */
	if (VolumeAdjustment::IsOpen())
	{
		/** 操作中扱いにして通常画像に戻す */
		m_idleTimer = 0.0f;
		m_flickerTimer = 0.0f;
		m_currentImage = enTitleImage_Normal;
		return;
	}

	/** スタート決定後は、SEの再生が終わるのを待つ(暗いタイトルのまま) */
	if (m_isStarting)
	{
		UpdateStartWait();
		UpdateSprites();
		return;
	}

	HandleInput();

	/** HandleInputでスタートが決まった場合は点滅処理をしない */
	if (!m_isStarting)
	{
		HandleSelection();
		UpdateIdleFlicker();
	}

	UpdateSprites();
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

	/** 現在表示するタイトル画像を描画 */
	GetCurrentSprite().Draw(rc);

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
		/** ゲームスタートが決定されたことを示すフラグを立てる */
		m_isStarting = true;

		/** 真っ暗なタイトル画面にする */
		m_currentImage = enTitleImage_Dark;

		SoundManager* sound = FindGO<SoundManager>("soundManager");
		m_startSE = sound->PlayingSE(enStartGame, false);

		DeleteGO(m_titleBGM);

		/** ゲームスタートが決定されたことを示すフラグを立てる */
		m_isStarting = true;

		return;
	}

	if (g_pad[0]->IsTrigger(enButtonA) && m_selectedFontIndex == TitleState::Volume)
	{
		/** 音量調節画面を開く前に、決定SEを鳴らす */
		SoundManager* sound = FindGO<SoundManager>("soundManager");
		m_decisionSE = sound->PlayingSE(enDecisionSE, false);

		/** 音量調節画面を開く */
		VolumeAdjustment::Open();
	}

	if (g_pad[0]->IsTrigger(enButtonA) && m_selectedFontIndex == TitleState::Exit)
	{
		/** 決定SEを鳴らす */
		SoundManager* sound = FindGO<SoundManager>("soundManager");
		m_decisionSE = sound->PlayingSE(enDecisionSE, false);

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

	/** すでに暗転が始まっていたら何もしない(毎フレーム呼ばれるため) */
	Fade* fade = FindGO<Fade>("Fade");
	if (fade->IsBusy()) { return; }

	/** 暗転して、真っ暗になった瞬間にロード画面へ切り替え、明転する */
	fade->StartTransition(Vector3(0.0f, 0.0f, 0.0f), 0.5f, 0.5f, [this]()
		{
			NewGO<LoadingScene>(0, "loading");
			DeleteGO(this);
		});
}

void Title::UpdateIdleFlicker()
{
	/**入力があれば放置状態を解除して通常画像に戻す */
		if (IsAnyInput())
		{
			m_idleTimer = 0.0f;
			m_flickerTimer = 0.0f;
			m_flickerHoldTime = 0.0f;
			m_currentImage = enTitleImage_Normal;
			return;
		}

	const float deltaTime = g_gameTime->GetFrameDeltaTime();
	m_idleTimer += deltaTime;

	/** 放置時間が足りないうちは通常画像 */
	if (m_idleTimer < IDLE_START_TIME)
	{
		m_currentImage = enTitleImage_Normal;
		return;
	}

	/** 今の状態を保つ時間が残っているなら、そのまま待つ */
	m_flickerTimer += deltaTime;
	if (m_flickerTimer < m_flickerHoldTime)
	{
		return;
	}

	/** 保持時間が切れたので、次の状態と保持時間を抽選する */
	m_flickerTimer = 0.0f;

	if (m_currentImage == enTitleImage_Normal)
	{
		/** 通常からは、暗い画像のどちらかへ移る */
		if (RandomRange(0.0f, 1.0f) < DARK_PROBABILITY)
		{
			m_currentImage = enTitleImage_Dark;
			m_flickerHoldTime = RandomRange(DARK_HOLD_MIN, DARK_HOLD_MAX);
		}
		else
		{
			m_currentImage = enTitleImage_LitleDark;
			m_flickerHoldTime = RandomRange(LITLE_DARK_HOLD_MIN, LITLE_DARK_HOLD_MAX);
		}
	}
	else
	{
		/** 暗い状態からは、通常画像へ戻る */
		m_currentImage = enTitleImage_Normal;
		m_flickerHoldTime = RandomRange(NORMAL_HOLD_MIN, NORMAL_HOLD_MAX);
	}
}

bool Title::IsAnyInput() const
{
	/** 何か入力があったかを判定する */
	return g_pad[0]->IsTrigger(enButtonA)
		|| g_pad[0]->IsTrigger(enButtonB)
		|| g_pad[0]->IsTrigger(enButtonUp)
		|| g_pad[0]->IsTrigger(enButtonDown)
		|| g_pad[0]->IsTrigger(enButtonStart);
}

void Title::UpdateSprites()
{
	m_titleSprite.Update();
	m_litleDarkTitleSprite.Update();
	m_darkTitleSprite.Update();
}

SpriteRender& Title::GetCurrentSprite()
{
	/** 現在表示するタイトル画像を取得する */
	switch (m_currentImage)
	{
	case enTitleImage_LitleDark:
		return m_litleDarkTitleSprite;
	case enTitleImage_Dark:
		return m_darkTitleSprite;
	default:
		return m_titleSprite;
	}
}

float Title::RandomRange(float min, float max) const
{
	static std::mt19937 engine{ std::random_device{}() };
	std::uniform_real_distribution<float> dist(min, max);
	return dist(engine);
}
