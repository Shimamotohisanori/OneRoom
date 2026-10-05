#pragma once
/**
 * Plate.h
 * プレートクラス
 * ここでプレートを管理する。
 */
class Plate : public IGameObject
{
public:
	Plate();
	~Plate();
	bool Start();
	void Update();
	void Render(RenderContext& renderContext);

	/** プレートを踏んだかどうかを取得 */
	bool IsSteppedOn() const
	{
		return m_isSteppedOn;
	}

	/** プレートの座標を取得 */
	Vector3 GetPosition() const
	{
		return m_position;
	}

	/** 指定座標がプレートの設置範囲内か(高さは無視) */
	bool IsInPlacementRange(const Vector3& position) const;

private:
	/** プレートを踏んだときの処理 */
	void OnSteppedOn();

	/** プレートのモデル */
	ModelRender m_plateModel;

	/** プレートを(段ボールが)踏んだかどうか */
	bool m_isSteppedOn = false;

	Vector3 m_position = Vector3::Zero;

};

