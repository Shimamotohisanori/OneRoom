#pragma once
/**
 * CanActionEffect.h
 * アクション可能な事を知らせるエフェクト
 */
class CanActionEffect
{
public:
	CanActionEffect() {}
	~CanActionEffect() { Hide(); }

	/** 表示する(すでに表示中なら位置だけ更新) */
	void Show(const Vector3& pos, const Vector3& scale = Vector3::One);

	/** 消す */
	void Hide();

	/** 位置を更新(オブジェクトが動く場合用) */
	void SetPosition(const Vector3& pos);

	/** 表示中か */
	bool IsShowing() const { return m_emitter != nullptr; }

	/** 距離と遮蔽物を見て、表示/非表示を自動で切り替える(毎フレーム呼ぶ) */
	void UpdateVisibility(const Vector3& effectPos, const Vector3& playerPos, const Vector3& scale = Vector3::One);


private:
	/** プレイヤーから見て遮蔽物があるか */
	static bool IsBlockedByWall(const Vector3& from, const Vector3& to);


private:
	nsK2EngineLow::EffectEmitter* m_emitter = nullptr;
};

