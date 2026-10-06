#include "stdafx.h"
#include "KeyPad.h"
#include "Source/Actor/Player/Player.h"
#include "Key/Key.h"
namespace
{
	/** キーパッドモデルのファイルパス */
	const char* KEYPAD_MODEL_FILE_PATH = "Assets/modelData/KeyPad/KeyPad.tkm";

	/** キーパッドの画像ファイルパス */
	const char* KEYPAD_SPRITE_FILE_PATH = "Assets/sprite/KeyPad/KeyPad.dds";

	/** カーソルの画像ファイルパス */
	const char* CURSOR_SPRITE_FILE_PATH = "Assets/sprite/Cursor/Cursor.dds";
	
	/** 正解の四桁 */
	const std::wstring CORRECT_CODE = L"3951";

	/** キーパッドの初期座標 */
	const Vector3 KEYPAD_INITIAL_POSITION = Vector3(-330.0f, 10.0f, -200.0f);

	/** カーソルの初期座標 */
	const Vector3 CURSOR_INITIAL_POSITION = Vector3(0.0f, 0.0f, 0.0f);

	/** キーパッドに表示される数字の初期座標 */
	const Vector3 DIGITS_INITIAL_POSITION = Vector3(-290.0f, 300.0f, 0.0f);

	/** カーソルのスケール */
	const Vector3 CURSOR_SCALE = Vector3(1.0f, 1.0f, 1.0f);

	/** キーパッドのスケール */
	const Vector3 KEYPAD_SCALE = Vector3(3.0f, 3.0f, 3.0f);

	/** キーパッドの画像のスケール */
	const Vector3 KEYPAD_SPRITE_SCALE = Vector3(1.0f, 1.0f, 1.0f);

	/** キーパッドの画像の位置 */
	const Vector3 KEYPAD_SPRITE_POSITION = Vector3(0.0f, 0.0f, 0.0f);

	/** 案内文字の表示位置 */
	const Vector3 PROMPT_POSITION = Vector3(-150.0f, -200.0f, 0.0f);

	/** カーソルの色 */
	const Vector4 CURSOR_COLOR = Vector4(1.0f, 1.0f, 1.0f, 0.5f);

	/** キーパッド画像の縦幅 */
	constexpr float KEYPAD_SPRITE_HEIGHT = 800.0f;

	/** キーパッド画像の横幅 */
	constexpr float KEYPAD_SPRITE_WIDTH = 800.0f;

	/** カーソル画像の縦幅 */
	constexpr float CURSOR_SPRITE_HEIGHT = 60.0f;

	/** カーソル画像の横幅 */
	constexpr float CURSOR_SPRITE_WIDTH = 70.0f;

	/** インタラクトできる距離(モデルの大きさに合わせて調整) */
	constexpr float INTERACT_DISTANCE = 100.0f;

	/** 案内文字のスケール */
	constexpr float PROMPT_FONT_SCALE = 1.5f;

	/** 入力数字のスケール */
	constexpr float INPUT_FONT_SCALE = 1.25f;

	/** 入力できる最大桁数 */
	constexpr int MAX_DIGITS = 4;

	constexpr int NONE = -1;
	constexpr int KEY_COUNT = 11;
	constexpr int ENTER_INDEX = 10;
	constexpr int INITIAL_SELECT = 4; // 5のキーから開始

