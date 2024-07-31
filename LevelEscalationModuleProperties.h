#pragma once
#include "LevelModuleProperties.h"
#include "WaveManagerProperties.h"

class LevelEscalationModuleProperties : public LevelModuleProperties
{
public:
	char pad048[8];
	RtWeakPtr<WaveManagerProperties> WaveManagerProps;
	std::vector<RtWeakPtr<void>> ZombiePool;
	int FlagCount;
	int WavesPerFlag;
	int PlantfoodToSpawnCount;
	int PointIncrementPerWave;
	int StartingPoints;
	int MaxPoints;
};

static_assert(offsetof(LevelEscalationModuleProperties, ZombiePool) == 0x58);

