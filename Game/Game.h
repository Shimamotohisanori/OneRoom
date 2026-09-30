#pragma once

#include "Level3DRender/LevelRender.h"

class Player;
class MainStage;
class GameCamera;
class Game : public IGameObject
{
public:
	Game() {}
	~Game() {}
	bool Start();
	void Update();
	void Render(RenderContext& rc);

private:
	/** プレイヤー */
	Player* m_player;

	/** メインステージ */
	MainStage* m_mainStage;

	/** ゲームカメラ */
	GameCamera* m_gameCamera;
};