	// 座標は画像に合わせて調整してください
	const KeyButton KEY_BUTTONS[KEY_COUNT] =
	{
		//  種類              数字  座標(x,y)         上    下   左    右
		{ EnKeyButtonType::Number, 1, {-232.5f, 139.5f}, NONE,   3, NONE,   1 },
		{ EnKeyButtonType::Number, 2, {-95.0f,  139.5f}, NONE,   4,    0,    2 },
		{ EnKeyButtonType::Number, 3, { 40.0f,  139.5f}, NONE,   5,    1,   10 },
		{ EnKeyButtonType::Number, 4, {-232.5f,  10.0f},    0,   6, NONE,    4 },
		{ EnKeyButtonType::Number, 5, {-95.0f,   10.0f},    1,   7,    3,    5 },
		{ EnKeyButtonType::Number, 6, { 40.0f,   10.0f},    2,   8,    4,   10 },
		{ EnKeyButtonType::Number, 7, {-232.5f, -115.0f},    3, NONE, NONE,    7 },
		{ EnKeyButtonType::Number, 8, {-95.0f,  -115.0f},    4,   9,    6,    8 },
		{ EnKeyButtonType::Number, 9, { 40.0f,  -115.0f},    5, NONE,    7,   10 },
		{ EnKeyButtonType::Number, 0, { -95.0f, -235.0f},    7, NONE, NONE, NONE },
		{ EnKeyButtonType::Enter,  0, { 210.0f,   0.0f}, NONE, NONE,    5, NONE },
	};
}
KeyPad::KeyPad()
{}

KeyPad::~KeyPad()
{
	/** 開いたまま破棄されてもプレイヤーが動けなくならないようにする */
	if (m_isKeypadOpen && m_player != nullptr)
	{
		m_player->SetInputEnabled(true);
	}
}

bool KeyPad::Start()
{
	m_keyPadModel.Init(KEYPAD_MODEL_FILE_PATH);

	m_keyPadModel.SetPosition(KEYPAD_INITIAL_POSITION);
	m_keyPadModel.SetScale(KEYPAD_SCALE);

	m_keyPadSprite.Init(KEYPAD_SPRITE_FILE_PATH, KEYPAD_SPRITE_WIDTH, KEYPAD_SPRITE_HEIGHT);
	m_keyPadSprite.SetPosition(KEYPAD_SPRITE_POSITION);
	m_keyPadSprite.SetScale(KEYPAD_SPRITE_SCALE);

	m_cursorSprite.Init(CURSOR_SPRITE_FILE_PATH, CURSOR_SPRITE_WIDTH, CURSOR_SPRITE_HEIGHT);
	m_cursorSprite.SetPosition(CURSOR_INITIAL_POSITION);
	m_cursorSprite.SetScale(CURSOR_SCALE);

	/** プレイヤーの取得 */
	m_player = FindGO<Player>("Player");

	/** キーの取得 */
	m_key = FindGO<Key>("Key");

	/** 操作案内文字の初期化 */
	m_promptFont.SetPosition(PROMPT_POSITION);
	m_promptFont.SetScale(PROMPT_FONT_SCALE);
	m_promptFont.SetColor(Vector4::White);

	/** 入力数字の表示設定 */
	m_inputFont.SetPosition(DIGITS_INITIAL_POSITION);
	m_inputFont.SetScale(INPUT_FONT_SCALE);
	m_inputFont.SetColor(Vector4::Black);
	return true;
}

void KeyPad::Update()
{
	/** キーパッドを開いている間は、閉じる入力だけ受け付ける */
	if (m_isKeypadOpen)
	{
		/** Bボタンで閉じる */
		if (m_player->IsCancelTriggered())
		{
			Close();
			return;
		}

		/** 十字キーでカーソルを動かす */
		UpdateSelection();

		/** Aボタンで、選んでいるボタンを押す */
		if (m_player->IsInteractTriggered())
		{
			PressSelectedKey();
		}

		m_cursorSprite.SetPosition(Vector3
		(KEY_BUTTONS[m_selectedIndex].position.x,
			KEY_BUTTONS[m_selectedIndex].position.y,
			0.0f));
		m_cursorSprite.Update();
		m_keyPadSprite.Update();
		m_keyPadModel.Update();
		return;
	}

	/** プレイヤーが近くにいる場合は、操作案内文字を表示する */
	m_isPromptVisible = false;

	if (IsPlayerInRange())
	{
		m_promptFont.SetText(L"Press A to use keypad");
		m_isPromptVisible = true;

		if (m_player->IsInteractTriggered())
		{
			Interact();
		}
	}
	m_keyPadModel.Update();
}

