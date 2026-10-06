#pragma once
/**
 * Paper.h
 * 紙クラス
 * ここで紙を管理する。
 * 暗闇の中で、プレイヤーが近づくと看板の文字が浮かび上がる。
 */
class Player;
class Paper : public IGameObject
{
public:
	Paper();
	~Paper();
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

	/** ライトを照らしている時の紙の画像(これだと数字は浮かばない) */
	SpriteRender m_litPaperSprite;

	/** ライトを照らしていない時の紙の画像(数字が浮かぶ) */
	SpriteRender m_unlitPaperSprite;

	/** 操作案内フォント */
	FontRender m_introductionFont;

	/** プレイヤー */
	Player* m_player = nullptr;

	/** 紙を調べている最中か */
	bool m_isViewing = false;

	/** 案内文字を出すか */
	bool m_isPromptVisible = false;
};

