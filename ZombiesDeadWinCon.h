#pragma once
#include "LevelModule.h"

class ZombiesDeadWinCon : public LevelModule
{
	float m_invulnZombieTargetTime;
	bool m_invulnZombieFound;
};

static_assert(sizeof(ZombiesDeadWinCon) == 0x20);

