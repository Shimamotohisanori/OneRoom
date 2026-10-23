#pragma once

/**
 * ゲームクリア画面のメニュー項目
 * ※TitleStateやGameOverMenuと名前が衝突しないよう、接頭辞を付けている
 */
enum GameClearMenu
{
    enGameClearRetry,		// リトライ
    enGameClearBackTitle,	// タイトルに戻る
    enGameClearVolume,		// 音量調節
    enGameClearExit,		// ゲームを閉じる
    enGameClearMenuNum,		// 項目数
};

/**
 * GameClear.h
 * ゲームクリア画面クラス
 * ここでゲームクリア画面の操作や状態を管理する。
 */
class GameClear : public IGameObject
{
public:
    GameClear();
    ~GameClear();

    bool Start();
    void Update();
    void Render(RenderContext& rc);

    /** ゲームクリア画面の操作に関する処理 */
    void HandleInput();

    /** ゲームクリア画面の選択に関する処理 */
    void HandleSelection();

private:
    /** リトライ決定後の待機状態の更新 */
    void UpdateRetryWait();

private:
    /** ゲームクリアスプライト */
    SpriteRender m_clearSprite;

    /** ゲームクリアのBGM */
    SoundSource* m_clearBGM = nullptr;

    /** 選択時のSE */
    SoundSource* m_selectionSE = nullptr;

    /** リトライ時のSE */
    SoundSource* m_retrySE = nullptr;

    /** リトライのフォント */
    FontRender m_retryFont;

    /** タイトルに戻るのフォント */
    FontRender m_titleFont;

    /** 音量調節のフォント */
    FontRender m_volumeFont;

    /** ゲームを閉じるフォント */
    FontRender m_exitFont;

    /** 現在選んでいるフォントのインデックス */
    int m_selectedFontIndex = enGameClearRetry;

    /** リトライが決定され、SEの再生終了を待っているか */
    bool m_isRetrying = false;

    /** タイトルに戻る処理で自身を削除したか */
    bool m_isClosing = false;
};