#pragma once
/**
 * Door.h
 * ドアクラス(親)
 * 全てのドアに共通する、モデル・開閉・インタラクトの処理を持つ。
 * 子クラスは、モデルのパス・座標・開く条件だけを決める。
 */
class Door : public IGameObject
{
public:
	Door();
	~Door();
	bool Start();
	void Update();
	void Render(RenderContext & rc);

	/** ドアが開いているか */
	bool GetIsOpen() const { return m_isOpen; }

	/** ドアを開ける */
	void Open();

/** 保護状態 */
protected:
	/** ドアのモデルのファイルパス */
	virtual const char* GetModelPath() const = 0;

	/** ドアの座標 */
	virtual Vector3 GetDoorPosition() const = 0;

	/** 開ける条件を満たしているか(鍵を持っているとか) */
	virtual bool CanOpen() const { return true; }

	/** 開いた状態の追加処理 */
	virtual void OnOpened() {}

	/** ドアの拡大率(デフォルトは等倍) */
	virtual Vector3 GetDoorScale() const { return Vector3::One; }

	/** ドアのY軸回転(度数。デフォルトは0度) */
	virtual float GetDoorRotationY() const { return 0.0f; }

	/** ドアが開いたときのY軸回転(度数。デフォルトは0度) */
	virtual float GetDoorOpenRotationY() const { return 0.0f; }

	/** 最初から開いているドアか(trueなら開いた状態で始まる。デフォルトはtrue) */
	virtual bool IsInitiallyOpen() const { return true; }

	/** ドアが開いたときの座標(デフォルトは閉じているときと同じ) */
	virtual Vector3 GetDoorOpenPosition() const { return GetDoorPosition(); }


private:
	/** 開閉の動きを更新する */
	void UpdateOpenAnimation();

	/** 当たり判定を再構築する */
	void RebuildPhysics();

private:
	/** ドアのモデル */
	ModelRender m_doorModel;

	/** ドアの当たり判定(静的) */
	PhysicsStaticObject m_doorPhysics;

	/** 閉じているときの座標 */
	Vector3 m_closedPosition = Vector3::Zero;

	/** 開いたときの座標 */
	Vector3 m_openPosition = Vector3::Zero;

	/** 開いているか */
	bool m_isOpen = false;

	/** アニメーションが終わったか */
	bool m_isAnimationFinished = false;

	/** 現在の向き(度数) */
	float m_currentAngle = 0.0f;

	/** 閉じているときの向き(度数) */
	float m_closedAngleY = 0.0f;

	/** 開いたときの向き(度数) */
	float m_openAngleY = 0.0f;
};

