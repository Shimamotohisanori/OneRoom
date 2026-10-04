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

private:
	/** プレートを踏んだときの処理 */
	void OnSteppedOn();

	/** プレートのモデル */
	ModelRender m_plateModel;

	/** プレートを踏んだかどうか */
	bool m_isSteppedOn;

};

