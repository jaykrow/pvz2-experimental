#pragma once

#include "LevelModuleProperties.h"
#include "ReflectionBuilder.h"

class SkyModuleProps : public LevelModuleProperties
{
	DECLARE_STATIC_RT_CLASS_MEMBERS(SkyModuleProps);

	DEFINE_RT_CLASS_BUILD_SYMBOLS_WITHOUT_CLASS_PROPERTIES(LevelModuleProperties);

	DEFINE_RT_CLASS_CONSTRUCT_FUNCTION(SkyModuleProps, 0x54B4BC);

	DEFINE_RT_CLASS_GET_CLASS_FUNCTION(SkyModuleProps, LEVELMODULEPROPS_GETCLASS_ADDR);

	DEFINE_RT_CLASS_REGISTER_CLASS_FUNCTION(SkyModuleProps);

	static void* GetModuleType();
};

