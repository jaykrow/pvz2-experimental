#pragma once

#include "IntroModule.h"
#include "AnimationMgr.h"
#include "SeedChooser.h"
#include "SurvivalModuleProps.h"

enum SurvivalModuleState {
	levelintro = 0,
	transitionfromloading = 1,
	playing = 2,
	playingfinished = 3,
	panningtozombies = 4,
	pannedtozombies = 5,
	panningtoboard = 6,
	pannedtoboard = 7,
	resumepanning = 8,
	resumed = 9,
	initial = 10
};

// class SurvivalModule : public StandardLevelIntro
class SurvivalModule : public LevelModule
{
public:
	// static int waveCount;

	// RtWeakPtr<AnimationMgr> m_animationMgr;
	SurvivalModuleState m_state = initial;
	// RtWeakPtr<SeedChooser> m_seedChooser;
	int lastPausedAt = -1;
	int wavesBetweenChoosers = 1;
	int chooserCount = 0;
	int flagsBetweenChoosers = 1;
	int flagsCount = 0;
	bool invulnZombieFound = false;
	float invulnZombieTargetTime = MAXFLOAT;

	// bool overrideMusic = false;
	// SexyString musicType = "MiniGame_A";

	// static bool brah;
	// static bool firstseed;

	DECLARE_STATIC_RT_CLASS_MEMBERS(SurvivalModule);

	RT_CLASS_BUILD_SYMBOLS_BEGIN(LevelModule)
		REGISTER_INT_PROPERTY(builder, rclass, SurvivalModule, m_state);
		REGISTER_STANDARD_PROPERTY(builder, rclass, SurvivalModule, lastPausedAt);
		REGISTER_STANDARD_PROPERTY(builder, rclass, SurvivalModule, wavesBetweenChoosers);
		REGISTER_STANDARD_PROPERTY(builder, rclass, SurvivalModule, chooserCount);
		REGISTER_STANDARD_PROPERTY(builder, rclass, SurvivalModule, flagsCount);
		REGISTER_STANDARD_PROPERTY(builder, rclass, SurvivalModule, invulnZombieFound);
		REGISTER_STANDARD_PROPERTY(builder, rclass, SurvivalModule, invulnZombieTargetTime);

		buildEventCallbacks(builder, rclass);
	RT_CLASS_BUILD_SYMBOLS_END();

	static void* construct() {
		((void)__android_log_print(ANDROID_LOG_INFO, "PVZ2ExampleMod", "Constructing ""SurvivalModule")); 
		auto* instance = new SurvivalModule(); 
		typedef void* (*ctorWithThisPtr)(void*); 
		// ctorWithThisPtr baseCtor = (ctorWithThisPtr)GetActualOffset(0xA29B5C); 
		ctorWithThisPtr baseCtor = (ctorWithThisPtr)GetActualOffset(0x54B4BC); 
		baseCtor(instance); 

		// instance->unkEffectPtr = RtWeakPtr<StandaloneEffect>();
		// instance->unk2 = 0;

		setVFTable(instance, (uintptr_t)vftable); 
		return instance;
	};

	// DEFINE_RT_CLASS_GET_CLASS_FUNCTION(SurvivalModule, STANDARDLEVELINTRO_GETCLASS);
	DEFINE_RT_CLASS_GET_CLASS_FUNCTION(SurvivalModule, LEVELMODULE_GETCLASS_ADDR);

	DEFINE_RT_CLASS_REGISTER_CLASS_FUNCTION(SurvivalModule);

	static void RegisterCallbacks(SurvivalModule* self);

	static void buildEventCallbacks(Reflection::CRefManualSymbolBuilder* builder, void* rclass);

	void ChangeState(SurvivalModuleState newState);

	SurvivalModuleProps* GetProps();

	static SurvivalModule* GetInstance();

	static void* Destroy(SurvivalModule* self);

	static void DestroyAndDealloc(SurvivalModule* self) {
		auto ptr = Destroy(self);
		operator delete(ptr);
	}
};

class CowboyMinigameModule : public LevelModule {
public:
	int m_gameMode;
	int m_state;
	RtWeakPtr<AnimationMgr> m_animationMgr;
	std::vector<RtWeakPtr<void>> zombieGates;
	float m_delayStart;
};
