#pragma once
/**
 * Transform.h
 * トランスフォームクラス
 */
class Transform
{
private:
	/** 位置 */
	Vector3 m_position = Vector3::Zero;
	
	/** 回転 */
	Quaternion m_rotation = Quaternion::Identity;
	
	/** スケール */
	Vector3 m_scale;


public:
	/** 位置の取得 */
	Vector3& GetPosition() { return m_position; }
	
	/** 回転の取得 */
	Quaternion& GetRotation() { return m_rotation; }
	
	/** スケールの取得 */
	Vector3& GetScale() { return m_scale; }
	
	/** 位置の設定 */
	void SetPosition(Vector3 position) { m_position = position; }
	
	/** 回転の設定 */
	void SetRotation(Quaternion rotation) { m_rotation = rotation; }
	
	/** スケールの設定 */
	void SetScale(Vector3 scale) { m_scale = scale; }
};

