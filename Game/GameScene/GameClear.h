#pragma once

/**
 * GameClear.h
 * ゲームクリア画面クラス
 * ここでゲームクリア画面の操作や状態を管理する。
 */

class GameCamera;
class SANUI;
class Player;
class MainStage;
class Plate;
class DanBall;
class IntroductionUI;
class Key;
class KeyUI;
class SANCalculation;
class Paper;
class KeyPad;
class Phone;
class PhonePaper;

class GameClear : public IGameObject
{
public:
    GameClear();
    ~GameClear();

    bool Start();
    void Update();
    void Render(RenderContext& rc);

private:
    /** ゲームクリアスプライト */
    SpriteRender m_clearSprite;

    /** プレイヤー */
    Player* m_player;

	/** SAN UI */
    SANUI* m_sanUI;

	/** メインステージ */
    MainStage* m_mainStage;

	/** ゲームカメラ */
    GameCamera* m_gameCamera;

	/** 空 */
    SkyCube* m_skyCube;

	/** プレート */
    Plate* m_plate;

	/** 段ボール */
    DanBall* m_danBall;

	/** 操作紹介UI */
    IntroductionUI* m_introductionUI;

	/** キー */
    Key* m_key;

	/** キーUI */
    KeyUI* m_keyUI;

	/** SAN計算クラス */
    SANCalculation* m_sanCalculation;

	/** 紙 */
    Paper* m_paper;

	/** キーパッド */
    KeyPad* m_keyPad;

    /** 電話 */
	Phone* m_phone;

	/** 電話の紙 */
	PhonePaper* m_phonePaper;

    /** ゲームクリアのBGM */
    SoundSource* m_clearBGM;
};