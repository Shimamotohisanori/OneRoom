#pragma once
#include "Effect/CanActionEffect.h"
class Player;
class Key;
class KeyGetDirector;

/**
 * KeyItem.h
 * ワールドに置かれた鍵。取るとキーの数が増える。
 */
class KeyItem : public IGameObject
{
public:
	KeyItem() {}
	~KeyItem() {}
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	/** 鍵の位置を設定する(NewGOの直後に呼ぶ) */
	void SetPosition(const Vector3& pos) { m_position = pos; }

private:
	/** プレイヤーが取れる範囲内か */
	bool IsPlayerInRange() const;

private:
	/** 鍵のモデル */
	ModelRender m_model;

	/** 取り得るアクションのエフェクト */
	CanActionEffect m_canActionEffect;

	/** プレイヤー */
	Player* m_player = nullptr;

	/** 鍵 */
	Key* m_key = nullptr;

	/** 鍵取得時の演出 */
	KeyGetDirector* m_director = nullptr;

	Vector3 m_position = Vector3::Zero;

	/** 回転と上下の動きのための経過時間 */
	float m_time = 0.0f;
};