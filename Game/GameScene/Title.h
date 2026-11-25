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

enum TitleImage
{
	enTitleImage_Normal, // (普通の)タイトル画面
	enTitleImage_LitleDark, // (少しくらい)タイトル画面
	enTitleImage_Dark, // (かなり暗い)タイトル画面
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
	/** ゲームスタート待機状態の更新 */
	void UpdateStartWait();

	/** 放置時の点滅処理の更新 */
	void UpdateIdleFlicker();

	/** 何か入力があったか */
	bool IsAnyInput() const;

	/** 全スプライトの更新 */
	void UpdateSprites();

	/** 現在表示するスプライトを取得 */
	SpriteRender& GetCurrentSprite();

	/** min~maxの範囲でランダムな値を取得 */
	float RandomRange(float min, float max) const;

private:
	/** (普通の)タイトルスプライト */
	SpriteRender m_titleSprite;

	/** (少しくらい)タイトルスプライト */
	SpriteRender m_litleDarkTitleSprite;

	/** (かなり暗い)タイトルスプライト */
	SpriteRender m_darkTitleSprite;

	/** 現在表示しているタイトル画像 */
	TitleImage m_currentImage = enTitleImage_Normal;

	/** タイトルのBGM */
	SoundSource* m_titleBGM = nullptr;

	/** 選択時のSE */
	SoundSource* m_selectionSE = nullptr;

	/** ゲーム開始時のSE */
	SoundSource* m_startSE = nullptr;

	/** 決定時のSE */
	SoundSource* m_decisionSE = nullptr;

	/** ゲームスタートのフォント */
	FontRender m_startFont;

	/** 音量調節のフォント */
	FontRender m_volumeFont;

	/** ゲームを閉じるフォント */
	FontRender m_exitFont;

	/** 現在選んでいるフォントのインデックス */
	int m_selectedFontIndex = TitleState::Start;

	/** 無操作の経過時間 */
	float m_idleTimer = 0.0f;

	/** 点滅の経過時間 */
	float m_flickerTimer = 0.0f;

	/** 現在の状態を保つ時間(秒) */
	float m_flickerHoldTime = 0.0f;

	/** ゲームスタートが決定され、SEの再生終了を待っているか */
	bool m_isStarting = false;
};

