#pragma once
#include "DamageTypeFlags.h"
#include "ZombieConditions.h"
#include <vector>

struct __attribute__((packed)) DamageInfo
	//struct DamageInfo
{
public:
	DamageInfo()
	{
		memset(this, 0, sizeof(DamageInfo));
	}

	~DamageInfo()
	{
		overrideStunConditions.clear();
		unkVec2.clear();
		unkVec3.clear();
		plantFamilies.clear();
	}

	int attacker; //0x0000
	float damage; //0x0004
	DamageTypeFlags flags; //0x0008
	std::vector<ZombieConditions> overrideStunConditions;
	std::vector<int> unkVec2;
	std::vector<int> unkVec3;
	std::vector<SexyString> plantFamilies;
	int unk;
}; //Size: 0x0044
static_assert(sizeof(DamageInfo) == 0x44);
