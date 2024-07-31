#pragma once

#include "WaveManagerModule.h"
#include "LevelModuleProperties.h"
#include "ZombieType.h"

class WaveGenerationData {
public:
	int id; // ID 0 defines zombieTypes, else it's a wave number
	std::vector<ZombieType*> zombieTypes;
	std::vector<int> unkVec2; // completely unused I believe
	int plantfoodCount;
	bool suppressDynamicZombies;
};

static_assert(sizeof(WaveGenerationData) == 0x24);

class WaveManagerGenerationData {
public:
	int unk; // 0
	int TotalWaves;
	int WavesPerFlag;
	int unk4; // 0 // these form a vector
	int unk5; // 0
	int unk6; // 0
	int StartingPoints;
	int PointIncrementPerWave;
	time_t RandSeed;
	int MaxPoints;
	std::vector<WaveGenerationData> GenerationData;

	WaveGenerationData* GetOrCreate(int id) {
		typedef WaveGenerationData* (*func)(WaveManagerGenerationData*, int);
		return ((func)GetActualOffset(0x56C8B8))(this, id);
	}
};

static_assert(sizeof(WaveManagerGenerationData) == 0x34);

class WaveManagerEndlessModule : public WaveManagerModule
{
public:
	int m_currentPoints;

	DECLARE_STATIC_RT_CLASS_MEMBERS(WaveManagerEndlessModule);

	// DEFINE_RT_CLASS_BUILD_SYMBOLS_WITHOUT_CLASS_PROPERTIES(WaveManagerModule);
	RT_CLASS_BUILD_SYMBOLS_BEGIN(WaveManagerModule)
		REGISTER_STANDARD_PROPERTY(builder, rclass, WaveManagerEndlessModule, m_currentPoints);
	RT_CLASS_BUILD_SYMBOLS_END();

	static void* construct() {
		((void)__android_log_print(ANDROID_LOG_INFO, "PVZ2ExampleMod", "Constructing ""WaveManagerEndlessModule")); 
		auto* instance = new WaveManagerEndlessModule(); 
		typedef void* (*ctorWithThisPtr)(void*); 
		ctorWithThisPtr baseCtor = (ctorWithThisPtr)GetActualOffset(0x54B4BC); 
		baseCtor(instance); 

		instance->propSheetPtr = RtWeakPtr<LevelModuleProperties>();

		typedef void (*waveManagerCtor)(WaveManager*);
		((waveManagerCtor)getActualOffset(WAVEMANAGER_CONSTRUCT_ADDR))(&instance->waveManager);

		setVFTable(instance, (uintptr_t)vftable); 
		return instance;
	};

	DEFINE_RT_CLASS_GET_CLASS_FUNCTION(WaveManagerEndlessModule, WAVEMANAGERMODULE_GETCLASS_ADDR);

	DEFINE_RT_CLASS_REGISTER_CLASS_FUNCTION(WaveManagerEndlessModule);

	static void RegisterCallbacks(WaveManagerEndlessModule*);
};

