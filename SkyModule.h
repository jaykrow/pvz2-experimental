#pragma once

#include "LevelModule.h"
#include "ReflectionBuilder.h"

class SkyModule : public LevelModule
{
	DECLARE_STATIC_RT_CLASS_MEMBERS(SkyModule);

	DEFINE_RT_CLASS_BUILD_SYMBOLS_WITHOUT_CLASS_PROPERTIES(LevelModule);

	DEFINE_RT_CLASS_CONSTRUCT_FUNCTION(SkyModule, 0x54B4BC);

	DEFINE_RT_CLASS_GET_CLASS_FUNCTION(SkyModule, LEVELMODULE_GETCLASS_ADDR);

	DEFINE_RT_CLASS_REGISTER_CLASS_FUNCTION(SkyModule);

	static void RegisterCallbacks(SkyModule *self);
};

