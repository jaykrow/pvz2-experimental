#pragma once
#include <vector>
#include "EntityConditionTracker.h"
#include "PlantConditions.h"

class PlantConditionTracker
{
public:
	std::vector<uint8_t> m_conditionFlags;
	std::vector<EntityCondition> m_conditions;
	char unk[32];
	int m_shaderOverrideColorRed;
	int m_shaderOverrideColorBlue;
	int m_shaderOverrideColorGreen;
	int m_shaderOverrideColorAlpha;
	int m_unkColorTrackerRed;
	int m_unkColorTrackerBlue;
	int m_unkColorTrackerGreen;
	int m_unkColorTrackerAlpha;
	int unk2;
	RtWeakPtr<void> unkPtr;

	bool HasCondition(PlantConditions condition) {
		return m_conditionFlags[condition] > 0;
	}

	virtual void Function0() {};
	virtual void Function1() {};
	virtual void Function2() {};
	virtual void Function3() {};
};

static_assert(sizeof(PlantConditionTracker) == 0x68);
