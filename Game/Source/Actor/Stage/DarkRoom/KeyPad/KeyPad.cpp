#include "stdafx.h"
#include "KeyPad.h"
#include "Source/Actor/Player/Player.h"
#include "Key/Key.h"
#include "SoundManager/SoundManager.h"
namespace
{
	/** キーパッドモデルのファイルパス */
	const char* KEYPAD_MODEL_FILE_PATH = "Assets/modelData/KeyPad/KeyPad.tkm";

	/** キーパッドの画像ファイルパス */
	const char* KEYPAD_SPRITE_FILE_PATH = "Assets/sprite/KeyPad/KeyPad.dds";

	/** カーソルの画像ファイルパス */
	const char* CURSOR_SPRITE_FILE_PATH = "Assets/sprite/Cursor/Cursor.dds";

	/** 正解の7桁(紙に書かれている番号) */
	const std::wstring CORRECT_CODE = L"3951784";

	/** キーパッドの初期座標 */
	const Vector3 KEYPAD_INITIAL_POSITION = Vector3(-330.0f, 10.0f, -200.0f);

	/** カーソルの初期座標 */
	const Vector3 CURSOR_INITIAL_POSITION = Vector3(0.0f, 0.0f, 0.0f);

	/** キーパッドに表示される数字の初期座標 */
	const Vector3 DIGITS_INITIAL_POSITION = Vector3(-290.0f, 300.0f, 0.0f);

	/** カーソルのスケール */
	const Vector3 CURSOR_SCALE = Vector3(1.0f, 1.0f, 1.0f);

	/** キーパッドのスケール */
	const Vector3 KEYPAD_SCALE = Vector3(3.0f, 5.0f, 5.0f);

	/** キーパッドの画像のスケール */
	const Vector3 KEYPAD_SPRITE_SCALE = Vector3(1.0f, 1.0f, 1.0f);

	/** キーパッドの画像の位置 */
	const Vector3 KEYPAD_SPRITE_POSITION = Vector3(0.0f, 0.0f, 0.0f);

	/** 案内文字の表示位置 */
	const Vector3 PROMPT_POSITION = Vector3(-130.0f, -100.0f, 0.0f);

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

	/** キーパッドの正面方向(キーパッドからプレイヤーが立つ側へ向かう向き)。
	 *  壁にキーパッドが付いているなら、壁の表面に垂直で、部屋の内側を向く方向にする。
	 *  壁の裏側(この向きと反対側)からは、どれだけ近くてもインタラクトできない */
	const Vector3 KEYPAD_FRONT_DIRECTION = Vector3(-1.0f, 0.0f, 0.0f);

	/** 正面方向に、どこまで離れてもインタラクトできるか(伸ばしたいのはこの値) */
	constexpr float INTERACT_MAX_DEPTH = 100.0f;

	/** 正面方向の最小値。0より小さくすると、キーパッドの原点より奥(壁側)でも反応する */
	constexpr float INTERACT_MIN_DEPTH = 0.0f;

	/** 正面方向と垂直な向き(横方向)に、中心からどこまでずれてもインタラクトできるか */
	constexpr float INTERACT_HALF_WIDTH = 50.0f;

	/** 案内文字のスケール */
	constexpr float PROMPT_FONT_SCALE = 1.5f;

	/** 入力数字のスケール */
	constexpr float INPUT_FONT_SCALE = 1.5f;

	/** 入力できる最大桁数(正解の桁数に合わせる) */
	const int MAX_DIGITS = static_cast<int>(CORRECT_CODE.size());

	constexpr int NONE = -1;
	constexpr int KEY_COUNT = 11;
	constexpr int ENTER_INDEX = 10;
	constexpr int INITIAL_SELECT = 4; // 5のキーから開始

