#pragma once

#include "LevelModuleProperties.h"
#include "ReflectionBuilder.h"

class SurvivalModuleProps : public LevelModuleProperties
{
public:
	int WavesBetweenChoosers = 3;
	bool IsEndless = false;
	int FlagsBetweenChoosers = 1; // Only need to be set for IsEndless = true

	DECLARE_STATIC_RT_CLASS_MEMBERS(SurvivalModuleProps);

	RT_CLASS_BUILD_SYMBOLS_BEGIN(LevelModuleProperties)
		REGISTER_STANDARD_PROPERTY(builder, rclass, SurvivalModuleProps, WavesBetweenChoosers);
		REGISTER_STANDARD_PROPERTY(builder, rclass, SurvivalModuleProps, IsEndless);
		REGISTER_STANDARD_PROPERTY(builder, rclass, SurvivalModuleProps, FlagsBetweenChoosers);
	RT_CLASS_BUILD_SYMBOLS_END();

	// DEFINE_RT_CLASS_BUILD_SYMBOLS_WITHOUT_CLASS_PROPERTIES(LevelModuleProperties);

	DEFINE_RT_CLASS_CONSTRUCT_FUNCTION(SurvivalModuleProps, 0x54B4BC);

	DEFINE_RT_CLASS_GET_CLASS_FUNCTION(SurvivalModuleProps, LEVELMODULEPROPS_GETCLASS_ADDR);

	DEFINE_RT_CLASS_REGISTER_CLASS_FUNCTION(SurvivalModuleProps);

	static void* GetModuleType();

	static void* Destroy(SurvivalModuleProps* self);
	static void DestroyAndDealloc(SurvivalModuleProps* self) {
		auto destroyed = Destroy(self);
		operator delete(destroyed);
	}
};
