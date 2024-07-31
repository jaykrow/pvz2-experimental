#pragma once

#include "PropertySheetBase.h"

class DangerRoomLevelDesigner : public PropertySheetBase { };

class DangerRoomModernDesigner : public DangerRoomLevelDesigner
{
public:
	ValueRange LevelRange;
	ValueRange FirstPortalWavesMin;
	ValueRange FirstPortalWavesMax;
	ValueRange NextPortalWavesMin;
	ValueRange NextPortalWavesMax;
	ValueRange PortalTypesMin;
	ValueRange PortalTypesMax;
	ValueRange PortalCountInWaveMin;
	ValueRange PortalCountInWaveMax;
	ValueRange ColumnToSpawnMin;
	ValueRange ColumnToSpawnMax;
	ValueRange ZombiesToSpawnMin;
	ValueRange ZombiesToSpawnMax;
	ValueRange ZombieRandomListStartMin;
	ValueRange ZombieRandomListStartMax;
	ValueRange ZombieRandomListEndMin;
	ValueRange ZombieRandomListEndMax;
	std::vector<StringWeightedOption> PortalTypePool;
};

static_assert(sizeof(DangerRoomModernDesigner) == 0xB0);
static_assert(offsetof(DangerRoomModernDesigner, LevelRange) == 0x1C);
static_assert(offsetof(DangerRoomModernDesigner, PortalTypesMax) == 0x4C);