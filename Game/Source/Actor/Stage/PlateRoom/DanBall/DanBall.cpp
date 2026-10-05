#include "stdafx.h"
#include "DanBall.h"
#include "Source/Actor/Player/Player.h"
#include "Source/Actor/Stage/PlateRoom/Plate/Plate.h"
namespace
{
	/** 段ボールモデルのファイルパス */
	const char* DAN_BALL_MODEL_FILE_PATH = "Assets/modelData/DanBall/DanBall.tkm";

	/** 段ボールの初期座標 */
	const Vector3 DAN_BALL_INITIAL_POSITION = Vector3(1200.0f, -65.0f, -500.0f);

	/** 段ボールのスケール */
	const Vector3 DAN_BALL_SCALE = Vector3(2.5f, 2.5f, 2.5f);

	/** 手持ち時のスケール */
	const Vector3 DAN_BALL_HELD_SCALE = Vector3(2.5f, 2.5f, 2.5f);

	/** 案内文字の表示位置(画面中心が原点、下がマイナス) */
	const Vector3 PROMPT_POSITION = Vector3(-150.0f, -200.0f, 0.0f);

	/** プレイヤーが段ボールを取得できる距離 */
	const float PICKUP_DISTANCE = 50.0f;

	/** カメラからの前方向オフセット */
	constexpr float HELD_FORWARD_OFFSET = 60.0f;

	/** カメラからの下方向オフセット(マイナスで下) */
	constexpr float HELD_UP_OFFSET = -35.0f;

	/** カメラからの右方向オフセット(中央は0) */
	constexpr float HELD_RIGHT_OFFSET = 0.0f;

	/** プレート上に置いたときの高さオフセット(モデルの原点に合わせて調整) */
	constexpr float PLACED_HEIGHT_OFFSET = 20.0f;

	/** 操作案内フォントのスケール */
	constexpr float PROMPT_FONT_SCALE = 1.5f;
}

DanBall::DanBall()
{}

DanBall::~DanBall()
{}

bool DanBall::Start()
{
	/** 段ボールモデルの初期化 */
	m_danBallModel.Init(DAN_BALL_MODEL_FILE_PATH);

	/** 段ボールの座標を設定 */
	m_danBallModel.SetPosition(DAN_BALL_INITIAL_POSITION);

	/** 段ボールのスケールを設定 */
	m_danBallModel.SetScale(DAN_BALL_SCALE);

	/** プレイヤーの取得 */
	m_player = FindGO<Player>("Player");

	/** プレートの取得 */
	m_plate = FindGO<Plate>("Plate");

	m_promptFont.SetPosition(PROMPT_POSITION);
	m_promptFont.SetScale(PROMPT_FONT_SCALE);
	m_promptFont.SetColor(Vector4::White);
	return true;
}

void DanBall::Update()
{
	/** 毎フレーム非表示から始めて、条件を満たしたときだけ表示する */
	m_isPromptVisible = false;

	switch (m_state)
	{
	case EnState::enState_Ground:
		if (IsPlayerInRange())
		{
			wchar_t text[256];
			swprintf_s(text, L"Aボタンで物を取得");	
			m_promptFont.SetText(text);
			m_isPromptVisible = true;

			if (g_pad[0]->IsTrigger(enButtonA))
			{
				PickUp();
			}
		}
		m_danBallModel.Update();
		break;

	case EnState::enState_Held:
		UpdateHeld();

		/** プレートに近いときだけ「置く」案内を出す */
		if (m_plate != nullptr && m_plate->IsInPlacementRange(m_player->GetPosition()))
		{
			wchar_t text[256];
			swprintf_s(text, L"Aボタンで置く");
			m_promptFont.SetText(text);
			m_isPromptVisible = true;

			if (g_pad[0]->IsTrigger(enButtonA))
			{
				PlaceOnPlate();
			}
		}
		break;

	case EnState::enState_Placed:
		break;
	}
}

void DanBall::Render(RenderContext & renderContext)
{
	/** 段ボールの描画 */
	m_danBallModel.Draw(renderContext);

	/** 操作案内文字の描画 */
	if (m_isPromptVisible)
	{
		m_promptFont.Draw(renderContext);
	}
}

bool DanBall::IsPlayerInRange() const
{
	if (m_player == nullptr) return false;

	Vector3 diff = m_player->GetPosition() - DAN_BALL_INITIAL_POSITION;
	diff.y = 0.0f;  /** 高さは無視 */
	return diff.Length() <= PICKUP_DISTANCE;
}

void DanBall::PickUp()
{
	/** プレイヤーが段ボールを持っている状態にする */
	m_state = EnState::enState_Held;

	/** プレイヤーの持っているフラグを立てる */
	m_player->SetIsHoldingDanBall(true);
}

void DanBall::PlaceOnPlate()
{
	/** プレートに置く */
	m_state = EnState::enState_Placed;

	/** プレイヤーの持っているフラグを下ろす */
	m_player->SetIsHoldingDanBall(false);

	/** プレートの上に配置 */
	Vector3 pos = m_plate->GetPosition();

	/** プレートの上に置くために高さを調整 */
	pos.y += PLACED_HEIGHT_OFFSET;

	m_danBallModel.SetPosition(pos);
	m_danBallModel.SetRotation(Quaternion::Identity);
	m_danBallModel.SetScale(DAN_BALL_SCALE);
	m_danBallModel.Update();
}

void DanBall::UpdateHeld()
{
	/** カメラの位置と向きを取得 */
	const Vector3 camPos = g_camera3D->GetPosition();
	const Vector3 camForward = g_camera3D->GetForward();
	const Vector3 camRight = g_camera3D->GetRight();
	const Vector3 camUp = g_camera3D->GetUp();

	/** カメラの前・下・右にずらした位置を計算 */
	Vector3 pos = camPos;
	pos += camForward * HELD_FORWARD_OFFSET;
	pos += camUp * HELD_UP_OFFSET;
	pos += camRight * HELD_RIGHT_OFFSET;

	/** カメラの左右の向きに段ボールを合わせる(Y軸回転のみ) */
	Quaternion rot;
	rot.SetRotationY(atan2f(camForward.x, camForward.z));

	m_danBallModel.SetPosition(pos);
	m_danBallModel.SetRotation(rot);
	m_danBallModel.SetScale(DAN_BALL_HELD_SCALE);
	m_danBallModel.Update();
}
