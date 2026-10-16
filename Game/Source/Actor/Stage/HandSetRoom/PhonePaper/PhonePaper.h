#pragma once
/**
 * PhonePaper.h
 * 電話の紙クラス
 * ここで電話の紙を管理する。
 * プレイヤーが近づくと操作案内を表示し、調べると紙の画像を表示する。
 */
class Player;

class PhonePaper : public IGameObject
{
public:
	PhonePaper();
	~PhonePaper();

	bool Start();
	void Update();
	void Render(RenderContext& rc);

private:
	/** プレイヤーとの距離を計算する */
	float CalculateDistanceToPlayer(const Vector3& playerPosition) const;

	/** プレイヤーがインタラクトの範囲内かどうかを判定する */
	bool IsPlayerInRange() const;

	/** インタラクト処理 */
	void Interact();

private:
	/** 紙モデル */
	ModelRender m_paperModel;

	/** 紙の画像 */
	SpriteRender m_paperSprite;

	/** 操作案内フォント */
	FontRender m_introductionFont;

	/** プレイヤー */
	Player* m_player = nullptr;

	/** インタラクト時のSE */
	SoundSource* m_interactSE;

	/** 紙を調べている最中か */
	bool m_isViewing = false;

	/** 案内文字を出すか */
	bool m_isPromptVisible = false;
};