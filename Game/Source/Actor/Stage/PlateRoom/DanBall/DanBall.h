#pragma once
/**
 * DanBall.h
 * 段ボールクラス
 * ここで段ボールを管理する。
 */
class Player;
class Plate;
class Key;
class SANCalculation;
class DanBall : public IGameObject
{
public:
	DanBall();
	~DanBall();
	bool Start();
	void Update();
	void Render(RenderContext& renderContext);


private:
	/** 段ボールの状態 */
	enum class EnState
	{
		enState_Ground,  // 地面に落ちている
		enState_Held,    // プレイヤーが持っている
		enState_Placed,  // プレートに置かれた
	};

	/** プレイヤーが取得範囲内にいるか */
	bool IsPlayerInRange() const;

	/** 取得処理 */
	void PickUp();

	/** プレートに置く処理 */
	void PlaceOnPlate();

	/** 取得後、カメラの前下に追従させる */
	void UpdateHeld();

private:
	/** 段ボールモデル */
	ModelRender m_danBallModel;

	/** 段ボールの状態 */
	FontRender m_promptFont;

	/** プレイヤー */
	Player* m_player = nullptr;
	
	/** キー */
	Key* m_key = nullptr;

	/** SAN計算クラス */
	SANCalculation* m_sanCalculation = nullptr;

	/** プレート */
	Plate* m_plate = nullptr;

	/** 段ボールの状態 */
	EnState m_state = EnState::enState_Ground;

	/** 取得済みか */
	bool m_isPickedUp = false;

	/** このフレームに案内文字を出すか */
	bool m_isPromptVisible = false;
};

