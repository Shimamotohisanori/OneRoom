#include "stdafx.h"
#include "KeyUI.h"

namespace
{
	/** キーUIスプライトのファイルパス */
	constexpr const char* KEY_UI_SPRITE_FILE_PATH = "Assets/sprite/Key/Key.dds";

	/** キーUIスプライトの横幅 */
	constexpr float KEY_UI_SPRITE_WIDTH = 200.0f;

	/** キーUIスプライトの縦幅 */
	constexpr float KEY_UI_SPRITE_HEIGHT = 200.0f;

	/** キーUIの座標 */
	const Vector3 KEY_UI_POSITION = Vector3(-870.0f, 500.0f, 0.0f);

	/** キーUIフォントの座標 */
	const Vector3 KEY_UI_FONT_POSITION = Vector3(-850.0f, 530.0f, 0.0f);

	/** キーUIフォントのサイズ */
	constexpr float KEY_UI_FONT_SIZE = 1.5f;
}

KeyUI::KeyUI()
{}

KeyUI::~KeyUI()
{}

bool KeyUI::Start()
{
	/** キーUIスプライトの初期化 */
	m_keyUISprite.Init(KEY_UI_SPRITE_FILE_PATH, KEY_UI_SPRITE_WIDTH, KEY_UI_SPRITE_HEIGHT);
	m_keyUISprite.SetPosition(KEY_UI_POSITION);
	return true;
}

void KeyUI::Update()
{
	/** キーUIフォントの更新 */
	wchar_t keyCountStr[256];
	swprintf_s(keyCountStr, L"%d / 3",m_key.GetKeyCount());
	m_keyUIFont.SetText(keyCountStr);
	m_keyUIFont.SetPosition(KEY_UI_FONT_POSITION);
	m_keyUIFont.SetScale(KEY_UI_FONT_SIZE);


	/** キーUIスプライトの更新 */
	m_keyUISprite.Update();
}

void KeyUI::Render(RenderContext & rc)
{
	m_keyUISprite.Draw(rc);
	m_keyUIFont.Draw(rc);
}
