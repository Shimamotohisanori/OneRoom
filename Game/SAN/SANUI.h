#pragma once
#include "SANCalculation.h"
/**
 * SANUI.h
 * SANUIクラス
 * ここでSANUIの操作や状態を管理する。
 */
class SANUI : public IGameObject
{
public:
	SANUI() {}
	~SANUI();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

private:
	/** SANUIスプライト */
	SpriteRender m_sanUISprite;

	/** SANUIフォント */
	FontRender m_sanUIFont;

	/** SAN値の計算クラス */
	SANCalculation m_sanCalculation;
};

