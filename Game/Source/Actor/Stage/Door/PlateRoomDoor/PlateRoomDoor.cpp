#include "stdafx.h"
#include "PlateRoomDoor.h"
namespace
{
	/** プレートルームのドアのモデルのパス */
	const char* PLATE_ROOM_DOOR_MODEL_PATH = "Assets/modelData/Door/Door2.tkm";

	/** プレートルームのドアの座標 */
	const Vector3 PLATE_ROOM_DOOR_POSITION = Vector3(430.0f, -140.0f, -410.0f);

	/** プレートルームのドアの拡大率 */
	const Vector3 PLATE_ROOM_DOOR_SCALE = Vector3(2.5f, 2.75f, 2.0f);

	/** プレートルームのドアのY軸回転(閉じている状態) */
	const float PLATE_ROOM_DOOR_ROTATION_Y = 0.0f;
}

/** コンストラクタ */
PlateRoomDoor::PlateRoomDoor() {}

/** デストラクタ */
PlateRoomDoor::~PlateRoomDoor() {}

/** ドアのモデルのパスを返す */
const char* PlateRoomDoor::GetModelPath() const { return PLATE_ROOM_DOOR_MODEL_PATH; }

/** ドアの座標を返す */
Vector3 PlateRoomDoor::GetDoorPosition() const { return PLATE_ROOM_DOOR_POSITION; }

bool PlateRoomDoor::CanOpen() const
{
	/** 例:段ボールをプレートに置いて鍵を取得していたら開けられる */
	return true; // ここに条件を書く
}

Vector3 PlateRoomDoor::GetDoorScale() const
{
	return PLATE_ROOM_DOOR_SCALE;
}

float PlateRoomDoor::GetDoorRotationY() const
{
	return PLATE_ROOM_DOOR_ROTATION_Y;
}
