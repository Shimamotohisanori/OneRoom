#pragma once
/**
 * KeyPad.h
 * キーパッドクラス
 * ここでキーパッドを管理する。
 */

 /** キーパッドのボタンの種類 */
enum class EnKeyButtonType {Number, Enter};

/** キーパッドのボタンの構造体 */
struct KeyButton
{
	EnKeyButtonType type;	/** ボタンの種類 */
	int number;				/** 数字ボタンの場合の数字 */
	Vector2 position;		/** ボタンの位置 */
	int up, down, left, right;	/** 移動先のボタン番号 */
};

class Player;
class Key;
class KeyPad : public IGameObject
{
public:
	KeyPad();
	~KeyPad();
	bool Start();
	void Update();
	void Render(RenderContext& rc);


private:
	/** キーパッドとプレイヤーの距離計算関数 */
	float CalculateDistanceToPlayer(const Vector3& playerPosition) const;

	/** プレイヤーがインタラクトの範囲内か */
	bool IsPlayerInRange() const;

	/** インタラクト処理 */
	void Interact();

	/** キーパッドを閉じる */
	void Close();

	/** カーソルを動かす */
	void UpdateSelection();

	/** 選んでいるボタンを押す */
	void PressSelectedKey();

	/** 数字を1桁足す */
	void InputDigit(int digit);

	/** Enterを押したときの処理 */
	void CheckCode();

	/** 入力をリセットする */
	void ResetInput();

private:
	/** キーパッドモデル */
	ModelRender m_keyPadModel;

	/** 操作案内の文字 */
	FontRender m_promptFont;

	/** 入力した数字を表示するフォント */
	FontRender m_inputFont;

	/** カーソル画像 */
	SpriteRender m_cursorSprite;

	/** キーパッドの画像 */
	SpriteRender m_keyPadSprite;

	/** プレイヤー */
	Player* m_player = nullptr;

	/** キー */
	Key* m_key = nullptr;

	/** 操作案内文字を出すかどうか */
	bool m_isPromptVisible = false;

	/** キーパッドの画像を表示中か */
	bool m_isKeypadOpen = false;

	/** 正解したかどうか */
	bool m_isUnlock = false;

	/** いま選んでいるボタンの番号（KEY_BUTTONSの添字） */
	int m_selectedIndex = 4;

	/** 入力した数字（例: "123"） */
	std::wstring m_inputText;

	
};