void KeyPad::Render(RenderContext & rc)
{
	m_keyPadModel.Draw(rc);

	if (m_isKeypadOpen)
	{
		m_keyPadSprite.Draw(rc);
		m_inputFont.Draw(rc);
		m_cursorSprite.SetMulColor(CURSOR_COLOR);
		m_cursorSprite.Draw(rc);
		return;
	}

	if (m_isPromptVisible)
	{
		m_promptFont.Draw(rc);
	}
}

float KeyPad::CalculateDistanceToPlayer(const Vector3& playerPosition) const
{
	Vector3 diff = playerPosition - KEYPAD_INITIAL_POSITION;

	/** 高さは無視して水平距離だけで判定 */
	diff.y = 0.0f;
	return diff.Length();
}

bool KeyPad::IsPlayerInRange() const
{
	if (m_player == nullptr) return false;

	/** プレイヤーとの距離を計算して、インタラクト可能な距離かどうかを返す */
	return CalculateDistanceToPlayer(m_player->GetPosition()) <= INTERACT_DISTANCE;
}

void KeyPad::Interact()
{
	/** キーパッドを開く */
	m_isKeypadOpen = true;

	/** 操作案内文字を非表示にする */
	m_isPromptVisible = false;
	m_player->SetInputEnabled(false);
}

void KeyPad::Close()
{
	m_isKeypadOpen = false;
	m_player->SetInputEnabled(true);

	/** 入力した数字を消す */
	ResetInput();
}

void KeyPad::UpdateSelection()
{
	/** 今選んでいるボタンの情報 */
	const KeyButton& now = KEY_BUTTONS[m_selectedIndex];

	/** 移動先の番号、(-1なら動かない) */
	int next = NONE;

	/** 入力に応じて移動先を決定 */
	/** 上入力 */
	if (g_pad[0]->IsTrigger(enButtonUp))
	{
		next = now.up;

	}
	/** 下入力 */
	else if (g_pad[0]->IsTrigger(enButtonDown))
	{
		next = now.down;
	}
	/** 左入力 */
	else if (g_pad[0]->IsTrigger(enButtonLeft))
	{
		next = now.left;
	}
	/** 右入力 */
	else if (g_pad[0]->IsTrigger(enButtonRight))
	{
		next = now.right;
	}

	/** 移動先があるときだけ動く */
	if (next != NONE)
	{
		m_selectedIndex = next;
	}
}

void KeyPad::PressSelectedKey()
{
	const KeyButton& key = KEY_BUTTONS[m_selectedIndex];

	if (key.type == EnKeyButtonType::Number)
	{
		/** 数字ボタンなら、その数字を足す */
		InputDigit(key.number);
	}
	else
	{
		/** Enterボタン：正解かどうかの判定 */
		CheckCode();
	}
}

void KeyPad::InputDigit(int digit)
{
	/** 4桁までしか入力できない */
	if (m_inputText.size() >= MAX_DIGITS)
	{
		return;
	}

	/** 数字を文字に変えて、後ろにくっつける
	 *  L'0' に digit を足すと '0'～'9' の文字になる */
	m_inputText += static_cast<wchar_t>(L'0' + digit);

	/** 表示用フォントに反映する */
	m_inputFont.SetText(m_inputText.c_str());
}

void KeyPad::CheckCode()
{
	/** 入力した数字が正解してて
	 * まだ問題を解いていない場合 
	 */
	if (m_inputText == CORRECT_CODE && !m_isUnlock)
	{
		/** 正解 */
		m_isUnlock = true;
		m_key->AddKey();
		Close();
	}
	else
	{
		/** 不正解 */
		ResetInput();
	}
}

void KeyPad::ResetInput()
{
	m_inputText.clear();
	m_inputFont.SetText(L"");
}
