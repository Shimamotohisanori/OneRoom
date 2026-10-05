#include "stdafx.h"
#include "IntroductionUI.h"

namespace
{
	/** 操作紹介UIスプライトのファイルパス */
	const char* INTRODUCTION_UI_SPRITE_FILE_PATH = "Assets/sprite/Introduction/Introduction.dds";

	/** 操作紹介UIスプライトの横幅 */
	constexpr float INTRODUCTION_UI_SPRITE_WIDTH = 500.0f;

	/** 操作紹介UIスプライトの縦幅 */
	constexpr float INTRODUCTION_UI_SPRITE_HEIGHT = 500.0f;

	/** 操作紹介UIの座標 */
	const Vector3 INTRODUCTION_UI_POSITION = Vector3(750.0f, -400.0f, 0.0f);
}

IntroductionUI::IntroductionUI()
{}

IntroductionUI::~IntroductionUI()
{}

bool IntroductionUI::Start()
{
	/** 初期化処理 */
	m_introductionUISprite.Init(INTRODUCTION_UI_SPRITE_FILE_PATH, INTRODUCTION_UI_SPRITE_WIDTH, INTRODUCTION_UI_SPRITE_HEIGHT);

	m_introductionUISprite.SetPosition(INTRODUCTION_UI_POSITION);
	return true;
}

void IntroductionUI::Update()
{
	m_introductionUISprite.Update();
}

void IntroductionUI::Render(RenderContext & rc)
{
	m_introductionUISprite.Draw(rc);
}
