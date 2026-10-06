#pragma once
/**
 * SANCalculation.h
 * SAN計算クラス
 * ここでSAN計算の操作や状態を管理する。
 */
class SANCalculation : public IGameObject
{
public:
	SANCalculation() {}
	~SANCalculation();
	bool Start();
	void Update();
	
	/** SAN値を増やす */
	void AddSANValue(int value)
	{
		m_sanValue += value;
	}

	/** SAN値を減らす */
	void SubtractSANValue(int value)
	{
		m_sanValue -= value;
	}

	/** SAN値を取得する */
	void SetSANValue(int value)
	{
		m_sanValue = value;
	}

	/** SAN値を取得する */
	int GetSANValue() const
	{
		return m_sanValue;
	}

private:
	/** SANを計算する */
	void CalculateSAN();

	/** SAN値 */
	int m_sanValue = 50;
};

