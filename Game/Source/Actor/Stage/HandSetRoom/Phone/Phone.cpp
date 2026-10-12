#include "stdafx.h"
#include "Phone.h"
#include "Source/Actor/Player/Player.h"
#include "Key/Key.h"

/** これを使いKeyButtonの構造体を利用する */
#include "Source/Actor/Stage/Darkroom/KeyPad/KeyPad.h"
namespace
{
	/** 電話のモデルのファイルパス */
	const char* PHONE_MODEL_FILE_PATH = "Assets/modelData/Phone/Phone.tkm";

	/** 電話のカーソル画像のファイルパス */
	const char* PHONE_CURSOR_FILE_PATH = "Assets/sprite/Black/Black.dds";

	/** 電話本体の画像のファイルパス */
	const char* PHONE_SPRITE_FILE_PATH = "Assets/sprite/Phone/Phone.dds";

	/** 正解の四桁 */
	const std::wstring CORRECT_CODE = L"5921";

	/** 電話本体の画像の縦幅 */
	constexpr float PHONE_SPRITE_HEIGHT = 900.0f;

	/** 電話本体の画像の横幅 */
	constexpr float PHONE_SPRITE_WIDTH = 950.0f;

	/** 電話のカーソル画像の縦幅 */
	constexpr float PHONE_CURSOR_HEIGHT = 52.5f;

	/** 電話のカーソル画像の横幅 */
	constexpr float PHONE_CURSOR_WIDTH = 52.5f;

	/** 操作案内文字のスケール */
	constexpr float PROMPT_FONT_SCALE = 1.5f;

	/** 入力した数字のスケール */
	constexpr float INPUT_FONT_SCALE = 1.25f;

	/** インタラクトできる距離 */
	constexpr float INTERACT_DISTANCE = 50.0f;

	/** 電話モデルの大きさ */
	const Vector3 PHONE_MODEL_SCALE = Vector3(0.3f, 0.3f, 0.3f);

	/** 案内文字の位置 */
	const Vector3 PROMPT_POSITION = Vector3(-150.0f, -200.0f, 0.0f);
	
	/** 入力数字の位置 */
	const Vector3 DIGITS_POSITION = Vector3(0.0f, 280.0f, 0.0f);

	/** カーソルの色(半透明) */
	const Vector4 CURSOR_COLOR = Vector4(1.0f, 1.0f, 1.0f, 0.5f);

	/** 入力できる最大桁数 */
	constexpr int MAX_DIGITS = 4;

	constexpr int NONE = -1;
	constexpr int KEY_COUNT = 13;

	/** ボタン配置(キーパッドと同じ。画像に合わせて調整してください) */
	const KeyButton KEY_BUTTONS[KEY_COUNT] =
	{
		//  種類                 数字  　座標(x, y)        上    下    左    右
		{ EnKeyButtonType::Number, 1, {  15.0f,  85.0f}, NONE,    3, NONE,    1 },  // [0]
		{ EnKeyButtonType::Number, 2, {  92.5f,  85.0f}, NONE,    4,    0,    2 },  // [1]
		{ EnKeyButtonType::Number, 3, { 167.5f,  85.0f}, NONE,    5,    1, NONE },  // [2]
		{ EnKeyButtonType::Number, 4, {  15.0f,  5.0f},    0,    6, NONE,    4 },  // [3]
		{ EnKeyButtonType::Number, 5, {  92.5f,  5.0f},    1,    7,    3,    5 },  // [4]
		{ EnKeyButtonType::Number, 6, { 167.5f,  5.0f},    2,    8,    4, NONE },  // [5]
		{ EnKeyButtonType::Number, 7, {  15.0f,  -75.0f},    3,    9, NONE,    7 },  // [6]
		{ EnKeyButtonType::Number, 8, {  92.5f,  -75.0f},    4,   10,    6,    8 },  // [7]
		{ EnKeyButtonType::Number, 9, { 167.5f,  -75.0f},    5,   11,    7, NONE },  // [8]
		{ EnKeyButtonType::Symbol, 0, {  15.0f, -160.0f},    6,   12, NONE,   10 },  // [9]  *
		{ EnKeyButtonType::Number, 0, {  92.5f, -160.0f},    7,   12,    9,   11 },  // [10] 0
		{ EnKeyButtonType::Symbol, 0, { 167.5f, -160.0f},    8,   12,   10, NONE },  // [11] #
		{ EnKeyButtonType::Enter,  0, { 210.0f, -230.0f},   10, NONE, NONE, NONE },  // [12] Enter
	};
}

