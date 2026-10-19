#include "stdafx.h"
#include "DarkRoomDoor.h"
#include "Source/Actor/Stage/PlateRoom/DanBall/DanBall.h"
namespace
{
	/** ダークルームのドアのモデルのパス */
	const char* DARK_ROOM_DOOR_MODEL_PATH = "Assets/modelData/Door/Door2.tkm";

	/** ダークルームのドアの座標 */
	const Vector3 DARK_ROOM_DOOR_POSITION = Vector3(-310.0f, -140.0f, 55.0f);
	
	/** ダークルームのドアの拡大率 */
	const Vector3 DARK_ROOM_DOOR_SCALE = Vector3(3.25f, 2.75f, 2.0f);

	/** ダークルームのドアの座標(開いた状態) */
	const Vector3 DARK_ROOM_DOOR_OPEN_POSITION = Vector3(-390.0f, -140.0f, -10.0f);

	/** ダークルームのドアのY軸回転(閉じている状態) */
	const float DARK_ROOM_DOOR_ROTATION_Y = 90.0f;

	/** ダークルームのドアのY軸回転(開いた状態) */
	const float DARK_ROOM_DOOR_OPEN_ROTATION_Y = 180.0f;
}

/** コンストラクタ */
DarkRoomDoor::DarkRoomDoor() {}

/** デストラクタ */
DarkRoomDoor::~DarkRoomDoor() {}

/** ドアのモデルのパスを返す */
const char* DarkRoomDoor::GetModelPath() const { return DARK_ROOM_DOOR_MODEL_PATH; }

/** ドアの座標を返す */
Vector3 DarkRoomDoor::GetDoorPosition() const { return DARK_ROOM_DOOR_POSITION; }

bool DarkRoomDoor::CanOpen() const
{
	/** 段ボールがプレートに置かれていたら開けられる */
	DanBall* danBall = FindGO<DanBall>("DanBall");
	return danBall != nullptr && danBall->IsPlaced();
}

Vector3 DarkRoomDoor::GetDoorScale() const
{
	return DARK_ROOM_DOOR_SCALE;
}

Vector3 DarkRoomDoor::GetDoorOpenPosition() const
{
	return DARK_ROOM_DOOR_OPEN_POSITION;
}

float DarkRoomDoor::GetDoorRotationY() const
{
	return DARK_ROOM_DOOR_ROTATION_Y;
}

float DarkRoomDoor::GetDoorOpenRotationY() const
{
	return DARK_ROOM_DOOR_OPEN_ROTATION_Y;
}

bool DarkRoomDoor::IsInitiallyOpen() const
{
	/** ギミックを解くまで閉じている */
	return false;
}
