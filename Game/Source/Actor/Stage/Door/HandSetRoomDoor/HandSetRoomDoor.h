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
	bool Start();
	void Update();
	void Render(RenderContext& rc);
};

