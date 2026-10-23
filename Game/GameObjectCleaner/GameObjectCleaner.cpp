#include "stdafx.h"
#include "GameObjectCleaner.h"
#include "Source/Actor/Player/Player.h"
#include "Source/Actor/Stage/MainStage.h"
#include "GameCamera/GameCamera.h"
#include "SAN/SANUI.h"
#include "Source/Actor/Stage/PlateRoom/Plate/Plate.h"
#include "Source/Actor/Stage/PlateRoom/DanBall/DanBall.h"
#include "IntroductionUI/IntroductionUI.h"
#include "Key/Key.h"
#include "Key/KeyUI/KeyUI.h"
#include "SAN/SANCalculation.h"
#include "Source/Actor/Stage/DarkRoom/Paper/Paper.h"
#include "Source/Actor/Stage/DarkRoom/KeyPad/KeyPad.h"
#include "Source/Actor/Stage/HandSetRoom/Phone/Phone.h"
#include "Source/Actor/Stage/HandSetRoom/PhonePaper/PhonePaper.h"
#include "Source/Actor/Stage/Door/PlateRoomDoor/PlateRoomDoor.h"
#include "Source/Actor/Stage/Door/DarkRoomDoor/DarkRoomDoor.h"
#include "Source/Actor/Stage/Door/HandSetRoomDoor/HandSetRoomDoor.h"
#include "Source/Actor/Stage/Door/GoalDoor/GoalDoor.h"

void GameObjectCleaner::DeleteAllGameObjects()
{
	DeleteGO(FindGO<MainStage>("MainStage"));
	DeleteGO(FindGO<Player>("Player"));
	DeleteGO(FindGO<GameCamera>("GameCamera"));
	DeleteGO(FindGO<SkyCube>("SkyCube"));
	DeleteGO(FindGO<SANUI>("SanUI"));
	DeleteGO(FindGO<Plate>("Plate"));
	DeleteGO(FindGO<DanBall>("DanBall"));
	DeleteGO(FindGO<IntroductionUI>("IntroductionUI"));
	DeleteGO(FindGO<Key>("Key"));
	DeleteGO(FindGO<KeyUI>("KeyUI"));
	DeleteGO(FindGO<SANCalculation>("SANCalculation"));
	DeleteGO(FindGO<Paper>("Paper"));
	DeleteGO(FindGO<KeyPad>("KeyPad"));

	/** 電話は複数あるので、同じ名前のものをすべて削除 */
	QueryGOs<Phone>("Phone", [](Phone* phone) -> bool
		{
			DeleteGO(phone);
			return true;
		});

	DeleteGO(FindGO<PhonePaper>("PhonePaper"));
	DeleteGO(FindGO<PlateRoomDoor>("PlateRoomDoor"));
	DeleteGO(FindGO<DarkRoomDoor>("DarkRoomDoor"));
	DeleteGO(FindGO<HandSetRoomDoor>("HandSetRoomDoor"));
	DeleteGO(FindGO<GoalDoor>("GoalDoor"));
}

bool GameObjectCleaner::DeleteGameObjectsStep(int step)
{
	switch (step)
	{
	case 0:  DeleteGO(FindGO<MainStage>("MainStage")); break;
	case 1:  DeleteGO(FindGO<Player>("Player")); break;
	case 2:  DeleteGO(FindGO<GameCamera>("GameCamera")); break;
	case 3:  DeleteGO(FindGO<SkyCube>("SkyCube")); break;
	case 4:  DeleteGO(FindGO<SANUI>("SanUI")); break;
	case 5:  DeleteGO(FindGO<Plate>("Plate")); break;
	case 6:  DeleteGO(FindGO<DanBall>("DanBall")); break;
	case 7:  DeleteGO(FindGO<IntroductionUI>("IntroductionUI")); break;
	case 8:  DeleteGO(FindGO<Key>("Key")); break;
	case 9:  DeleteGO(FindGO<KeyUI>("KeyUI")); break;
	case 10: DeleteGO(FindGO<SANCalculation>("SANCalculation")); break;
	case 11: DeleteGO(FindGO<Paper>("Paper")); break;
	case 12: DeleteGO(FindGO<KeyPad>("KeyPad")); break;
	case 13:
		QueryGOs<Phone>("Phone", [](Phone* phone) -> bool
			{
				DeleteGO(phone);
				return true;
			});
		break;
	case 14: DeleteGO(FindGO<PhonePaper>("PhonePaper")); break;
	case 15: DeleteGO(FindGO<PlateRoomDoor>("PlateRoomDoor")); break;
	case 16: DeleteGO(FindGO<DarkRoomDoor>("DarkRoomDoor")); break;
	case 17: DeleteGO(FindGO<HandSetRoomDoor>("HandSetRoomDoor")); break;
	case 18: DeleteGO(FindGO<GoalDoor>("GoalDoor")); break;
	default: return true;
	}
	return false;
}