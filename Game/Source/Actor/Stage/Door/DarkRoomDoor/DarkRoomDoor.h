#pragma once
#include "Source/Actor/Stage/Door/Door.h"
/**
 * DarkRoomDoor.h
 * ダークルームのドアクラス
 * ここでドアの操作や状態を管理する。
 */
class DarkRoomDoor : public Door
{
public:
	DarkRoomDoor();
	~DarkRoomDoor();


protected:
	const char* GetModelPath() const override;
	Vector3 GetDoorPosition() const override;
	bool CanOpen() const override;
	Vector3 GetDoorScale() const override;
	Vector3 GetDoorOpenPosition() const override;
	float GetDoorRotationY() const override;
	float GetDoorOpenRotationY() const override;
	bool IsInitiallyOpen() const override;
};