Phone::Phone()
{}

Phone::~Phone()
{
	/** 開いたまま破棄されてもプレイヤーが動けなくならないようにする */
	if (m_isPhoneOpen && m_player != nullptr)
	{
		m_player->SetInputEnabled(true);
	}
}

bool Phone::Start()
{
	/** 電話の初期化 */
	m_phoneModel.Init(PHONE_MODEL_FILE_PATH);
	m_phoneModel.SetPosition(m_position);
	m_phoneModel.SetScale(PHONE_MODEL_SCALE);

	/** Y軸(左右の向き)だけ回転させる */
	Quaternion rot;
	rot.SetRotationDeg(Vector3::AxisY, m_rotation.y);
	m_phoneModel.SetRotation(rot);

	/** カーソルの初期化 */
	m_phoneCursor.Init(PHONE_CURSOR_FILE_PATH, PHONE_CURSOR_WIDTH, PHONE_CURSOR_HEIGHT);

	/** 受話器の初期化 */
	m_phoneSprite.Init(PHONE_SPRITE_FILE_PATH, PHONE_SPRITE_WIDTH, PHONE_SPRITE_HEIGHT);

	/** プレイヤーの取得 */
	m_player = FindGO<Player>("Player");

	/** キーの取得 */
	m_key = FindGO<Key>("Key");

	/** フォントの初期化 */
	m_promptFont.SetPosition(PROMPT_POSITION);
	m_promptFont.SetScale(PROMPT_FONT_SCALE);
	m_promptFont.SetColor(Vector4::White);

	/** 入力した数字を表示するフォントの初期化 */
	m_inputFont.SetPosition(DIGITS_POSITION);
	m_inputFont.SetScale(INPUT_FONT_SCALE);
	m_inputFont.SetColor(Vector4::White);
	return true;
}

void Phone::Update()
{
	/** 受話器を開いているときの処理 */
	if (m_isPhoneOpen)
	{
		/** キャンセルボタンを押したとき */
		if (m_player->IsCancelTriggered())
		{
			/** 受話器を閉じる */
			Close();
			return;
		}

		/** カーソルの移動 */
		UpdateSelection();

		/** Aボタンを押したとき */
		if (m_player->IsInteractTriggered())
		{
			/** 選択したボタンを押す */
			PressSelectedKey();
		}

		/** 入力した数字を表示する */
		m_phoneCursor.SetPosition(Vector3(
			KEY_BUTTONS[m_selectedIndex].position.x,
			KEY_BUTTONS[m_selectedIndex].position.y,
			0.0f));
		m_phoneCursor.Update();
		m_phoneSprite.Update();
		m_phoneModel.Update();
		return;
	}

	/** 受話器を開いていないときの処理 */
	m_isPromptVisible = false;

	/** プレイヤーがインタラクトの範囲内にいるとき */
	if (IsPlayerInRange())
	{
		/** 操作案内文字を表示する */
		m_promptFont.SetText(L"Press A to pick up the phone");
		m_isPromptVisible = true;

		/** Aボタンを押したとき */
		if (m_player->IsInteractTriggered())
		{
			/** 受話器を取る */
			Interact();
		}
	}
	m_phoneModel.Update();
}

