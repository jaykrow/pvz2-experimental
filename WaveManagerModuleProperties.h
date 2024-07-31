#pragma once

#include "LevelModuleProperties.h"
#include "WaveManagerProperties.h"

#define WAVEMANAGERMODULEPROPS_GETCLASS 0x569BC8
#define WAVEMANAGERMODULEPROPS_CONSTRUCT 0x569E0C
#define WAVEMANAGERMODULEPROPS_VTBL 0x01BD1FC8
#define WAVEMANAGERMODULEPROPS_VFUNCS 18

class DynamicZombiesInstruction {
	int StartingPoints;
	int PointIncrementPerWave;
	int StartingWave;
	int MaxPoints;
	std::vector<RtWeakPtr<void>> ZombiePool;
};

class WaveManagerModuleProperties : public LevelModuleProperties
{
	char pad048[8];
	RtWeakPtr<WaveManagerProperties> WaveManagerProps;
	bool ManualStartup;
	std::vector<DynamicZombiesInstruction> DynamicZombies;
};

static_assert(sizeof(WaveManagerModuleProperties) == 0x68);
