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
	bool Start();
	void Update();
	void Render(RenderContext& rc);
};

