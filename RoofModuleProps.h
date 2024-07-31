#pragma once
#include "LevelModuleProperties.h"
#include "TStdVectorManipulator.h"
#include "Projectile.h"

struct SlopedEffect {
	SexyString Name;
	int OffsetX;
	int OffsetY;

	RT_CLASS_BUILD_SYMBOLS() {
		REGISTER_STRING_PROPERTY(builder, rclass, SlopedEffect, Name);
		REGISTER_INT_PROPERTY(builder, rclass, SlopedEffect, OffsetX);
		REGISTER_INT_PROPERTY(builder, rclass, SlopedEffect, OffsetY);
	}
};

struct KillTolerance {
	RtWeakPtr<ProjectilePropertySheet> Projectile;
	int ExtraHeight;

	RT_CLASS_BUILD_SYMBOLS() {
		REGISTER_CLASS_RTWEAKPTR_PROPERTY(builder, rclass, KillTolerance, Projectile, ProjectilePropertySheet);
		REGISTER_STANDARD_PROPERTY(builder, rclass, KillTolerance, ExtraHeight);
	}
};

class RoofModuleProps : public LevelModuleProperties
{
public:
	std::vector<SlopedEffect> SlopedEffects;
	std::vector<RtWeakPtr<ProjectilePropertySheet>> SpawnIgnoresSlope;
	std::vector<RtWeakPtr<ProjectilePropertySheet>> FollowsSlope;
	std::vector<KillTolerance> KillTolerances;
	std::vector<RtWeakPtr<ProjectilePropertySheet>> ImmuneToSlope;

	DECLARE_STATIC_RT_CLASS_MEMBERS(RoofModuleProps);

	RT_CLASS_BUILD_SYMBOLS_BEGIN(LevelModuleProperties)
		REGISTER_CLASS_VECTOR_PROPERTY(builder, rclass, RoofModuleProps, SlopedEffects, SlopedEffect);
		REGISTER_RTWEAKPTR_VECTOR_PROPERTY(builder, rclass, RoofModuleProps, SpawnIgnoresSlope, ProjectilePropertySheet);
		REGISTER_RTWEAKPTR_VECTOR_PROPERTY(builder, rclass, RoofModuleProps, FollowsSlope, ProjectilePropertySheet);
		REGISTER_RTWEAKPTR_VECTOR_PROPERTY(builder, rclass, RoofModuleProps, ImmuneToSlope, ProjectilePropertySheet);
		REGISTER_CLASS_VECTOR_PROPERTY(builder, rclass, RoofModuleProps, KillTolerances, KillTolerance);
	RT_CLASS_BUILD_SYMBOLS_END();

	static void* construct() {
		((void)__android_log_print(ANDROID_LOG_INFO, "PVZ2ExampleMod", "Constructing ""RoofModuleProps")); 
		auto* instance = new RoofModuleProps(); 
		typedef void* (*ctorWithThisPtr)(void*);
		ctorWithThisPtr baseCtor = (ctorWithThisPtr)GetActualOffset(0x54B4BC);

		instance->SlopedEffects.clear();
		instance->SlopedEffects.shrink_to_fit();

		instance->SpawnIgnoresSlope.clear();
		instance->SpawnIgnoresSlope.shrink_to_fit();

		instance->FollowsSlope.clear();
		instance->FollowsSlope.shrink_to_fit();

		instance->KillTolerances.clear();
		instance->KillTolerances.shrink_to_fit();

		instance->ImmuneToSlope.clear();
		instance->ImmuneToSlope.shrink_to_fit();

		baseCtor(instance); 
		setVFTable(instance, (uintptr_t)vftable); 
		return instance;
	};
	
	DEFINE_RT_CLASS_GET_CLASS_FUNCTION(RoofModuleProps, LEVELMODULEPROPS_GETCLASS_ADDR);

	static void registerClass() {
		typedef void* (*func12280C8)(); 
		func12280C8 func_0x12280C8 = (func12280C8)GetActualOffset(0x12280C8); 
		void* v0 = (void*)func_0x12280C8(); 
		if (v0) {
			typedef void* (*func122811C)(void*); 
			func122811C func_0x122811C = (func122811C)GetActualOffset(0x122811C); 
			auto* builder = (Reflection::CRefManualSymbolBuilder*)func_0x122811C(v0);
			if (builder) {
				(builder)->RegisterClassWithProperties("SlopedEffect", SlopedEffect::buildSymbols, sizeof(SlopedEffect), 0);
				(builder)->RegisterClassWithProperties("KillTolerance", KillTolerance::buildSymbols, sizeof(KillTolerance), 0);
				(builder)->RegisterClassWithProperties("RoofModuleProps", RoofModuleProps::buildSymbols, sizeof(RoofModuleProps), 0);;
			}
		}
	};

	static void* GetModuleType();
};

