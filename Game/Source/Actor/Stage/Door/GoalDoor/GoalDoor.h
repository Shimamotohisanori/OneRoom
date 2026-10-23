#pragma once
#include "Source/Actor/Stage/Door/Door.h"
class Player;
class Key;
/**
 * GoalDoor.h
 * ゴールのドア
 */
class GoalDoor : public Door
{
public:
	GoalDoor();
	~GoalDoor();
	bool Start();
	void Update();
	void Render(RenderContext& rc);


protected:
	const char* GetModelPath() const override;
	Vector3 GetDoorPosition() const override;
	bool CanOpen() const override;
	Vector3 GetDoorScale() const override;
	Vector3 GetDoorOpenPosition() const override;
	float GetDoorRotationY() const override;
	float GetDoorOpenRotationY() const override;
	bool IsInitiallyOpen() const override;


private:
	/** プレイヤーが操作できる距離にいるか */
	bool IsPlayerInRange() const;


private:
	/** プレイヤー */
	Player* m_player = nullptr;

	/** 鍵 */
	Key* m_key = nullptr;

	/** 案内文字(DanBall.h の m_promptFont と同じ型にする) */
	FontRender m_promptFont;

	/** 案内文字を表示するか */
	bool m_isPromptVisible = false;

	/** プレイヤーが開ける操作をしたか */
	bool m_isOpenRequested = false;
};

