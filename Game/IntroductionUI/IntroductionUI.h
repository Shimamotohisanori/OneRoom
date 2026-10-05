#pragma once
/**
 * IntroductionUI.h
 * 操作紹介UI
 */
class IntroductionUI : public IGameObject
{
public:
	IntroductionUI();
	~IntroductionUI();
	bool Start();
	void Update();
	void Render(RenderContext& rc);


private:
	/** 操作紹介UIスプライト */
	SpriteRender m_introductionUISprite;
};

