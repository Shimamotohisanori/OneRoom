#pragma once
#include "Source/Actor/Stage/Door/Door.h"
/**
 * HandSetRoomDoor.h
 * ハンドセットルームのドアクラス
 * ここでドアの操作や状態を管理する。
 */
class HandSetRoomDoor : public Door
{
public:
	HandSetRoomDoor();
	~HandSetRoomDoor();


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

