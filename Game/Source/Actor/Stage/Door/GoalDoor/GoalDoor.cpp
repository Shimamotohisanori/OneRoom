#include "stdafx.h"
#include "GoalDoor.h"
#include "Source/Actor/Player/Player.h"
#include "Key/Key.h"
#include "SoundManager/SoundManager.h"
namespace
{
	/** ゴールののドアのモデルのパス */
	const char* GOAL_DOOR_MODEL_PATH = "Assets/modelData/Door/Door2.tkm";

	/** ゴールのドアの座標 */
	const Vector3 GOAL_DOOR_POSITION = Vector3(25.0f, -140.0f, 1270.0f);

	/** ゴールのドアの拡大率 */
	const Vector3 GOAL_DOOR_SCALE = Vector3(2.5f, 2.75f, 2.0f);

	/** ゴールのドアの座標(開いた状態) */
	const Vector3 GOAL_DOOR_OPEN_POSITION = Vector3(-20.0f, -140.0f, 1320.0f);

	/** 案内文字の表示位置(画面中心が原点、下がマイナス) */
	const Vector3 PROMPT_POSITION = Vector3(-200.0f, -100.0f, 0.0f);

	/** ゴールのドアのY軸回転(閉じている状態) */
	const float GOAL_DOOR_ROTATION_Y = 0.0f;

	/** ゴールのドアのY軸回転(開いた状態) */
	const float GOAL_DOOR_OPEN_ROTATION_Y = -90.0f;

	/** プレイヤーが扉を操作できる距離 */
	constexpr float INTERACT_DISTANCE = 100.0f;

	/** 案内文字のスケール */
	constexpr float PROMPT_FONT_SCALE = 1.5f;

	/** 扉を開けるのに必要な鍵の数 */
	constexpr int REQUIRED_KEY_COUNT = 3;
}
GoalDoor::GoalDoor()
{}

GoalDoor::~GoalDoor()
{}

bool GoalDoor::Start()
{

	/** 親の初期化(モデル・当たり判定) */
	Door::Start();

	/** プレイヤーを検索する */
	m_player = FindGO<Player>("Player");

	/** 鍵の数を数える */
	m_promptFont.SetPosition(PROMPT_POSITION);
	m_promptFont.SetScale(PROMPT_FONT_SCALE);
	m_promptFont.SetColor(Vector4::White);
	return true;
}

void GoalDoor::Update()
{
	/** 毎フレーム非表示から始めて、条件を満たしたときだけ表示する */
	m_isPromptVisible = false;

	/** 鍵の取得 */
	if (m_key == nullptr)
	{
		m_key = FindGO<Key>("Key");
	}

	/** 閉じていて、まだ開ける操作をされていない間だけ案内を出す */
	if (!GetIsOpen() && !m_isOpenRequested &&
		m_player != nullptr && m_key != nullptr && IsPlayerInRange())
	{
		m_isPromptVisible = true;

		if (m_key->GetKeyCount() >= REQUIRED_KEY_COUNT)
		{
			m_promptFont.SetText(L"扉を開ける");

			if (m_player->IsInteractTriggered())
			{
				/** 開く音を再生 */
				SoundManager* sound = FindGO<SoundManager>("soundManager");
				m_doorOpenSound = sound->PlayingSE(enDoorOpenSE, false);

				/** プレイヤーが開ける操作をしたら開く */
				m_isOpenRequested = true;
			}
		}
		else
		{
			m_promptFont.SetText(L"鍵が三つ必要みたいだ");
		}
	}

	/** 親の更新(開く判定・アニメーション・当たり判定の再構築) */
	Door::Update();
}

void GoalDoor::Render(RenderContext & rc)
{
	Door::Render(rc);

	if (m_isPromptVisible)
	{
		m_promptFont.Draw(rc);
	}
}

const char* GoalDoor::GetModelPath() const
{
	return GOAL_DOOR_MODEL_PATH;
}

Vector3 GoalDoor::GetDoorPosition() const
{
	return GOAL_DOOR_POSITION;
}

bool GoalDoor::CanOpen() const
{
	/** プレイヤーが開ける操作をしたら開く */
	return m_isOpenRequested;
}

Vector3 GoalDoor::GetDoorScale() const
{
	return GOAL_DOOR_SCALE;
}

Vector3 GoalDoor::GetDoorOpenPosition() const
{
	return GOAL_DOOR_OPEN_POSITION;
}

float GoalDoor::GetDoorRotationY() const
{
	return GOAL_DOOR_ROTATION_Y;
}

float GoalDoor::GetDoorOpenRotationY() const
{
	return GOAL_DOOR_OPEN_ROTATION_Y;
}

bool GoalDoor::IsInitiallyOpen() const
{
	/** ゴールのドアは最初は閉まっている */
	return false;
}

bool GoalDoor::IsPlayerInRange() const
{
	/** プレイヤーがドアの近くにいるか */
	Vector3 diff = m_player->GetPosition() - GetDoorPosition();
	diff.y = 0.0f;
	return diff.Length() <= INTERACT_DISTANCE;
}