	// 座標は画像に合わせて調整してください
	const KeyButton KEY_BUTTONS[KEY_COUNT] =
	{
		//  種類              数字  座標(x,y)         上    下   左    右
		{ EnKeyButtonType::Number, 1,{ -232.5f, 139.5f }, NONE,   3, NONE,   1 },
		{ EnKeyButtonType::Number, 2,{ -95.0f,  139.5f }, NONE,   4,    0,    2 },
		{ EnKeyButtonType::Number, 3,{ 40.0f,  139.5f }, NONE,   5,    1,   10 },
		{ EnKeyButtonType::Number, 4,{ -232.5f,  10.0f },    0,   6, NONE,    4 },
		{ EnKeyButtonType::Number, 5,{ -95.0f,   10.0f },    1,   7,    3,    5 },
		{ EnKeyButtonType::Number, 6,{ 40.0f,   10.0f },    2,   8,    4,   10 },
		{ EnKeyButtonType::Number, 7,{ -232.5f, -115.0f },    3, NONE, NONE,    7 },
		{ EnKeyButtonType::Number, 8,{ -95.0f,  -115.0f },    4,   9,    6,    8 },
		{ EnKeyButtonType::Number, 9,{ 40.0f,  -115.0f },    5, NONE,    7,   10 },
		{ EnKeyButtonType::Number, 0,{ -95.0f, -235.0f },    7, NONE, NONE, NONE },
		{ EnKeyButtonType::Enter,  0,{ 210.0f,   0.0f }, NONE, NONE,    5, NONE },
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

	if (m_selectSE != nullptr)
	{
		DeleteGO(m_selectSE);
	}

	if (m_decisionSE != nullptr)
	{
		DeleteGO(m_decisionSE);
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
		m_promptFont.SetText(L"キーパッドを使う");
		m_isPromptVisible = true;

		if (m_player->IsInteractTriggered())
		{
			Interact();
		}
	}
	m_keyPadModel.Update();
}

void KeyPad::Render(RenderContext& rc)
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

bool KeyPad::IsPlayerInRange() const
{
	if (m_player == nullptr) return false;

	/** キーパッドからプレイヤーへのベクトル(高さは無視して水平だけで判定) */
	Vector3 diff = m_player->GetPosition() - KEYPAD_INITIAL_POSITION;
	diff.y = 0.0f;

	/** キーパッドの正面方向(水平) */
	Vector3 front = KEYPAD_FRONT_DIRECTION;
	front.y = 0.0f;
	front.Normalize();

	/** 正面方向の距離。マイナスなら壁の裏側にいるので反応させない */
	const float depth = diff.Dot(front);
	if (depth < INTERACT_MIN_DEPTH || depth > INTERACT_MAX_DEPTH)
	{
		return false;
	}

	/** 正面方向と垂直な向き(横方向)のずれが、範囲内かどうか */
	const Vector3 side = diff - front * depth;
	return side.Length() <= INTERACT_HALF_WIDTH;
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

	SoundManager* sound = FindGO<SoundManager>("soundManager");

	/** 入力に応じて移動先を決定 */
	/** 上入力 */
	if (g_pad[0]->IsTrigger(enButtonUp))
	{
		next = now.up;
		m_selectSE = sound->PlayingSE(enPadSelect, false);
	}
	/** 下入力 */
	else if (g_pad[0]->IsTrigger(enButtonDown))
	{
		next = now.down;
		m_selectSE = sound->PlayingSE(enPadSelect, false);
	}
	/** 左入力 */
	else if (g_pad[0]->IsTrigger(enButtonLeft))
	{
		next = now.left;
		m_selectSE = sound->PlayingSE(enPadSelect, false);
	}
	/** 右入力 */
	else if (g_pad[0]->IsTrigger(enButtonRight))
	{
		next = now.right;
		m_selectSE = sound->PlayingSE(enPadSelect, false);
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

	/** 決定音を鳴らす */
	SoundManager* sound = FindGO<SoundManager>("soundManager");
	m_decisionSE = sound->PlayingSE(enPadDecision, false);

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
	/** 正解の桁数までしか入力できない */
	if (m_inputText.size() >= static_cast<size_t>(MAX_DIGITS))
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

		/** 閉じる */
		Close();
	}
}

void KeyPad::ResetInput()
{
	m_inputText.clear();
	m_inputFont.SetText(L"");
}