#pragma once
/**
 * MainStage.h
 * メインステージクラス
 * ここでメインステージを管理する。
 */
class MainStage : public IGameObject
{
public:
	MainStage();
	~MainStage();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

private:
	ModelRender m_mainStageRender;
};