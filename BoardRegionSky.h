#pragma once

#include "BoardRegion.h"

class BoardRegionSky : public BoardRegion
{
	DECLARE_STATIC_RT_CLASS_MEMBERS(BoardRegionSky);

	DEFINE_RT_CLASS_BUILD_SYMBOLS_WITHOUT_CLASS_PROPERTIES(BoardRegion);

	static void* construct() {
		((void)__android_log_print(ANDROID_LOG_INFO, "PVZ2ExampleMod", "Constructing ""BoardRegionSky")); 
		auto* instance = new BoardRegionSky(); 
		typedef void* (*ctorWithThisPtr)(void*); 
		ctorWithThisPtr baseCtor = (ctorWithThisPtr)GetActualOffset(BOARDREGION_CONSTRUCT_ADDR); 
		baseCtor(instance); 
		setVFTable(instance, (uintptr_t)vftable);
		instance->SetRegionFlag(region_sky);

		return instance;
	};

	DEFINE_RT_CLASS_GET_CLASS_FUNCTION(BoardRegionSky, BOARDREGION_GETCLASS_ADDR);

	DEFINE_RT_CLASS_REGISTER_CLASS_FUNCTION(BoardRegionSky);
};

bool SkyExistsAtPosition(SexyVector3* position);

