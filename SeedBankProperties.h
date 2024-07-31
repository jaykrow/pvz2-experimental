#pragma once
#include "LevelModuleProperties.h"

class PresetPlantEntry {
public:
	SexyString plantType;
	int level;
};

class PresetPlantCostumeEntry {
public:
	SexyString plantType;
	int costumeID;
};

class SeedBankProperties : public LevelModuleProperties
{
public:
	int unk;
	int SelectionMethod;
	bool AlwaysUseChooser;
	std::vector<PresetPlantEntry> PresetPlantList;
	std::vector<PresetPlantCostumeEntry> PresetPlantCostumeList;
	std::vector<SexyString> BeghouledUpgradeList;
	std::vector<std::vector<SexyString>> SuggestPlantList;
	std::vector<SexyString> PlantIncludeList;
	std::vector<SexyString> PlantExcludeList;
	SexyString ExcludeListLiftedBy;
	bool ExcludeListSunProducers;
	int OverrideSeedSlotsCount;
};

static_assert(sizeof(SeedBankProperties) == 0xB0);
static_assert(offsetof(SeedBankProperties, PlantIncludeList) == 0x84);