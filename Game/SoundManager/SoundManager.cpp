#include "stdafx.h"
#include "SoundManager.h"
#include "sound/SoundEngine.h"
#include "sound/SoundSource.h"
#include <algorithm>
namespace
{
	/** サウンド関連の定数をnamespace内にまとめる */
	const char* BGM_FILEPATH = "Assets/BGM/";
	const char* SE_FILEPATH = "Assets/SE/";
	const char* SOUND_FILE_FORMAT = ".wav";

	/** BGMファイル名一覧 */
	const char* BGM_FILENAME_LIST[enBGMNum] =
	{
		"GameTitleBGM",
		"GameBGM",
		"GameOverBGM",
		"GameClearBGM"
	};

	/** SEファイル名一覧 */
	const char* SE_FILENAME_LIST[enSENum] =
	{
		"PaperInteractSE",
		"LightOnOffSE",
		"PadSelectSE",
		"PadDecisionSE"
	};
}

SoundManager::SoundManager()
{

	m_bgmCount = enBGMNum;
	m_seCount = enSENum;

	/** BGMファイルパスを作って登録する。 */
	for (int i = 0; i < enBGMNum; i++)
	{
		/** BGMファイルパス生成 */
		std::string bgmFile = std::string(BGM_FILEPATH) + BGM_FILENAME_LIST[i] + (SOUND_FILE_FORMAT);
		const char* bgmFilePath = bgmFile.c_str();

		/** BGM登録 */
		g_soundEngine->ResistWaveFileBank(i, bgmFilePath);
	}

	/** SEファイルパスを作って登録する */
	for (int i = 0; i < enSENum; i++)
	{
		/** SEファイルパス生成 */
		std::string seFile = std::string(SE_FILEPATH) + SE_FILENAME_LIST[i] + (SOUND_FILE_FORMAT);
		const char* seFilePath = seFile.c_str();

		/** SE登録 */
		g_soundEngine->ResistWaveFileBank(i + m_bgmCount, seFilePath);

	}
}

SoundManager::~SoundManager()
{}

void SoundManager::Update()
{}

SoundSource* SoundManager::PlayingBGM(SoundBGM number, bool isLoop)
{
	/** 新しく音を鳴らすためのオブジェクトを作成 */
	SoundSource* bgm = NewGO<SoundSource>(0);

	/** どのBGMを鳴らすか設定 */
	bgm->Init(number);

	/** 音量を設定 */
	bgm->SetVolume(m_bgmVolume);

	/** ループ再生するかどうかを指定して再生 */
	bgm->Play(isLoop);

	/** 再生中のBGMとして保持(音量の即時反映用) */
	m_currentBGM = bgm;

	/** 作成した音を返す */
	return bgm;
}

SoundSource* SoundManager::PlayingSE(SoundSE number, bool isLoop)
{
	/** 新しく音を鳴らすオブジェクトを作成 */
	SoundSource* se = NewGO<SoundSource>(0);

	/** SEはBGMの後ろに登録されているのでオフセットを足す */
	se->Init(number + m_bgmCount);

	/** 音量設定 */
	se->SetVolume(m_seVolume);

	/** 再生 */
	se->Play(isLoop);

	return se;
}

void SoundManager::SetBGMVolumeLevel(int level)
{
	/** ここでBGM音量レベルを設定 */
	/** maxとmin関数を使って、0からVOLUME_LEVEL_MAXの範囲内に収める */
	m_bgmVolumeLevel = (std::max)(0, (std::min)(level, VOLUME_LEVEL_MAX));

	/** ここでBGM音量を設定 */
	/** 音量レベルをfloatに変換 */
	m_bgmVolume = static_cast<float>(m_bgmVolumeLevel) / VOLUME_LEVEL_MAX;

	/** 再生中のBGMにも即反映 */
	if (m_currentBGM != nullptr)
	{
		m_currentBGM->SetVolume(m_bgmVolume);
	}
}

void SoundManager::SetSEVolumeLevel(int level)
{
	/** ここでSE音量レベルを設定 */
	/** maxとmin関数を使って、0からVOLUME_LEVEL_MAXの範囲内に収める */
	m_seVolumeLevel = (std::max)(0, (std::min)(level, VOLUME_LEVEL_MAX));

	/** ここでSE音量を設定 */
	/** SEは再生中のSEを保持していないので、即反映はできない */
	m_seVolume = static_cast<float>(m_seVolumeLevel) / VOLUME_LEVEL_MAX;
}
