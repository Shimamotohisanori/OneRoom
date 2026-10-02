#pragma once

#include "Level3DRender/LevelRender.h"
class SANUI;
class Game : public IGameObject
{
public:
	Game() {}
	~Game() {}
	bool Start();
	void Update();
	void Render(RenderContext& rc);

private:
	/** SANUI */
	SANUI* m_sanUI;
};

