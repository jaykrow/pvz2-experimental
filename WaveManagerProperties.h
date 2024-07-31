#pragma once
#include "PropertySheetBase.h"

class WaveManagerProperties : public PropertySheetBase
{
public:
	std::vector < std::vector<RtWeakPtr<void>>> Waves; // pointer to WaveActionProperties
	int FlagWaveInterval;
	std::vector<int> AdditionalFlagWaves;
	int WaveCount;
	bool SuppressFlagZombie;
	std::vector<int> FlagWaveVeteranOverrideTypes;
	std::vector<int> SuppressedDynamicZombieWaves;
	SexyString LevelJam;
	float MinNextWaveHealthPercentage;
	float MaxNextWaveHealthPercentage;
	float ZombieCountdownFirstWaveSecs;
	float ZombieCountdownFirstWaveConveyorSecs;
	float ZombieCountdownHugeWaveDelay;

	DEFINE_RT_CLASS_GET_CLASS_WRAPPER(0x568914);
};

static_assert(sizeof(WaveManagerProperties) == 0x78);

