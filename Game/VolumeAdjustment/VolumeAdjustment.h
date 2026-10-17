#pragma once
#include "SoundManager/SoundManager.h"
/** 調整する項目 */
enum EnItem
{
	enItemBGM,
	enItemSE,
	enItemNum
};

/**
 * VolumeAdjustment.h
 * 音量調整クラス
 */
class VolumeAdjustment : public IGameObject
{
public:
	VolumeAdjustment();
	~VolumeAdjustment();
	bool Start();
	void Update();
	void Render(RenderContext& renderContext);

	/** 音量調節画面を開く(どこからでも呼べる) */
	static void Open();

	/** 音量調節画面が開いているか */
	static bool IsOpen();

private:
	/** ゲージの目盛りの数(ここはSoundManagerの最大レベルと同じ) */
	static const int GAUGE_SEGMENT_NUM = 10;

	/** 項目選択の処理 */
	void SelectItem();

	/** 音量変更の処理 */
	void ChangeVolume();

	/** ゲージの見た目を更新する */
	void UpdateGauge();

	/** 項目ごとの現在のレベルを取得・設定 */
	int GetLevel(int item) const;
	void SetLevel(int item, int level);

	/** サウンドマネージャー */
	SoundManager* m_soundManager = nullptr;

	/** ゲージ(項目×目盛り) */
	SpriteRender m_gauge[enItemNum][GAUGE_SEGMENT_NUM];

	/** 項目名("BGM" / "SE") */
	FontRender m_labelFont[enItemNum];

	/** 選択中の項目 */
	int m_selectItem = enItemBGM;

	/** staticにすることで
	 * すべてのインスタンスで共有される */
	/** 開いているかどうかのフラグ */
	static bool m_isOpen;
};

