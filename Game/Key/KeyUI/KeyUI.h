#pragma once

#include "Key/Key.h"
					
class KeyUI : public IGameObject {
public:
	KeyUI();
	~KeyUI();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

private:
	/** キーUIスプライト */
	SpriteRender m_keyUISprite;

	/** キーUIフォント */
	FontRender m_keyUIFont;

	/** キー */
	Key m_key;
};