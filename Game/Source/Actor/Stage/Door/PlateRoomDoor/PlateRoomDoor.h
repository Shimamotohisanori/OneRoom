#pragma once
#include "Source/Actor/Stage/Door/Door.h"
/**
 * PlateRoomDoor.h		
 * プレートルームのドアクラス
 * ここでドアの操作や状態を管理する。
 */
class PlateRoomDoor : public Door
{
public:
	PlateRoomDoor();
	~PlateRoomDoor();


protected:
	const char* GetModelPath() const override;
	Vector3 GetDoorPosition() const override;
	bool CanOpen() const override;
	Vector3 GetDoorScale() const override;
	float GetDoorRotationY() const override;
};

