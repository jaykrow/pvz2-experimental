#pragma once

#include "LevelModuleProperties.h"

#define STAGEMODULEPROPS_GETCLASS_ADDR 0x4BD184
#define STAGEMODULEPROPS_VTBL 0x1BCD9B0
#define STAGEMODULEPROPS_VFUNCS 18
#define STAGEMODULEPROPS_CONSTRUCT_ADDR 0x32C290

class StageModuleProperties : public LevelModuleProperties
{
public:
	char pad0048[4];
	SexyString backgroundResourceGroup;
	SexyString backgroundImagePrefix;
	SexyString stagePrefix;
	SexyString basicZombieTypeName;
	SexyString flagZombieTypeName;
	std::vector<SexyString> flagVeteranZombieTypenames;
	SexyString armor1ZombieTypeName;
	SexyString armor2ZombieTypeName;
	SexyString armor4ZombieTypeName;
	SexyString musicSuffix;
	SexyString musicSuffixMinigame;
	SexyString ambientAudioSuffix;
	SexyString railcartDefaultTypeName;
	SexyString gravestoneDefaultTypeName;
	SexyString dirtSpawnEffectName;
	SexyString zombieEventAudio;
	SexyString levelPowerupSet;
	std::vector<Point> disabledStreetCells;
	SexyString progressFlagAsset;
	SexyString starChallengeUnlockLevel;
};

static_assert(sizeof(StageModuleProperties) == 0x13C);
static_assert(offsetof(StageModuleProperties, railcartDefaultTypeName) == 0xDC);

