#pragma once
#include <functional>
/** 
 * Fade.h 
 * 画面の暗転や明転を管理するクラス
 * シーンをまたいで1つだけ生成し、FindGOで探して使う
 */
class Fade : public IGameObject
{
public:
	Fade();
	~Fade();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	/** 画面を色で覆っていく durationは秒になる */
	void StartFadeOut(const Vector3& color, float duration);

	/** 色を消して、画面を見えるようにする durationは秒 */
	void StartFadeIn(float duration);

	/** 画面が完全に覆われているか */
	bool IsFullyCovered() const
	{
		return m_state == EnFadeState::enFadeState_Covered;
	}

	/** 動きが終わっているか(何もしていない、または覆い切った) */
	bool IsFinished() const
	{
		return m_state == EnFadeState::enFadeState_None
			|| m_state == EnFadeState::enFadeState_Covered;
	}

	/** 暗転→onCoveredを実行→明転、を1回で行う */
	void StartTransition(const Vector3& color, float outDuration, float inDuration,
		std::function<void()> onCovered);

	/** フェード中か(何もしていない時以外はtrue)。入力を止める判断に使う */
	bool IsBusy() const
	{
		return m_state != EnFadeState::enFadeState_None;
	}

private:
	enum class EnFadeState
	{
		enFadeState_None,		//!< 何もしていない(画面は見えている)
		enFadeState_FadingOut,	//!< 覆っている最中
		enFadeState_Covered,	//!< 完全に覆われている
		enFadeState_FadingIn,	//!< 消している最中
	};

private:
	/** 画面全体を覆う画像 */
	SpriteRender m_sprite;

	/** フェードの状態 */
	EnFadeState m_state = EnFadeState::enFadeState_None;

	/** 覆う色(RGB) */
	Vector3 m_color = Vector3(0.0f, 0.0f, 0.0f);

	/** 透明度(0.0で見える、1.0で完全に覆う) */
	float m_alpha = 0.0f;

	/** 1秒あたりに変わる透明度の量 */
	float m_speed = 0.0f;

	/** 完全に覆われた時に1回だけ実行する処理 */
	std::function<void()> m_onCovered;

	/** 切り替え後の明転にかける時間 */
	float m_inDuration = 0.0f;
};

