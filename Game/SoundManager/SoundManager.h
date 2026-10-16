#pragma once
#include "sound/SoundSource.h"
enum SoundBGM
{
	enTitle,
	enGame,
	enGameOver,
	enGameClear,
	enBGMNum
};

enum SoundSE
{
	enPaperInteract,
	enLightOnOff,
	enPadSelect,
	enPadDecision,
	enSENum
};

/**
 * SoundManager.h
 * サウンドマネージャークラス 
 */

class SoundManager : public IGameObject
{
public:
	SoundManager();
	~SoundManager();
	void Update();

	/** BGMを再生する関数 */
	SoundSource* PlayingBGM(SoundBGM number, bool isLoop = true);

	/** SEを再生する関数 */
	SoundSource* PlayingSE(SoundSE number, bool isLoop = true);

	/** BGMの音量を取得する関数 */
	float GetBGMVolume() const
	{
		return m_bgmVolume;
	}

	/** SEの音量を取得する関数 */
	float GetSEVolume() const
	{
		return m_seVolume;
	}

	/** BGMの数を設定する関数 */
	void SetBGMVolume(float volume)
	{
		m_bgmVolume = volume;
	}

	/** SEの数を設定する関数 */
	void SetSEVolume(float volume)
	{
		m_seVolume = volume;
	}


private:
	/** BGM音量 */
	float m_bgmVolume = 0.3f;

	/** SE音量 */
	float m_seVolume = 0.3f;

	/** BGMの数 */
	uint8_t m_bgmCount = 0;

	/** SEの数 */
	uint8_t m_seCount = 0;
};

