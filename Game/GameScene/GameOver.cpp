#include "stdafx.h"
#include "GameOver.h"
#include "GameScene/Title.h"
#include "LoadingScene.h"
#include "GameObjectCleaner/GameObjectCleaner.h"
#include "SoundManager/SoundManager.h"
#include "Fade.h"
namespace
{
	/** ゲームオーバー画面のファイルパス */
	constexpr const char* GAMEOVER_FILE_PATH = "Assets/sprite/GameOver/GameOver2.dds";
	/** ゲームオーバー画面の横幅 */
	constexpr float GAMEOVER_WIDTH = 1920.0f;
	/** ゲームオーバー画面の縦幅 */
	constexpr float GAMEOVER_HEIGHT = 1080.0f;

	/** リトライ文字の位置 */
	const Vector3 RETRY_FONT_POSITION = { -100.0f, 0.0f, 0.0f };

	/** タイトルに戻る文字の位置 */
	const Vector3 TITLE_FONT_POSITION = { -150.0f, -100.0f, 0.0f };

	/** ゲームを閉じる文字の位置 */
	const Vector3 EXIT_FONT_POSITION = { -150.0f, -200.0f, 0.0f };
}

GameOver::GameOver()
{}

GameOver::~GameOver()
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

bool GameOver::Start()
{
	/** すべてのゲームオブジェクトを削除 */
	GameObjectCleaner::DeleteAllGameObjects();

	/** ゲームオーバー画面を初期化 */
	m_gameOverSprite.Init(GAMEOVER_FILE_PATH, GAMEOVER_WIDTH, GAMEOVER_HEIGHT);

	/** ゲームオーバーBGMを再生 */
	SoundManager* sound = FindGO<SoundManager>("soundManager");
	m_gameOverBGM = sound->PlayingBGM(enGameOver, true);

	/** フォントの初期化 */
	m_retryFont.SetText(L"リトライ");
	m_retryFont.SetPosition(RETRY_FONT_POSITION);

	m_titleFont.SetText(L"タイトルに戻る");
	m_titleFont.SetPosition(TITLE_FONT_POSITION);

	m_exitFont.SetText(L"ゲームを閉じる");
	m_exitFont.SetPosition(EXIT_FONT_POSITION);

	/** 黒い画面から、ゲームオーバー画面を見せていく */
	FindGO<Fade>("Fade")->StartFadeIn(3.0f);

	return true;
}

void GameOver::Update()
{
	/** フェード中は入力を受け付けない */
	if (FindGO<Fade>("Fade")->IsBusy())
	{
		m_gameOverSprite.Update();
		return;
	}

	/** リトライ決定後は、SEの再生が終わるのを待つ */
	if (m_isRetrying)
	{
		UpdateRetryWait();
		return;
	}

	HandleInput();

	/** HandleInputでDeleteGO(this)された場合は以降の処理をしない */
	if (m_isRetrying || m_isClosing)
	{
		return;
	}

	HandleSelection();

	m_gameOverSprite.Update();
}

void GameOver::Render(RenderContext& rc)
{
	/** 現在選択しているインデックスによって描画するフォントの色を変える */
	m_retryFont.SetColor(m_selectedFontIndex == enGameOverRetry ? Vector4::White : Vector4::Gray);
	m_titleFont.SetColor(m_selectedFontIndex == enGameOverBackTitle ? Vector4::White : Vector4::Gray);
	m_exitFont.SetColor(m_selectedFontIndex == enGameOverExit ? Vector4::White : Vector4::Gray);

	m_gameOverSprite.Draw(rc);

	m_retryFont.Draw(rc);
	m_titleFont.Draw(rc);
	m_exitFont.Draw(rc);
}

void GameOver::HandleInput()
{
	if (!g_pad[0]->IsTrigger(enButtonA))
	{
		return;
	}

	switch (m_selectedFontIndex)
	{
	case enGameOverRetry:
	{
		DeleteGO(m_gameOverBGM);
		m_gameOverBGM = nullptr;

		SoundManager* sound = FindGO<SoundManager>("soundManager");
		m_retrySE = sound->PlayingSE(enStartGame, false);

		/** リトライが決定されたことを示すフラグを立てる */
		m_isRetrying = true;
		break;
	}

	case enGameOverBackTitle:
	{
		DeleteGO(m_gameOverBGM);
		m_gameOverBGM = nullptr;

		/** 決定時のSEを再生 */
		SoundManager* sound = FindGO<SoundManager>("soundManager");
		m_decideSE = sound->PlayingSE(enDecisionSE, false);

		/** このフレームの以降のUpdate処理を止める */
		m_isClosing = true;

		FindGO<Fade>("Fade")->StartTransition(Vector3(0.0f, 0.0f, 0.0f), 0.5f, 0.5f, [this]()
			{
				NewGO<Title>(0, "title");
				DeleteGO(this);
			});
		break;
	}
	case enGameOverExit:
		/** ゲームを閉じる処理 */
		PostQuitMessage(0);
		break;

	default:
		break;
	}
}

void GameOver::HandleSelection()
{
	/** 上下キーの入力に応じて選択中のフォントを切り替える */
	if (g_pad[0]->IsTrigger(enButtonUp))
	{
		m_selectedFontIndex--;

		SoundManager* sound = FindGO<SoundManager>("soundManager");
		m_selectionSE = sound->PlayingSE(enOutGameSelect, false);

		if (m_selectedFontIndex < enGameOverRetry)
		{
			m_selectedFontIndex = enGameOverExit;
		}
	}

	if (g_pad[0]->IsTrigger(enButtonDown))
	{
		m_selectedFontIndex++;

		SoundManager* sound = FindGO<SoundManager>("soundManager");
		m_selectionSE = sound->PlayingSE(enOutGameSelect, false);

		if (m_selectedFontIndex >= enGameOverMenuNum)
		{
			m_selectedFontIndex = enGameOverRetry;
		}
	}
}

void GameOver::UpdateRetryWait()
{
	/** SEがまだ再生中なら待つ */
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