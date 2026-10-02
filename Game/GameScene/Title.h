#pragma once
/**
 * Title.h
 * タイトル画面クラス
 * ここでタイトル画面の操作や状態を管理する。
 */
class LoadingScene;
class Title : public IGameObject
{
public:
	Title();
	~Title();
	bool Start();
	void Update();
	void Render(RenderContext& rc);


private:
	/** タイトルスプライト */
	SpriteRender m_titleSprite;

};

