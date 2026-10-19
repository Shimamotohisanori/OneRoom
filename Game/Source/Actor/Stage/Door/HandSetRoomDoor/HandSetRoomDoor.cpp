#include "stdafx.h"
#include "HandSetRoomDoor.h"
#include "Source/Actor/Stage/DarkRoom/KeyPad/KeyPad.h"
namespace
{
	/** ハンドセットルームのドアのモデルのパス */
	const char* HAND_SET_ROOM_DOOR_MODEL_PATH = "Assets/modelData/Door/Door2.tkm";

	/** ハンドセットルームのドアの座標 */
	const Vector3 HAND_SET_ROOM_DOOR_POSITION = Vector3(370.0f, -140.0f, 660.0f);

	/** ハンドセットルームのドアの拡大率 */
	const Vector3 HAND_SET_ROOM_DOOR_SCALE = Vector3(2.25f, 2.75f, 2.0f);

	/** ハンドセットルームのドアの座標(開いた状態) */
	const Vector3 HAND_SET_ROOM_DOOR_OPEN_POSITION = Vector3(420.0f, -140.0f, 610.0f);

	/** ハンドセットルームのドアのY軸回転(閉じている状態) */
	const float HAND_SET_ROOM_DOOR_ROTATION_Y = 90.0f;

	/** ハンドセットルームのドアのY軸回転(開いた状態) */
	const float HAND_SET_ROOM_DOOR_OPEN_ROTATION_Y = 0.0f;
}

/** コンストラクタ */
HandSetRoomDoor::HandSetRoomDoor() {}

/** デストラクタ */
HandSetRoomDoor::~HandSetRoomDoor() {}

/** ドアのモデルのパスを返す */
const char* HandSetRoomDoor::GetModelPath() const { return HAND_SET_ROOM_DOOR_MODEL_PATH; }

/** ドアの座標を返す */
Vector3 HandSetRoomDoor::GetDoorPosition() const { return HAND_SET_ROOM_DOOR_POSITION; }

bool HandSetRoomDoor::CanOpen() const
{
	KeyPad* keyPad = FindGO<KeyPad>("KeyPad");
	
	/** キーパッドが存在し、かつ解錠されている場合にドアを開けられる */
	return keyPad != nullptr && keyPad->GetIsUnlock();
}

Vector3 HandSetRoomDoor::GetDoorScale() const
{
	return HAND_SET_ROOM_DOOR_SCALE;
}

Vector3 HandSetRoomDoor::GetDoorOpenPosition() const
{
	return HAND_SET_ROOM_DOOR_OPEN_POSITION;
}

float HandSetRoomDoor::GetDoorRotationY() const
{
	return HAND_SET_ROOM_DOOR_ROTATION_Y;
}

float HandSetRoomDoor::GetDoorOpenRotationY() const
{
	return HAND_SET_ROOM_DOOR_OPEN_ROTATION_Y;
}

bool HandSetRoomDoor::IsInitiallyOpen() const
{
	/** 最初は閉じているドア */
	return false;
}
