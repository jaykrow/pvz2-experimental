#pragma once

#include "GameSubsystem.h"
#include "Effect_FrogAnim.h"
#include "PlantGroup.h"
#include "TStdVectorManipulator.h"
#include "assert.h"

struct Frog {
	RtWeakPtr<Effect_FrogAnim> effect;
	std::vector<RtWeakPtr<Plant>> plants;
	SexyVector3 coords;
	bool firstIdle;
	bool markedForRemoval;
	bool removed;
	bool leftLawn;

	Frog() { };

	void ReInit(RtWeakPtr<Effect_FrogAnim>* frogEffect, RtWeakPtr<PlantGroup>* plantGroup, SexyVector3* coords) {
		firstIdle = true;
		markedForRemoval = false;
		removed = false;
		leftLawn = false;
		this->coords = *coords;

		effect.FromOther(frogEffect);

		RtWeakPtr<PlantGroup> local = RtWeakPtr<PlantGroup>();
		local.FromOther(plantGroup);
		PlantGroup* group = local.get();

		assert(plants.empty());
		for (auto& plantPtr : group->m_plants) {
			plants.emplace_back(&plantPtr);
		}
	}

	void DestroyEffect() {
		plants.clear();
		effect.~RtWeakPtr();
		effect = RtWeakPtr<Effect_FrogAnim>();
	}

	Frog(RtWeakPtr<Effect_FrogAnim>* frogEffect, RtWeakPtr<PlantGroup>* plantGroup, SexyVector3* coords) : 
		coords(*coords), firstIdle(true), markedForRemoval(false), removed(false), leftLawn(false) {
		effect = RtWeakPtr<Effect_FrogAnim>();
		effect.FromOther(frogEffect);

		RtWeakPtr<PlantGroup> local = RtWeakPtr<PlantGroup>();
		local.FromOther(plantGroup);
		PlantGroup* group = local.get();
		
		for (auto& plantPtr : group->m_plants) {
			plants.emplace_back(&plantPtr);
		}
	};

	RT_CLASS_BUILD_SYMBOLS() {
		LOGI("Building symbols for Frog");
		
		REGISTER_CLASS_RTWEAKPTR_PROPERTY(builder, rclass, Frog, effect, Effect_FrogAnim);
		REGISTER_RTWEAKPTR_VECTOR_PROPERTY(builder, rclass, Frog, plants, Plant);
		REGISTER_CLASS_PROPERTY(builder, rclass, Frog, SexyVector3, coords);

		REGISTER_STANDARD_PROPERTY(builder, rclass, Frog, firstIdle);
		REGISTER_STANDARD_PROPERTY(builder, rclass, Frog, markedForRemoval);
		REGISTER_STANDARD_PROPERTY(builder, rclass, Frog, removed);
		REGISTER_STANDARD_PROPERTY(builder, rclass, Frog, leftLawn);

		LOGI("Frog symbols built");
	}
};

// building this off the base GameSubsystem will crash for some reason when GetRTClass is ran in ModInit
// I've tried workarounds but this worked
class FrogJumpSubsystem : public ZombieChickenSubsystem
{
public:
	std::vector<Frog> m_frogs;

	DECLARE_STATIC_RT_CLASS_MEMBERS(FrogJumpSubsystem)
	static bool checkIgnoreFrogs;

	RT_CLASS_BUILD_SYMBOLS() {
		LOGI("Building symbols for FrogJumpSubsystem");
		RtClass* rtClass = GetRTClass();
		rtClass->SetRClass(rclass);
		rclass->m_rtclass = rtClass;

		Reflection::RClassRef* parentRef = builder->GetClassRef("ZombieChickenSubsystem");
		Reflection::RClass* parent = parentRef->GetRClass();

		builder->SetRClassWithParent(rclass, parent, 0);

		REGISTER_CLASS_VECTOR_PROPERTY(builder, rclass, FrogJumpSubsystem, m_frogs, Frog);

		LOGI("FrogJumpSubsystem symbols built");
	}

	static void* construct() {
		((void)__android_log_print(ANDROID_LOG_INFO, "PVZ2ExampleMod", "Constructing ""FrogJumpSubsystem")); 

		auto* instance = new FrogJumpSubsystem(); 
		typedef void* (*ctorWithThisPtr)(void*); 
		ctorWithThisPtr baseCtor = (ctorWithThisPtr)GetActualOffset(0x54B4BC);
		
		baseCtor(instance);
		memset(&instance->m_chickenings, 0, sizeof(0xC));
		memset(&instance->m_frogs, 0, sizeof(0xC));

		setVFTable(instance, (uintptr_t)vftable); 
		return instance;
	};

	DEFINE_RT_CLASS_GET_CLASS_FUNCTION(FrogJumpSubsystem, 0x340A84);

	static void registerClass() {
		typedef void* (*func12280C8)();
		func12280C8 func_0x12280C8 = (func12280C8)GetActualOffset(0x12280C8);
		void* v0 = (void*)func_0x12280C8();
		if (v0) {
			typedef void* (*func122811C)(void*); 
			func122811C func_0x122811C = (func122811C)GetActualOffset(0x122811C); 
			auto* builder = (Reflection::CRefManualSymbolBuilder*)func_0x122811C(v0); 
			if (builder) {
				(builder)->RegisterClassWithProperties("Frog", Frog::buildSymbols, sizeof(Frog), 0);
				(builder)->RegisterClassWithProperties("FrogJumpSubsystem", FrogJumpSubsystem::buildSymbols, sizeof(FrogJumpSubsystem), 0);
			}
		}
	};

	void AddFrog(RtWeakPtr<Effect_FrogAnim>* frog, RtWeakPtr<PlantGroup>* plantGroup, SexyVector3* coords);
	void MarkFrogForRemoval(RtWeakPtr<Effect_FrogAnim>* frog);

	static void* Destroy(FrogJumpSubsystem*);
	static void DestroyAndDealloc(FrogJumpSubsystem*);
};

static_assert(offsetof(FrogJumpSubsystem, m_frogs) == 32);

