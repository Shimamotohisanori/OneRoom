#pragma once

/**
 * タイトル画面の状態
 */
enum TitleState
{
	Start,		// ゲームスタート
	Volume,		// 音量調節
	Exit,		// ゲームを閉じる
};

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

	/** タイトル画面の操作に関する処理 */
	void HandleInput();

	/** タイトル画面の選択に関する処理 */
	void HandleSelection();

private:
	/** タイトルスプライト */
	SpriteRender m_titleSprite;

	/** タイトルのBGM */
	SoundSource* m_titleBGM = nullptr;

	/** ゲームスタートのフォント */
	FontRender m_startFont;

	/** 音量調節のフォント */
	FontRender m_volumeFont;

	/** ゲームを閉じるフォント */
	FontRender m_exitFont;

	/** 現在選んでいるフォントのインデックス */
	int m_selectedFontIndex = TitleState::Start;
};

