#pragma once
/**
 * PlayerModel.h
 * プレイヤーモデルクラス
 */
class PlayerModel
{
public:
	PlayerModel();
	~PlayerModel();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	/** プレイヤーの座標を取得 */
	Vector3 GetPosition() const
	{
		return m_position;
	}

	/** プレイヤーの座標を設定 */
	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}

	/** プレイヤーの向きを設定 */
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
	}

private:
	/** プレイヤーの座標 */
	Vector3 m_position = Vector3::Zero;
	
	/** プレイヤーの向き */
	Quaternion m_rotation = Quaternion::Identity;

	/** モデル描画クラス */
	ModelRender m_modelRender;
};

