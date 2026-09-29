#pragma once

#include "Level3DRender/LevelRender.h"

class Player;
class MainStage;
class Game : public IGameObject
{
public:
	Game() {}
	~Game() {}
	bool Start();
	void Update();
	void Render(RenderContext& rc);

private:
	Player* m_player;
	MainStage* m_mainStage;
};

