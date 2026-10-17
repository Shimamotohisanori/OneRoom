#pragma once

/**
 * 一時停止画面の状態
 * ※TitleStateと名前が衝突しないように接頭辞を付けている
 */
enum PauseState
{
	PauseResume,	// ゲームに戻る
	PauseTitle,		// タイトルに戻る
	PauseVolume,	// 音量調節
	PauseExit,		// ゲームを閉じる
};

/**
* Pause.h
* 一時停止機能を提供するクラス
*/
class Player;
class Pause : public IGameObject
{
public:
	Pause();
	~Pause();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	/** 一時停止状態フラグの取得 */
	bool GetIsPaused() const
	{
		return m_isPaused;
	}

	/** 一時停止状態フラグの設定 */
	void SetIsPaused(bool isPaused)
	{
		m_isPaused = isPaused;
	}

	/** 一時停止画面の操作に関する処理 */
	void HandleInput();

	/** 一時停止画面の選択に関する処理 */
	void HandleSelection();

	/** 選択状態に応じてフォントの色を更新する処理 */
	void UpdateFontColor();

private:
	/** ゲーム内のオブジェクトをすべて削除する処理 */
	void DeleteAllGameObjects();


private:
	/** 一時停止画面のスプライト */
	SpriteRender m_pauseSprite;

	/** ゲームに戻るフォント */
	FontRender m_resumeFont;

	/** タイトルに戻るフォント */
	FontRender m_titleFont;

	/** 音量調整フォント */
	FontRender m_volumeFont;

	/** ゲームを閉じるフォント */
	FontRender m_exitFont;

	/** プレイヤー */
	Player* m_player;

	/** 現在選んでいるフォントのインデックス */
	int m_selectedFontIndex = PauseState::PauseResume;

	/** 一時停止中かどうかのフラグ */
	bool m_isPaused = false;
};