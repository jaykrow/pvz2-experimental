#pragma once
#include "LevelModule.h"

class DummyModule : public LevelModule 
{
	DECLARE_STATIC_RT_CLASS_MEMBERS(DummyModule);

	DEFINE_RT_CLASS_BUILD_SYMBOLS_WITHOUT_CLASS_PROPERTIES(LevelModule);
	/*RT_CLASS_BUILD_SYMBOLS_BEGIN(LevelModule)
	RT_CLASS_BUILD_SYMBOLS_END()*/

	DEFINE_RT_CLASS_CONSTRUCT_FUNCTION(DummyModule, 0x54B4BC);

	DEFINE_RT_CLASS_GET_CLASS_FUNCTION(DummyModule, LEVELMODULE_GETCLASS_ADDR);

	DEFINE_RT_CLASS_REGISTER_CLASS_FUNCTION(DummyModule);

	// static void buildEventCallbacks(Reflection::CRefManualSymbolBuilder* builder, void* rclass);
};

