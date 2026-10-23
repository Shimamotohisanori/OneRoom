#include "stdafx.h"
#include "Key.h"

namespace
{
	/** キーの初期数 */
	constexpr int INITIAL_KEY_COUNT = 3;
}

Key::Key()
{}

Key::~Key()
{}

bool Key::Start()
{
	m_keyCount = INITIAL_KEY_COUNT;
	return true;
}

void Key::Update()
{}
