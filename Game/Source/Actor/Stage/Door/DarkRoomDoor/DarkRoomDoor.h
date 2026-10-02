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
	bool Start();
	void Update();
	void Render(RenderContext& rc);
};

