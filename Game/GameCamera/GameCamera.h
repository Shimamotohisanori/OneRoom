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

private:
	/** プレイヤー */
	Player* m_player = nullptr;

	/** 注視点から視点までのベクトル */
	Vector3 m_toCameraPosition;

	/** 累積されるピッチ角(度数) */
	float m_pitch = 0.0f;
};

