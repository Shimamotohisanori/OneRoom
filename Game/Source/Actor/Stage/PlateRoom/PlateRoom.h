#pragma once
/**
 * PlateRoom.h
 * プレートルームクラス
 * ここでプレートルームを管理する。
 */
class PlateRoom : public IGameObject
{
public:
	PlateRoom();
	~PlateRoom();
	bool Start();
	void Update();

private:

};

