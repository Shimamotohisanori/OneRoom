#include "stdafx.h"
#include "Title.h"
#include "LoadingScene.h"
namespace
{
	/** タイトル画面のファイルパス */
	constexpr const char* TITLE_FILE_PATH = "Assets/sprite/Title/Title.dds";
	/** タイトル画面の横幅 */
	constexpr float TITLE_WIDTH = 1920.0f;
	/** タイトル画面の縦幅 */
	constexpr float TITLE_HEIGHT = 1080.0f;
}

Title::Title()
{
	
}

Title::~Title()
{}

bool Title::Start()
{
	/** タイトルスプライトの初期化 */
	m_titleSprite.Init(TITLE_FILE_PATH, TITLE_WIDTH, TITLE_HEIGHT);
	m_titleSprite.SetPosition(Vector3::Zero);
	return true;
}

void Title::Update()
{
	if (g_pad[0]->IsTrigger(enButtonA))
	{
		NewGO<LoadingScene>(0, "loading");

		DeleteGO(this);
		return;
	}

	m_titleSprite.Update();
}

void Title::Render(RenderContext & rc)
{
	m_titleSprite.Draw(rc);
}
