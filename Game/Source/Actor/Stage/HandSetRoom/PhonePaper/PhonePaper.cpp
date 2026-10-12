#include "stdafx.h"
#include "PhonePaper.h"
#include "Source/Actor/Player/Player.h"

namespace
{
	/** 紙モデルのファイルパス */
	const char* PHONE_PAPER_MODEL_FILE_PATH =
		"Assets/modelData/Paper/Paper.tkm";

	/** 紙画像のファイルパス */
	const char* PHONE_PAPER_SPRITE_FILE_PATH =
		"Assets/sprite/Paper/PhonePaper.dds";

	/** 紙モデルの初期座標 */
	const Vector3 PHONE_PAPER_INITIAL_POSITION =
		Vector3(700.0f, -90.0f, 430.0f);

	/** 紙画像の初期座標 */
	const Vector3 PHONE_PAPER_SPRITE_INITIAL_POSITION =
		Vector3(0.0f, 0.0f, 0.0f);

	/** 操作案内フォントの座標 */
	const Vector3 PROMPT_POSITION =
		Vector3(-150.0f, -200.0f, 0.0f);

	/** 紙画像の横幅 */
	constexpr float PHONE_PAPER_SPRITE_WIDTH = 1920.0f;

	/** 紙画像の縦幅 */
	constexpr float PHONE_PAPER_SPRITE_HEIGHT = 1080.0f;

	/** プレイヤーがインタラクト可能な距離 */
	constexpr float INTERACT_DISTANCE = 100.0f;

	/** 操作案内フォントのスケール */
	constexpr float PROMPT_SCALE = 1.0f;
}

PhonePaper::PhonePaper()
{}

PhonePaper::~PhonePaper()
{}

bool PhonePaper::Start()
{
	/** プレイヤーを取得 */
	m_player = FindGO<Player>("Player");

	/** 紙モデルの初期化 */
	m_paperModel.Init(PHONE_PAPER_MODEL_FILE_PATH);
	m_paperModel.SetPosition(PHONE_PAPER_INITIAL_POSITION);

	/** 紙画像の初期化 */
	m_paperSprite.Init(
		PHONE_PAPER_SPRITE_FILE_PATH,
		PHONE_PAPER_SPRITE_WIDTH,
		PHONE_PAPER_SPRITE_HEIGHT
	);
	m_paperSprite.SetPosition(PHONE_PAPER_SPRITE_INITIAL_POSITION);

	/** 操作案内フォントの初期化 */
	m_introductionFont.SetPosition(PROMPT_POSITION);
	m_introductionFont.SetScale(PROMPT_SCALE);
	m_introductionFont.SetColor(Vector4::White);

	return true;
}

void PhonePaper::Update()
{
	/** 毎フレーム非表示から始める */
	m_isPromptVisible = false;

	if (m_player != nullptr)
	{
		if (m_isViewing)
		{
			/** 調べ中: Bボタンで閉じる */
			if (m_player->IsCancelTriggered())
			{
				m_isViewing = false;
				m_player->SetInputEnabled(true);
			}
		}
		else if (IsPlayerInRange())
		{
			/** 範囲内: 操作案内を表示 */
			m_introductionFont.SetText(L"Press A to interact");
			m_isPromptVisible = true;

			/** Aボタンで紙を調べる */
			if (m_player->IsInteractTriggered())
			{
				Interact();
			}
		}
	}

	/** 各オブジェクトを更新 */
	m_paperSprite.Update();
	m_paperModel.Update();
}

void PhonePaper::Render(RenderContext& rc)
{
	/** 紙モデルを描画 */
	m_paperModel.Draw(rc);

	if (m_isViewing)
	{
		/** 調べている間は紙画像を表示 */
		m_paperSprite.Draw(rc);
	}
	else if (m_isPromptVisible)
	{
		/** 範囲内では操作案内を表示 */
		m_introductionFont.Draw(rc);
	}
}

float PhonePaper::CalculateDistanceToPlayer(
	const Vector3& playerPosition
) const
{
	Vector3 diff = playerPosition - PHONE_PAPER_INITIAL_POSITION;

	/** 高さの差を無視して距離を計算 */
	diff.y = 0.0f;

	return diff.Length();
}

bool PhonePaper::IsPlayerInRange() const
{
	if (m_player == nullptr)
	{
		return false;
	}

	/** インタラクト可能な距離かどうかを判定 */
	return CalculateDistanceToPlayer(m_player->GetPosition())
		<= INTERACT_DISTANCE;
}

void PhonePaper::Interact()
{
	/** 紙を調べる状態にする */
	m_isViewing = true;

	/** プレイヤーの操作を無効化 */
	m_player->SetInputEnabled(false);
}