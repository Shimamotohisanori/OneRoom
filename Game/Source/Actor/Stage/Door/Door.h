#pragma once
/**
 * Door.h
 * ドアクラス
 * ここでドアの操作や状態を管理する。
 */
class Door : public IGameObject
{
public:
	Door();
	~Door();
	bool Start();
	void Update();
	void Render(RenderContext & rc);

};

