#pragma once

#include "Zombie.h"
#include "ReflectionBuilder.h"

enum DogfighterState {
	shooting = 0x10,
	flying_back = 0x11
};

class ZombieDogfighter : public Zombie
{
	DECLARE_STATIC_RT_CLASS_MEMBERS(ZombieDogfighter);

	RT_CLASS_BUILD_SYMBOLS_BEGIN(Zombie)
		buildEventCallbacks(builder, rclass);
	RT_CLASS_BUILD_SYMBOLS_END();

	DEFINE_RT_CLASS_CONSTRUCT_FUNCTION(ZombieDogfighter, ZOMBIE_CTOR_ADDR);

	DEFINE_RT_CLASS_GET_CLASS_FUNCTION(ZombieDogfighter, ZOMBIE_GET_RTCLASS);

	DEFINE_RT_CLASS_REGISTER_CLASS_AND_STATES_FUNCTION(ZombieDogfighter);

	static void buildEventCallbacks(Reflection::CRefManualSymbolBuilder* builder, void* rtClass);
	static void buildStates();
};

