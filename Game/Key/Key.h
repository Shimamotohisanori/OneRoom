#pragma once
/**
 * Key.h
 * キークラス
 * ここでキーの操作や状態を管理する。
 */
class Key : public IGameObject
{
public:
	Key();
	~Key();
	bool Start();
	void Update();

	/** キーの数を増やす */
	void AddKey() { m_keyCount++; }

	/** キーの数を取得 */
	int GetKeyCount() const { return m_keyCount; }

private:
	/** キーの数 */
	int m_keyCount;
};

