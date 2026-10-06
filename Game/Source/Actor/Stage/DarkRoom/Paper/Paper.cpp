#include "stdafx.h"
#include "Paper.h"
#include "Source/Actor/Player/Player.h"
namespace
{
	/** 紙のモデルのファイルパス */
	const char* PAPER_MODEL_FILE_PATH = "Assets/modelData/Paper/Paper.tkm";

	/** 紙画像のファイルパス(照らしている時) */
	const char* PAPER_LIT_SPRITE_FILE_PATH = "Assets/sprite/Paper/Black.dds";

	/** 紙画像のファイルパス(照らしていない時) */
	const char* PAPER_UNLIT_SPRITE_FILE_PATH = "Assets/sprite/Paper/LightNumber.dds";

	/** 紙の初期座標 */
	const Vector3 PAPER_INITIAL_POSITION = Vector3(-900.0f, -10.0f, 210.0f);

	/** 紙画像の初期座標 */
	const Vector3 PAPER_SPRITE_INITIAL_POSITION = Vector3(0.0f, 0.0f, 0.0f);

	/** 操作案内フォントの座標 */
	const Vector3 PROMPT_POSITION = Vector3(-150.0f, -200.0f, 0.0f);

	/** 紙画像の横幅 */
	constexpr float PAPER_SPRITE_WIDTH = 1920.0f;

	/** 紙画像の縦幅 */
	constexpr float PAPER_SPRITE_HEIGHT = 1080.0f;

	/** プレイヤーがインタラクト可能な距離 */
	constexpr float INTERACT_DISTANCE = 100.0f;

	/** 操作案内フォントのスケール */
	constexpr float PROMPT_SCALE = 1.0f;
}
Paper::Paper()
{}

Paper::~Paper()
{}

bool Paper::Start()
{
	m_player = FindGO<Player>("Player");

	m_paperModel.Init(PAPER_MODEL_FILE_PATH);

	m_paperModel.SetPosition(PAPER_INITIAL_POSITION);

	m_litPaperSprite.Init(PAPER_LIT_SPRITE_FILE_PATH, PAPER_SPRITE_WIDTH, PAPER_SPRITE_HEIGHT);
	m_litPaperSprite.SetPosition(PAPER_SPRITE_INITIAL_POSITION);

	m_unlitPaperSprite.Init(PAPER_UNLIT_SPRITE_FILE_PATH, PAPER_SPRITE_WIDTH, PAPER_SPRITE_HEIGHT);
	m_unlitPaperSprite.SetPosition(PAPER_SPRITE_INITIAL_POSITION);

	/** 操作案内フォントの初期化 */
	m_introductionFont.SetPosition(PROMPT_POSITION);
	m_introductionFont.SetScale(PROMPT_SCALE);
	m_introductionFont.SetColor(Vector4::White);
	return true;
}

void Paper::Update()
{
	/** 毎フレーム非表示から始める */
	m_isPromptVisible = false;

	if (m_player != nullptr)
	{
		if (m_isViewing)
		{
			/** 調べ中: Bで閉じる */
			if (m_player->IsCancelTriggered())
			{
				m_isViewing = false;
				m_player->SetInputEnabled(true);
			}
		}
		else if (IsPlayerInRange())
		{
			/** 範囲内: 案内を出し、Aで調べ始める */
			m_introductionFont.SetText(L"Press A to interact");
			m_isPromptVisible = true;

			if (m_player->IsInteractTriggered())
			{
				Interact();
			}
		}
	}

	m_litPaperSprite.Update();
	m_unlitPaperSprite.Update();
	m_paperModel.Update();
}

void Paper::Render(RenderContext & rc)
{
	m_paperModel.Draw(rc);

	if (m_isViewing && m_player != nullptr)
	{
		/** ライト ON なら数字なし、OFF なら数字が浮かぶ画像 */
		if (m_player->IsLightOn())
		{
			m_litPaperSprite.Draw(rc);
		}
		else
		{
			m_unlitPaperSprite.Draw(rc);
		}
	}
	else if (m_isPromptVisible)
	{
		m_introductionFont.Draw(rc);
	}
}

float Paper::CalculateDistanceToPlayer(const Vector3& playerPosition) const
{
	Vector3 diff = playerPosition - PAPER_INITIAL_POSITION;
	diff.y = 0.0f;
	return diff.Length();
}

bool Paper::IsPlayerInRange() const
{
	if (m_player == nullptr) return false;

	/** プレイヤーとの距離を計算して、インタラクト可能な距離かどうかを返す */
	return CalculateDistanceToPlayer(m_player->GetPosition()) <= INTERACT_DISTANCE;
}

void Paper::Interact()
{
	/** 調べ始める。操作を止める */
	m_isViewing = true;
	m_player->SetInputEnabled(false);
}
