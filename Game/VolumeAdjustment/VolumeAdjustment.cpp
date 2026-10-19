#include "stdafx.h"
#include "VolumeAdjustment.h"

namespace
{
	/** 音量調節画像ファイルパス */
	const char* VOLUME_PATH = "Assets/sprite/SoundControl/Volume.dds";

	/** 他の画面より手前に描画するための優先度 */
	constexpr int VOLUME_PRIORITY = 10;

	/** ゲージ左端のX */
	constexpr float GAUGE_START_X = -100.0f;

	/** ゲージ左端のY */
	constexpr float GAUGE_START_Y = 0.0f;

	/** ゲージの目盛りの間隔 */
	constexpr float GAUGE_SPACING_X = 40.0f;

	/** 項目どうしの間隔 */
	constexpr float ITEM_SPACING_Y = -150.0f;

	/** 項目名のX座標のオフセット */
	constexpr float LABEL_OFFSET_X = -200.0f;

	/** 音量調整画像の横幅 */
	constexpr float VOLUME_WIDTH = 30.0f;

	/** 音量調整画像の縦幅 */
	constexpr float VOLUME_HEIGHT = 60.0f;

	/** フォントの高さのオフセット */
	constexpr float FONT_HEIGHT_OFFSET = 25.0f;

	/** フォントのスケール */
	constexpr float FONT_SCALE = 1.5f;

	/** 戻る案内文字のスケール */
	constexpr float BACK_FONT_SCALE = 1.5f;

	/** 選択中の色 */
	const Vector4 COLOR_ON_SELECT(1.0f, 1.0f, 1.0f, 1.0f);

	/** 非選択中の色 */
	const Vector4 COLOR_ON(0.6f, 0.6f, 0.6f, 1.0f);

	/** 消灯の色 */
	const Vector4 COLOR_OFF(0.15f, 0.15f, 0.15f, 1.0f);

	/** 戻る案内文字の位置 */
	const Vector3 BACK_FONT_POSITION = { -225.0f, -300.0f, 0.0f };
}

/** 音量調節画面が開いているかどうかのフラグ */
bool VolumeAdjustment::m_isOpen = false;

VolumeAdjustment::VolumeAdjustment()
{}

VolumeAdjustment::~VolumeAdjustment()
{
	/** 念のため、破棄されたら必ず閉じた扱いにする */
	m_isOpen = false;
}

bool VolumeAdjustment::Start()
{
	m_soundManager = FindGO<SoundManager>("soundManager");

	const wchar_t* labels[enItemNum] = { L"BGM", L"SE" };

	for (int item = 0; item < enItemNum; item++)
	{
		float y = GAUGE_START_Y + ITEM_SPACING_Y * item;

		/** 項目名 */
		m_labelFont[item].SetText(labels[item]);
		m_labelFont[item].SetPosition({ GAUGE_START_X + LABEL_OFFSET_X, y + FONT_HEIGHT_OFFSET, 0.0f });
		m_labelFont[item].SetScale(FONT_SCALE);

		/** ゲージの目盛り */
		for (int i = 0; i < GAUGE_SEGMENT_NUM; i++)
		{
			m_gauge[item][i].Init(VOLUME_PATH, VOLUME_WIDTH, VOLUME_HEIGHT);
			m_gauge[item][i].SetPosition({ GAUGE_START_X + GAUGE_SPACING_X * i, y, 0.0f });
			m_gauge[item][i].Update();
		}
	}

	/** 戻る案内文字 */
	m_backFont.SetText(L"Bボタンを押して戻る");
	m_backFont.SetPosition(BACK_FONT_POSITION);
	m_backFont.SetScale(BACK_FONT_SCALE);
	m_backFont.SetColor(COLOR_ON_SELECT);

	UpdateGauge();
	return true;
}

void VolumeAdjustment::Update()
{
	SelectItem();
	ChangeVolume();

	/** 閉じる */
	if (g_pad[0]->IsTrigger(enButtonB))
	{
		m_isOpen = false;
		DeleteGO(this);
	}
}

void VolumeAdjustment::SelectItem()
{
	/** 上で項目を選択 */
	if (g_pad[0]->IsTrigger(enButtonUp))
	{
		m_selectItem = (m_selectItem + enItemNum - 1) % enItemNum;
		UpdateGauge();
	}

	/** 下で項目を選択 */
	else if (g_pad[0]->IsTrigger(enButtonDown))
	{
		m_selectItem = (m_selectItem + 1) % enItemNum;
		UpdateGauge();
	}
}

void VolumeAdjustment::ChangeVolume()
{
	/** 左右で音量を変更 */
	int delta = 0;
	if (g_pad[0]->IsTrigger(enButtonLeft)) { delta = -1; }
	if (g_pad[0]->IsTrigger(enButtonRight)) { delta = 1; }
	if (delta == 0) { return; }

	/** 選択中の項目の音量レベルを変更 */
	int level = GetLevel(m_selectItem) + delta;
	if (level < 0) { level = 0; }
	if (level > SoundManager::VOLUME_LEVEL_MAX) { level = SoundManager::VOLUME_LEVEL_MAX; }

	/** 音量レベルを設定 */
	SetLevel(m_selectItem, level);

	/** ゲージの見た目を更新 */
	UpdateGauge();
}

void VolumeAdjustment::UpdateGauge()
{
	/** ゲージの見た目を更新 */
	for (int item = 0; item < enItemNum; item++)
	{
		/** 現在の音量レベルを取得 */
		int level = GetLevel(item);

		/** 選択中の項目かどうか */
		bool isSelected = (item == m_selectItem);

		/** 項目名の色をゲージと揃える(選択中は白、非選択は灰色) */
		m_labelFont[item].SetColor(isSelected ? COLOR_ON_SELECT : COLOR_ON);

		/** ゲージの目盛りごとに色を設定 */
		for (int i = 0; i < GAUGE_SEGMENT_NUM; i++)
		{
			/** 選択中の項目かつ、現在の音量レベルより小さい目盛りは点灯 */
			if (i < level)
			{
				m_gauge[item][i].SetMulColor(isSelected ? COLOR_ON_SELECT : COLOR_ON);
			}
			
			/** それ以外は消灯 */
			else
			{
				m_gauge[item][i].SetMulColor(COLOR_OFF);
			}
			m_gauge[item][i].Update();
		}
	}
}

int VolumeAdjustment::GetLevel(int item) const
{
	return (item == enItemBGM) ? m_soundManager->GetBGMVolumeLevel()
		: m_soundManager->GetSEVolumeLevel();
}

void VolumeAdjustment::SetLevel(int item, int level)
{
	if (item == enItemBGM) { m_soundManager->SetBGMVolumeLevel(level); }
	else { m_soundManager->SetSEVolumeLevel(level); }
}

void VolumeAdjustment::Render(RenderContext& renderContext)
{
	for (int item = 0; item < enItemNum; item++)
	{
		m_labelFont[item].Draw(renderContext);
		for (int i = 0; i < GAUGE_SEGMENT_NUM; i++)
		{
			m_gauge[item][i].Draw(renderContext);
		}
	}

	/** 戻る案内文字を描画 */
	m_backFont.Draw(renderContext);
}

void VolumeAdjustment::Open()
{
	/** すでに開いていたら何もしない */
	if (m_isOpen)
	{
		return;
	}
	m_isOpen = true;
	NewGO<VolumeAdjustment>(VOLUME_PRIORITY, "volumeAdjustment");
}

bool VolumeAdjustment::IsOpen()
{
	return m_isOpen;
}
