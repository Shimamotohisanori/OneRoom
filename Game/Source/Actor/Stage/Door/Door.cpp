#include "stdafx.h"
#include "Door.h"
namespace
{
	/** プレイヤーがドアを操作できる距離 */
	constexpr float INTERACT_DISTANCE = 150.0f;

	/** 1秒あたりに開く角度(度数) */
	constexpr float OPEN_SPEED = 120.0f;
}

Door::Door()
{}

Door::~Door()
{}

bool Door::Start()
{
	/** 子クラスが決めたモデルと座標とスケールで初期化 */
	m_doorModel.Init(GetModelPath());
	m_doorModel.SetPosition(GetDoorPosition());
	m_doorModel.SetScale(GetDoorScale());

	/** 閉じているときと開いたときの向きを保存しておく */
	m_closedAngleY = GetDoorRotationY();
	m_openAngleY = GetDoorOpenRotationY();
	
	/** 閉じているときと開いたときの座標を保存しておく */
	m_closedPosition = GetDoorPosition();
	m_openPosition = GetDoorOpenPosition();

	if (IsInitiallyOpen())
	{
		/** 最初から開いた状態にする(アニメーションなし) */
		m_isOpen = true;
		m_currentAngle = m_openAngleY;
		m_doorModel.SetPosition(m_openPosition);
	}
	else
	{
		/** 閉じた状態から始める */
		m_currentAngle = m_closedAngleY;
		m_doorModel.SetPosition(m_closedPosition);
	}

	/** 閉じている状態の向きを設定する */
	Quaternion rot;
	rot.SetRotationDeg(Vector3::AxisY, m_currentAngle);
	m_doorModel.SetRotation(rot);
	
	/** ワールド行列を確定させてから、当たり判定を作る */
	m_doorModel.Update();
	m_doorPhysics.CreateFromModel(
		m_doorModel.GetModel(),
		m_doorModel.GetModel().GetWorldMatrix()
	);

	return true;
}

void Door::Update()
{
	/** 閉じてる間、開ける条件を満たしているなら開く */
	if (m_isOpen == false && CanOpen())
	{
		/** ドアを開く */
		Open();
	}

	UpdateOpenAnimation();
	m_doorModel.Update();

	if (m_isAnimationFinished)
	{
		/** アニメーションが終わったら当たり判定を再構築する */
		RebuildPhysics();
	}
}

void Door::Render(RenderContext& rc)
{
	m_doorModel.Draw(rc);
}

void Door::Open()
{
	/** すでに開いている場合は何もしない */
	if (m_isOpen)
	{
		return;
	}
	m_isOpen = true;

	/** 開いたら当たり判定をなくす */
	m_doorPhysics.Release();

	/** 開いたときの追加処理 */
	OnOpened();
}

void Door::UpdateOpenAnimation()
{
	/** 開いていない、またはすでに開き切っていたら何もしない */
	if (!m_isOpen || m_currentAngle == m_openAngleY)
	{
		return;
	}

	/** 開いたときの向きに向かって、1フレーム分だけ近づける */
	const float step = OPEN_SPEED * g_gameTime->GetFrameDeltaTime();
	const float diff = m_openAngleY - m_currentAngle;

	if (fabsf(diff) <= step)
	{
		m_currentAngle = m_openAngleY;
		m_isAnimationFinished = true;
	}
	else
	{
		m_currentAngle += (diff > 0.0f ? step : -step);
	}

	/** 閉じ→開きのどのあたりかを0〜1で求める */
	const float total = m_openAngleY - m_closedAngleY;

	/** 線形補完で座標を求める */
	/** 徐々に扉を動かす */
	const float t = (total != 0.0f) ? (m_currentAngle - m_closedAngleY) / total : 1.0f;

	Vector3 pos;
	pos.Lerp(t, m_closedPosition, m_openPosition);
	m_doorModel.SetPosition(pos);

	Quaternion rot;
	rot.SetRotationDeg(Vector3::AxisY, m_currentAngle);
	m_doorModel.SetRotation(rot);
}

void Door::RebuildPhysics()
{
	/** いったん消す */
	m_doorPhysics.Release();

	/** ワールド行列を最新にしてから作る */
	m_doorModel.Update();
	m_doorPhysics.CreateFromModel(
		m_doorModel.GetModel(),
		m_doorModel.GetModel().GetWorldMatrix()
	);

	/** アニメーションが終わったフラグをリセットする */
	m_isAnimationFinished = false;
}
