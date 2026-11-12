#include "stdafx.h"
#include "GameClear.h"
#include "GameScene/Title.h"
#include "LoadingScene.h"
#include "GameObjectCleaner/GameObjectCleaner.h"
#include "SoundManager/SoundManager.h"
#include "Fade.h"
namespace
{
	/** ゲームクリア画面のファイルパス */
	const char* GAMECLEAR_FILE_PATH = "Assets/sprite/GameClear/GameClear2.dds";

	/** ゲームクリア画面の横幅 */
	constexpr float GAMECLEAR_WIDTH = 1920.0f;

	/** ゲームクリア画面の縦幅 */
	constexpr float GAMECLEAR_HEIGHT = 1080.0f;

	/** リトライ文字の位置 */
	const Vector3 RETRY_FONT_POSITION = { -100.0f, 0.0f, 0.0f };

	/** タイトルに戻る文字の位置 */
	const Vector3 TITLE_FONT_POSITION = { -150.0f, -100.0f, 0.0f };

	/** ゲームを閉じる文字の位置 */
	const Vector3 EXIT_FONT_POSITION = { -150.0f, -200.0f, 0.0f };

	/** 選択中の文字の色(白い背景でも見えるオレンジ寄りの赤) */
	const Vector4 COLOR_SELECTED(0.9f, 0.3f, 0.0f, 1.0f);

	/** 非選択中の文字の色(濃いグレー) */
	const Vector4 COLOR_NORMAL(0.3f, 0.3f, 0.3f, 1.0f);
}

GameClear::GameClear()
{}

GameClear::~GameClear()
{
	if (m_selectionSE != nullptr)
	{
		DeleteGO(m_selectionSE);
	}

	if (m_retrySE != nullptr)
	{
		DeleteGO(m_retrySE);
	}
}

bool GameClear::Start()
{
	/** すべてのゲームオブジェクトを削除 */
	GameObjectCleaner::DeleteAllGameObjects();

	/** ゲームクリア画像を初期化 */
	m_clearSprite.Init(
		GAMECLEAR_FILE_PATH,
		GAMECLEAR_WIDTH,
		GAMECLEAR_HEIGHT
	);

	/** ゲームクリアBGMを再生 */
	SoundManager* sound = FindGO<SoundManager>("soundManager");
	m_clearBGM = sound->PlayingBGM(enGameClear, true);

	/** フォントの初期化 */
	m_retryFont.SetText(L"リトライ");
	m_retryFont.SetPosition(RETRY_FONT_POSITION);

	m_titleFont.SetText(L"タイトルに戻る");
	m_titleFont.SetPosition(TITLE_FONT_POSITION);

	m_exitFont.SetText(L"ゲームを閉じる");
	m_exitFont.SetPosition(EXIT_FONT_POSITION);

	/** 白い画面から、クリア画面を見せていく */
	FindGO<Fade>("Fade")->StartFadeIn(3.0f);
	return true;
}

void GameClear::Update()
{
	/** フェード中は入力を受け付けない */
	if (FindGO<Fade>("Fade")->IsBusy())
	{
		m_clearSprite.Update();
		return;
	}

	/** リトライ決定後は、SEの再生が終わるのを待つ */
	if (m_isRetrying)
	{
		UpdateRetryWait();
		return;
	}

	HandleInput();

	/** HandleInputでリトライ決定やDeleteGO(this)された場合は以降の処理をしない */
	if (m_isRetrying || m_isClosing)
	{
		return;
	}

	HandleSelection();

	m_clearSprite.Update();
}

void GameClear::Render(RenderContext& rc)
{
	/** 現在選択しているインデックスによって描画するフォントの色を変える */
	m_retryFont.SetColor(m_selectedFontIndex == enGameClearRetry ? COLOR_SELECTED : COLOR_NORMAL);
	m_titleFont.SetColor(m_selectedFontIndex == enGameClearBackTitle ? COLOR_SELECTED : COLOR_NORMAL);
	m_exitFont.SetColor(m_selectedFontIndex == enGameClearExit ? COLOR_SELECTED : COLOR_NORMAL);

	m_clearSprite.Draw(rc);

	m_retryFont.Draw(rc);
	m_titleFont.Draw(rc);
	m_exitFont.Draw(rc);
}

void GameClear::HandleInput()
{
	if (!g_pad[0]->IsTrigger(enButtonA))
	{
		return;
	}

	switch (m_selectedFontIndex)
	{
	case enGameClearRetry:
	{
		DeleteGO(m_clearBGM);
		m_clearBGM = nullptr;

		SoundManager* sound = FindGO<SoundManager>("soundManager");
		m_retrySE = sound->PlayingSE(enStartGame, false);

		/** リトライが決定されたことを示すフラグを立てる */
		m_isRetrying = true;
		break;
	}
	case enGameClearBackTitle:
	{
		DeleteGO(m_clearBGM);
		m_clearBGM = nullptr;

		SoundManager* sound = FindGO<SoundManager>("soundManager");
		m_decideSE = sound->PlayingSE(enDecisionSE, false);

		/** 暗転中に、毎フレーム入力処理が走らないようにする */
		m_isClosing = true;

		FindGO<Fade>("Fade")->StartTransition(Vector3(0.0f, 0.0f, 0.0f), 0.5f, 0.5f, [this]()
			{
				NewGO<Title>(0, "title");
				DeleteGO(this);
			});

		break;
	}
	case enGameClearExit:

		/** ゲームを閉じる処理 */
		PostQuitMessage(0);
		break;

	default:
		break;
	}
}

void GameClear::HandleSelection()
{
	/** 上下キーの入力に応じて選択中のフォントを切り替える */
	if (g_pad[0]->IsTrigger(enButtonUp))
	{
		m_selectedFontIndex--;

		SoundManager* sound = FindGO<SoundManager>("soundManager");
		m_selectionSE = sound->PlayingSE(enOutGameSelect, false);

		if (m_selectedFontIndex < enGameClearRetry)
		{
			m_selectedFontIndex = enGameClearExit;
		}
	}

	if (g_pad[0]->IsTrigger(enButtonDown))
	{
		m_selectedFontIndex++;

		SoundManager* sound = FindGO<SoundManager>("soundManager");
		m_selectionSE = sound->PlayingSE(enOutGameSelect, false);

		if (m_selectedFontIndex >= enGameClearMenuNum)
		{
			m_selectedFontIndex = enGameClearRetry;
		}
	}
}

void GameClear::UpdateRetryWait()
{
	if (m_retrySE != nullptr && m_retrySE->IsPlaying())
	{
		return;
	}

	/** すでに暗転が始まっていたら、何もしない(毎フレーム呼ばれるため) */
	Fade* fade = FindGO<Fade>("Fade");
	if (fade->IsBusy()) { return; }

	fade->StartTransition(Vector3(0.0f, 0.0f, 0.0f), 0.5f, 0.5f, [this]()
		{
			NewGO<LoadingScene>(0, "loading");
			DeleteGO(this);
		});
}