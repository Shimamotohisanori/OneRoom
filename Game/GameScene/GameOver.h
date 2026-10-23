#pragma once
/**
 * GameOver.h
 * ゲームオーバー画面クラス
 * ここでゲームオーバー画面の操作や状態を管理する。
 */

 /**
  * ゲームオーバー画面のメニュー項目
  * ※TitleStateのStart/Volume/Exitと名前が衝突しないよう、接頭辞を付けている
  */
enum GameOverMenu
{
	enGameOverRetry,		// リトライ
	enGameOverVolume,		// 音量調節
	enGameOverBackTitle,	// タイトルに戻る
	enGameOverExit,			// ゲームを閉じる
	enGameOverMenuNum,		// 項目数
};

class GameOver : public IGameObject
{
public:
	GameOver();
	~GameOver();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	/** ゲームオーバー画面の操作に関する処理 */
	void HandleInput();

	/** ゲームオーバー画面の選択に関する処理 */
	void HandleSelection();

private:
	/** リトライ決定後の待機状態の更新 */
	void UpdateRetryWait();

private:
	/** ゲームオーバースプライト */
	SpriteRender m_gameOverSprite;

	/** ゲームオーバーBGM */
	SoundSource* m_gameOverBGM = nullptr;

	/** 選択時のSE */
	SoundSource* m_selectionSE = nullptr;

	/** リトライ時のSE */
	SoundSource* m_retrySE = nullptr;

	/** リトライのフォント */
	FontRender m_retryFont;

	/** 音量調節のフォント */
	FontRender m_volumeFont;

	/** タイトルに戻るフォント */
	FontRender m_titleFont;

	/** ゲームを閉じるフォント */
	FontRender m_exitFont;

	/** 現在選んでいるフォントのインデックス */
	int m_selectedFontIndex = enGameOverRetry;

	/** リトライが決定され、SEの再生終了を待っているか */
	bool m_isRetrying = false;

	/** タイトルに戻る処理で自身を削除したか */
	bool m_isClosing = false;
};