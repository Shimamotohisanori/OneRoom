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
	/** メインステージのモデル */
	ModelRender m_mainStageRender;

	/** メインステージの当たり判定 */
	PhysicsStaticObject m_mainStagePhysics;
};