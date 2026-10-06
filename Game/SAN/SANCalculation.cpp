#include "stdafx.h"
#include "SANCalculation.h"
namespace
{
	/** SAN値の初期値 */
	constexpr int SAN_VALUE_INITIAL = 100;

	/** SAN値の最大値 */
	constexpr int SAN_VALUE_MAX = 100;

	/** SAN値の最小値 */
	constexpr int SAN_VALUE_MIN = 0;
}

SANCalculation::~SANCalculation()
{}

bool SANCalculation::Start()
{
	m_sanValue = SAN_VALUE_INITIAL;
	return true;
}

void SANCalculation::Update()
{
	/** SAN値を計算する */
	CalculateSAN();
}

void SANCalculation::CalculateSAN()
{
	/** SAN値の範囲を0～100に制限する */
	if (m_sanValue < SAN_VALUE_MIN)
	{
		m_sanValue = SAN_VALUE_MIN;
	}
	else if (m_sanValue > SAN_VALUE_MAX)
	{
		m_sanValue = SAN_VALUE_MAX;
	}

	//NOTE: 今後、SAN値の計算式を実装する。
	if (g_pad[0]->IsPress(enButtonB))
	{
		m_sanValue -= 1;
	}
	else if (g_pad[0]->IsPress(enButtonA))
	{
		m_sanValue += 1;
	}

}
