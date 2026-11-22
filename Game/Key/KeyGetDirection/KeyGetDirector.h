#pragma once
class Player;
class GameCamera;

/** 鍵取得時の演出で使用する構造体 */
struct KeyGetDirection
{
	Vector3 cameraPosition;	/** カメラの位置 */
	Vector3 cameraTarget;	/** カメラが映す物の位置 */
	Vector3 KeyPosition;	/** 鍵の位置 */
	float moveTime;         /** カメラの移動時間 */
	float holdTime;         /** カメラが対象を映したまま止まる時間 */
};

/** 
 * KeyGetDirector.h
  * 鍵を取得したときの演出を管理するクラス
 */
class KeyGetDirector : public IGameObject
{
public:
	bool Start();
	void Update();

	/** 演出を始める(演出中なら無視) */
	void Play(const KeyGetDirection& direction);

	/** 演出中かどうか */
	bool IsPlaying() const { return m_phase != EnPhase::Idle; }

private:
	/** 演出のフェーズ */
	enum class EnPhase { Idle, MoveIn, Hold, MoveOut };

	/** 開始値から終了値へ滑らかに補間する */
	static Vector3 Blend(const Vector3& a, const Vector3& b, float t);

private:
	/** プレイヤー */
	Player* m_player = nullptr;

	/** ゲームカメラ */
	GameCamera* m_camera = nullptr;

	/** 演出のフェーズ */
	EnPhase m_phase = EnPhase::Idle;

	/** 演出のタイマー */
	float   m_timer = 0.0f;

	/** 演出の方向 */
	KeyGetDirection m_direction;

	/** 演出前のカメラ(戻り先) */
	Vector3 m_startPos;
	Vector3 m_startTarget;
};

