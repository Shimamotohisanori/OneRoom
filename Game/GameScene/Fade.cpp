#include "stdafx.h"
#include "Fade.h"

namespace
{
	/** 画像のパス。GoalLightで使っている真っ白な画像を流用する */
	const char* FADE_WHITE_SPRITE_PATH = "Assets/sprite/White/White.dds";

	/** 画面全体の大きさ */
	constexpr float FADE_WIDTH = 1920.0f;
	constexpr float FADE_HEIGHT = 1080.0f;

	/** durationが0以下だった時の保険(0除算を防ぐ) */
	constexpr float MIN_DURATION = 0.01f;
}

Fade::Fade()
{
}

Fade::~Fade()
{
}

bool Fade::Start()
{
	m_sprite.Init(FADE_WHITE_SPRITE_PATH, FADE_WIDTH, FADE_HEIGHT);
	m_sprite.SetMulColor(Vector4(m_color.x, m_color.y, m_color.z, 0.0f));
	m_sprite.Update();
	return true;
}

void Fade::StartFadeOut(const Vector3& color, float duration)
{
	m_onCovered = nullptr;
	m_color = color;
	m_speed = 1.0f / max(duration, MIN_DURATION);
	m_state = EnFadeState::enFadeState_FadingOut;
}

void Fade::StartFadeIn(float duration)
{
	m_speed = 1.0f / max(duration, MIN_DURATION);
	m_state = EnFadeState::enFadeState_FadingIn;
}

void Fade::StartTransition(const Vector3& color, float outDuration, float inDuration, std::function<void()> onCovered)
{
	/** フェードアウトを開始する */
	StartFadeOut(color, outDuration);

	/** onCoveredを保存しておく。フェードアウトが終わったら実行する */
	m_onCovered = std::move(onCovered);

	/** フェードインにかける時間を保存しておく */
	m_inDuration = inDuration;
}

void Fade::Update()
{
	/** 何もしていない時と、覆い切った時は、透明度を変えない */
	if (m_state == EnFadeState::enFadeState_None
		|| m_state == EnFadeState::enFadeState_Covered) {
		return;
	}

	/** 重いフレームでフェードが一瞬で終わらないよう、上限をつける */
	const float deltaTime = min(g_gameTime->GetFrameDeltaTime(), 1.0f / 30.0f);

	if (m_state == EnFadeState::enFadeState_FadingOut)
	{
		m_alpha += m_speed * deltaTime;
		if (m_alpha >= 1.0f) {
			m_alpha = 1.0f;
			m_state = EnFadeState::enFadeState_Covered;

			/** 予約された切り替え処理があれば実行して、そのまま明転する */
			if (m_onCovered) {
				auto callback = std::move(m_onCovered);
				m_onCovered = nullptr;
				callback();
				StartFadeIn(m_inDuration);
			}
		}
	}
	else if (m_state == EnFadeState::enFadeState_FadingIn)
	{
		m_alpha -= m_speed * deltaTime;
		if (m_alpha <= 0.0f) {
			m_alpha = 0.0f;
			m_state = EnFadeState::enFadeState_None;
		}
	}

	m_sprite.SetMulColor(Vector4(m_color.x, m_color.y, m_color.z, m_alpha));
	m_sprite.Update();
}

void Fade::Render(RenderContext& rc)
{
	/** 完全に透明なら描かない */
	if (m_alpha <= 0.0f) {
		return;
	}
	m_sprite.Draw(rc);
}