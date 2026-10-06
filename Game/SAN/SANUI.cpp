#include "stdafx.h"
#include "SANUI.h"
#include "SANCalculation.h"
namespace
{
	/** SANUIのファイルパス */
	constexpr const char* SANUI_FILE_PATH = "Assets/sprite/SAN/SAN.dds";
	
	/** SANUIの横幅 */
	constexpr float SANUI_WIDTH = 300.0f;
	
	/** SANUIの縦幅 */
	constexpr float SANUI_HEIGHT = 200.0f;

	/** SANUIフォントのスケール */
	constexpr float SANUI_FONT_SCALE = 1.5f;

	/** 10 */
	constexpr int TEN = 10;

	/** 100 */
	constexpr int HUNDRED = 100;
	
	/** SANUIの座標 */
	const Vector3 SANUI_POSITION = { -800.0f, 400.0f, 0.0f };

	/** SANUIフォントの座標(1桁の時) */
	const Vector3 SANUI_FONT_POSITION_1DIGIT = { -810.0f, 460.0f, 0.0f };

	/** SANUIフォントの座標(2桁の時) */
	const Vector3 SANUI_FONT_POSITION = { -820.0f, 460.0f, 0.0f };

	/** SANUIフォントの座標(3桁の時) */
	const Vector3 SANUI_FONT_POSITION_3DIGIT = { -830.0f, 460.0f, 0.0f };
}

SANUI::~SANUI()
{}

bool SANUI::Start()
{
	/** SANUIスプライトの初期化 */
	m_sanUISprite.Init(SANUI_FILE_PATH, SANUI_WIDTH, SANUI_HEIGHT);
	m_sanUISprite.SetPosition(SANUI_POSITION);
	
	/** SANUIフォントの初期化 */
	m_sanCalculation = FindGO<SANCalculation>("SANCalculation");
	return true;
}

void SANUI::Update()
{

	/** SAN値を表示する */
	wchar_t text[256];
	swprintf_s(text, L"%d", m_sanCalculation->GetSANValue());
	m_sanUIFont.SetText(text);

	/** SANUIフォントの座標を更新 */
	/** SAN値の桁数によって座標を変更する */
	/** 1桁の時 */
	if (m_sanCalculation->GetSANValue() < TEN)
	{
		m_sanUIFont.SetPosition(SANUI_FONT_POSITION_1DIGIT);
	}
	/** 2桁の時 */
	else if (m_sanCalculation->GetSANValue() < HUNDRED)
	{
		m_sanUIFont.SetPosition(SANUI_FONT_POSITION);
	}
	/** 3桁の時 */
	else
	{
		m_sanUIFont.SetPosition(SANUI_FONT_POSITION_3DIGIT);
	}
	m_sanUIFont.SetPosition(SANUI_FONT_POSITION);
	m_sanUIFont.SetScale(SANUI_FONT_SCALE);

	/** SANUIスプライトを更新 */
	m_sanUISprite.Update();
}

void SANUI::Render(RenderContext & rc)
{
	m_sanUIFont.Draw(rc);
	m_sanUISprite.Draw(rc);
}
