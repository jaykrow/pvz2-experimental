#pragma once

#include "WaveManagerModuleProperties.h"
#include "ReflectionBuilder.h"
#include "ZombieType.h"
#include "TStdVectorManipulator.h"

class WaveManagerEndlessModuleProps : public WaveManagerModuleProperties
{
public:
	int StartingPoints = 100;
	int FlagsPerChooser;
	int WavesPerFlag;
	int PointIncrementPerWave;
	int MaxPoints;
	std::vector<RtWeakPtr<ZombieType>> ZombiePool;

	DECLARE_STATIC_RT_CLASS_MEMBERS(WaveManagerEndlessModuleProps);

	// DEFINE_RT_CLASS_BUILD_SYMBOLS_WITHOUT_CLASS_PROPERTIES(WaveManagerModuleProperties);
	RT_CLASS_BUILD_SYMBOLS_BEGIN(WaveManagerModuleProperties)
		REGISTER_STANDARD_PROPERTY(builder, rclass, WaveManagerEndlessModuleProps, StartingPoints);
		REGISTER_STANDARD_PROPERTY(builder, rclass, WaveManagerEndlessModuleProps, FlagsPerChooser);
		REGISTER_STANDARD_PROPERTY(builder, rclass, WaveManagerEndlessModuleProps, WavesPerFlag);
		REGISTER_STANDARD_PROPERTY(builder, rclass, WaveManagerEndlessModuleProps, PointIncrementPerWave);
		REGISTER_STANDARD_PROPERTY(builder, rclass, WaveManagerEndlessModuleProps, MaxPoints);
		REGISTER_RTWEAKPTR_VECTOR_PROPERTY(builder, rclass, WaveManagerEndlessModuleProps, ZombiePool, ZombieType);
	RT_CLASS_BUILD_SYMBOLS_END()

	DEFINE_RT_CLASS_CONSTRUCT_FUNCTION(WaveManagerEndlessModuleProps, WAVEMANAGERMODULEPROPS_CONSTRUCT);

	DEFINE_RT_CLASS_GET_CLASS_FUNCTION(WaveManagerEndlessModuleProps, WAVEMANAGERMODULEPROPS_GETCLASS);

	DEFINE_RT_CLASS_REGISTER_CLASS_FUNCTION(WaveManagerEndlessModuleProps);

	static void* GetModuleType();
};