void Phone::Render(RenderContext & rc)
{
	/** 電話のモデルを描画 */
	m_phoneModel.Draw(rc);

	/** 受話器を開いているときの描画 */
	if (m_isPhoneOpen)
	{
		m_phoneSprite.Draw(rc);
		m_inputFont.Draw(rc);
		m_phoneCursor.SetMulColor(CURSOR_COLOR);
		m_phoneCursor.Draw(rc);
		return;
	}

	/** 受話器を開いていないときの描画 */
	if (m_isPromptVisible)
	{
		m_promptFont.Draw(rc);
	}
}

float Phone::CalculateDistanceToPlayer(const Vector3& playerPosition) const
{
	/** プレイヤーの位置と電話の位置の差を計算 */
	Vector3 diff = playerPosition - m_position;
	diff.y = 0.0f;
	return diff.Length();
}

bool Phone::IsPlayerInRange() const
{
	/** プレイヤーが存在しない場合は範囲外とする */
	if (m_player == nullptr) return false;
	return CalculateDistanceToPlayer(m_player->GetPosition()) <= INTERACT_DISTANCE;
}

void Phone::Interact()
{
	/** 受話器を開く */
	m_isPhoneOpen = true;

	/** 操作案内文字を非表示にする */
	m_isPromptVisible = false;

	/** プレイヤーの入力を無効化する */
	m_player->SetInputEnabled(false);
}

void Phone::Close()
{
	/** 受話器を閉じる */
	m_isPhoneOpen = false;

	/** プレイヤーの入力を有効化する */
	m_player->SetInputEnabled(true);
	
	/** 入力をリセットする */
	ResetInput();
}

void Phone::UpdateSelection()
{
	/** 現在選択しているボタンの情報を取得 */
	const KeyButton& now = KEY_BUTTONS[m_selectedIndex];
	int next = NONE;

	/** ゲームパッドの入力に応じてカーソルを移動 */
	if (g_pad[0]->IsTrigger(enButtonUp))         next = now.up;
	else if (g_pad[0]->IsTrigger(enButtonDown))  next = now.down;
	else if (g_pad[0]->IsTrigger(enButtonLeft))  next = now.left;
	else if (g_pad[0]->IsTrigger(enButtonRight)) next = now.right;

	/** 移動先が存在する場合は選択中のボタンを更新 */
	if (next != NONE)
	{
		m_selectedIndex = next;
	}
}

void Phone::PressSelectedKey()
{
	/** 現在選択しているボタンの情報を取得 */
	const KeyButton& key = KEY_BUTTONS[m_selectedIndex];

	/** ボタンの種類に応じて処理を分岐 */
	if (key.type == EnKeyButtonType::Number)
	{
		/** 数字ボタンが押された場合は入力した数字を追加 */
		InputDigit(key.number);
	}
	/** Enterボタンが押された場合は入力した数字を判定 */
	else if (key.type == EnKeyButtonType::Enter)
	{
		/** 入力した数字が正解のコードと一致するかを判定 */
		CheckCode();
	}
	/** Symbol(* や #)は何もしない */
}

void Phone::InputDigit(int digit)
{
	/** 入力できる最大桁数を超えている場合は何もしない */
	if (m_inputText.size() >= MAX_DIGITS) return;

	m_inputText += static_cast<wchar_t>(L'0' + digit);
	m_inputFont.SetText(m_inputText.c_str());
}

void Phone::CheckCode()
{
	/** 入力した数字が正解のコードと一致するかを判定 */
	if (m_isCorrectPhone && m_inputText == CORRECT_CODE && !m_isUnlock)
	{
		/** 正解した場合は鍵を渡す */
		m_isUnlock = true;

		/** キーの数を増やす */
		m_key->AddKey();

		/** 受話器を閉じる */
		Close();
	}
	else
	{
		/** 不正解の場合は入力をリセットする */
		ResetInput();

		/** 受話器を閉じる */
		Close();
	}
}

void Phone::ResetInput()
{
	/** 入力した数字をリセットする */
	m_inputText.clear();

	/** 入力表示をリセットする */
	m_inputFont.SetText(L"");
}
