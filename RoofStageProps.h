#pragma once
#include "StageModuleProperties.h"
#include "TStdVectorManipulator.h"

class RoofStageProps : public StageModuleProperties
{
public:
	int unk;
	std::vector<int> ColumnHeights;
	std::vector<int> KillHeights;

	DECLARE_STATIC_RT_CLASS_MEMBERS(RoofStageProps);

	RT_CLASS_BUILD_SYMBOLS_BEGIN(StageModuleProperties)
		REGISTER_STANDARD_VECTOR_PROPERTY(builder, rclass, RoofStageProps, ColumnHeights, int);

		REGISTER_STANDARD_VECTOR_PROPERTY(builder, rclass, RoofStageProps, KillHeights, int);
	RT_CLASS_BUILD_SYMBOLS_END();

	static void* construct() {
		((void)__android_log_print(ANDROID_LOG_INFO, "PVZ2ExampleMod", "Constructing ""RoofStageProps")); 
		auto* instance = new RoofStageProps(); 
		typedef void* (*ctorWithThisPtr)(void*); 
		ctorWithThisPtr baseCtor = (ctorWithThisPtr)GetActualOffset(0x32C290);
		baseCtor(instance); 

		instance->ColumnHeights.clear();
		instance->KillHeights.clear();
		instance->ColumnHeights.shrink_to_fit();
		instance->KillHeights.shrink_to_fit();

		setVFTable(instance, (uintptr_t)vftable); 
		return instance;
	};

	DEFINE_RT_CLASS_GET_CLASS_FUNCTION(RoofStageProps, STAGEMODULEPROPS_GETCLASS_ADDR);

	DEFINE_RT_CLASS_REGISTER_CLASS_FUNCTION(RoofStageProps);

	static void* GetModuleType();
};

