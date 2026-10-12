#pragma once
/**
 * Phone.h
 * 電話クラス
 * 同じクラスを複数生成し、そのうち一つだけ正解の受話器にする
 */
class Player;
class Key;
class Phone : public IGameObject
{
public:
	Phone();
	~Phone();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	/** 設置位置を設定する */
	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}

	/** 設置した際の向きを設定する */
	void SetRotation(const Vector3& rotation)
	{
		m_rotation = rotation;
	}

	/** 正解の受話器かどうかを設定(NewGOの直後に呼ぶ) */
	void SetIsCorrectPhone(bool isCorrect) 
	{ 
		m_isCorrectPhone = isCorrect;
	}

private:
	/** プレイヤーとの距離を計算する */
	float CalculateDistanceToPlayer(const Vector3& playerPosition) const;

	/** プレイヤーがインタラクトの範囲内か */
	bool IsPlayerInRange() const;

	/** インタラクト処理(受話器を取る) */
	void Interact();

	/** 受話器を置く(閉じる) */
	void Close();

	/** カーソルを動かす */
	void UpdateSelection();

	/** 選んでいるボタンを押す */
	void PressSelectedKey();

	/** 数字を1桁足す */
	void InputDigit(int digit);

	/** Enterを押したときの判定 */
	void CheckCode();

	/** 入力をリセットする */
	void ResetInput();

private:
	/** 電話のモデル */
	ModelRender m_phoneModel;

	/** 電話のカーソル画像 */
	SpriteRender m_phoneCursor;

	/** 電話本体の画像 */
	SpriteRender m_phoneSprite;

	/** 操作案内の文字 */
	FontRender m_promptFont;

	/** 入力した数字を表示するフォント */
	FontRender m_inputFont;

	/** プレイヤー */
	Player* m_player = nullptr;

	/** キー */
	Key* m_key = nullptr;

	/** 設置位置 */
	Vector3 m_position = Vector3::Zero;

	/** モデルの向き */
	Vector3 m_rotation = Vector3::Zero;

	/** 正解の受話器か */
	bool m_isCorrectPhone = false;

	/** 操作案内文字を出すか */
	bool m_isPromptVisible = false;

	/** 操作画面を開いているか */
	bool m_isPhoneOpen = false;

	/** すでに鍵を渡したか */
	bool m_isUnlock = false;

	/** いま選んでいるボタンの番号 */
	int m_selectedIndex = 4;

	/** 入力した数字 */
	std::wstring m_inputText;

};

