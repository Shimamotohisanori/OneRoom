#pragma once
/**
 * PlayerModel.h
 * プレイヤーモデルクラス
 */
class PlayerModel
{
public:
	PlayerModel();
	~PlayerModel();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

private:
	ModelRender m_modelRender;
};

