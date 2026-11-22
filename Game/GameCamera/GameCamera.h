#pragma once
/**
 * GameCamera.h
 * ゲームカメラクラス
 * ここでゲームカメラの操作や状態を管理する。
 */
class Player;
class GameCamera : public IGameObject
{
public:
	GameCamera() {}
	~GameCamera();
	bool Start();
	void Update();

	/** 演出用: 指定した位置と注視点にカメラを置く(演出中は毎フレーム呼ぶ) */
	void SetDirectionView(const Vector3& pos, const Vector3& target);
	
	/** 演出を終えて、通常のカメラに戻す */
	void EndDirection() { m_isDirection = false; }

private:
	/** プレイヤー */
	Player* m_player = nullptr;

	/** 注視点から視点までのベクトル */
	Vector3 m_toCameraPosition;

	/** 累積されるピッチ角(度数) */
	float m_pitch = 0.0f;

	/** 演出中かどうか */
	bool m_isDirection = false;
};

