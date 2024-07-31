#include "pch.h"
#include "PlantCustomConditions.h"
#include "memUtils.h"
#include "CRefManualSymbolBuilder.h"
#include "NamedFlagWithId.h"
#include "PvZ2/RtWeakPtr.h"
#include "PvZ2/Board.h"
#include "AnimationSequenceEntry.h"
#include "PlantGroup.h"
#include "Zombie.h"
#include "PlantConditions.h"
#include "Effect_FrogAnim.h"
#include "FrogJumpSubsystem.h"
#include "TimeMgr.h"

#define CONCATENATE_DETAIL(x, y) x##y
#define CONCATENATE(x, y) CONCATENATE_DETAIL(x, y)
#define FUNC_NAME(offset) CONCATENATE(sub, offset)

#define Sub(offset, ...) \
    typedef void (*FUNC_NAME(offset))(__VA_ARGS__); \
    ((FUNC_NAME(offset))getActualOffset(offset))

#define Sub2(offset, ret, a1, ...) \
    typedef ret (*FUNC_NAME(offset))(__VA_ARGS__); \
    ret a1 = ((FUNC_NAME(offset))getActualOffset(offset))

typedef int (*getConditionsCount)();

int hkGetConditionsCount() {
	// LOGI("Called in plant construction");
	return 0x1B; // original was 0x1A
}

typedef void (*wizardTurnTargetToSheep)(int, int);
wizardTurnTargetToSheep oWizardTurnTargetToSheep = NULL;

void hkWizardTurnTargetToSheep(int a1, PlantGroup* a2) {
	if (a2) {
		// LOGI("initial");
		Sub2(0x1069C40, int, v4, int)(a1);
		Sub2(0x8B2C48, int, tmp, int, PlantGroup*)(v4, a2);
		// LOGI("passed first 2");
		if (tmp) {
			Sub2(0x77A450, int, v5, int)(a1);
			RtWeakPtr<Zombie> zombiePtr;
			zombiePtr.FromOther((RtWeakPtr<Zombie>*)(a1 + 0x18));

			Zombie* zombie = zombiePtr.get();

			//LOGI("got zombie");

			float mX = a2->m_position.mX;
			float mY = a2->m_position.mY;
			float mZ = a2->m_position.mZ;

			float v5x = static_cast<float>(*(int*)(v5 + 0x74));
			float v5y = static_cast<float>(*(int*)(v5 + 0x78));
			SexyVector3 coords = SexyVector3(mX + v5x, mY, mZ + v5y);

			//LOGI("got coords");

			int board = (int) getBoard();
			int effectType = (int) Effect_FrogAnim::GetRTClass();
			Sub2(0x720D08, Effect_FrogAnim*, instance, int, int)(board, effectType);

			//LOGI("board created instance");

			RtWeakPtr<int> a3;
			// SexyString resourceName = *(SexyString*)(v5 + 0x68);
			// LOGI("Old resource name is %s", ((SexyString*)(v5 + 0x68))->c_str());
			SexyString resourceName = "POPANIM_PLANT_FROG";
			Sub(0xC90A14, RtWeakPtr<int>*, SexyString*)(&a3, &resourceName);
			int popAnim = (int)a3.get();
			Sub(0xE47544, Effect_PopAnim*, int, int)(instance, popAnim, 0);

			//LOGI("got pop anim");

			typedef int (*tempfun)(PlantGroup*);
			tempfun fun = *(tempfun*)(*(uint*)a2 + 0xB0);
			// LOGI("fun got");
			int v19 = fun(a2);
			// LOGI("v19 got");
			Sub(0x7401A8, Effect_PopAnim*, SexyVector3*, int)(instance, &coords, v19 + 2);

			//LOGI("set position and layer");

			Sub(0xE4761C, Effect_PopAnim*, int)(instance, 1);
			Sub(0x740DC8, Effect_PopAnim*, int)(instance, 1);

			//  LOGI("stuff");

			SexyString startAnim = "appear";
			SexyString loopAnim = "idle";
			SexyString jumpUpAnim = "jump_up_right";
			SexyString jumpDownAnim = "jump_down_right";
			std::vector<AnimationSequenceEntry> anims;
			Sub(0xE45AF4, std::vector<AnimationSequenceEntry>*, SexyString*, float)(&anims, &startAnim, 0.0);
			Sub(0xE45BD0, std::vector<AnimationSequenceEntry>*, SexyString*, float, int)(&anims, &loopAnim, MAXFLOAT, 0);
			Sub(0xE45AF4, std::vector<AnimationSequenceEntry>*, SexyString*, float)(&anims, &jumpUpAnim, 0.0);
			Sub(0xE45AF4, std::vector<AnimationSequenceEntry>*, SexyString*, float)(&anims, &jumpDownAnim, 0.0);
			// instance->m_timeOfLastStateSwitch = TimeMgr::GetInstance()->m_curTime;

			Sub(0xE4791C, Effect_PopAnim*, std::vector<AnimationSequenceEntry>*)(instance, &anims);
			
			// instance + 8 gives ptr
			RtWeakPtr<Effect_PopAnim> sheepPtr;
			sheepPtr.FromOther((RtWeakPtr<Effect_PopAnim>*) &instance->m_thisPtr);
			Sub(0x778BB4, Zombie*, RtWeakPtr<Effect_PopAnim>*)(zombie, &sheepPtr);

			typedef FrogJumpSubsystem* (*getSubsystem)(Board*, void*);
			FrogJumpSubsystem* subsystem = ((getSubsystem)getActualOffset(0x7287FC))(getBoard(), FrogJumpSubsystem::GetRTClass());
			RtWeakPtr<PlantGroup> plantGroupPtr;
			plantGroupPtr.FromOther((RtWeakPtr<PlantGroup>*) & a2->m_thisPtr);
			// LOGI("subsystem = %u", (uint)subsystem);
			subsystem->AddFrog((RtWeakPtr<Effect_FrogAnim>*) &sheepPtr, &plantGroupPtr, &coords);

			Sub(0xF3E09C, PlantGroup*, PlantConditions, float, float)(a2, plant_sheeped, MAXFLOAT, 0.0); // disable plants with condition
			// Sub(0x778C18, Zombie*, RtWeakPtr<PlantGroup>*)(zombie, &plantGroupPtr);

			getBoard()->wizardsSheepedPlant = true;
			// LOGI("Finished zap plant call");
		}
	}
}

typedef void (*restorePlants)(ZombieDarkWizard*);
restorePlants oRestorePlants = NULL;

void hkRestorePlants(ZombieDarkWizard* a1) {
	typedef FrogJumpSubsystem* (*getSubsystem)(Board*, void*);
	FrogJumpSubsystem* subsystem = ((getSubsystem)getActualOffset(0x7287FC))(getBoard(), FrogJumpSubsystem::GetRTClass());
	
	for (auto& sheep : a1->sheeps) {
		subsystem->MarkFrogForRemoval((RtWeakPtr<Effect_FrogAnim>*) &sheep);
	}

	a1->sheeps.clear();
}

typedef void (*plantGroupPlantingCallback)(int, int*, int, std::vector<int>*, int);
plantGroupPlantingCallback oCallback = NULL;

void hkCallback(int subsystem, int* gridCoords, int plantType, std::vector<int>* reasons, int a5) {
	return;
}

typedef void (*canPlantOnGroup)(PlantGroup*, int*, int, std::vector<int>*);
canPlantOnGroup oCanPlantOnGroup = NULL;

void hkCanPlantOnGroup(PlantGroup* r0_0, int* gridCell, int plantType, std::vector<int>* reasonsVec) {
	typedef int (*getPropsForPlantType)(int, int, int, int);
	int plantTypeProps = ((getPropsForPlantType)getActualOffset(0x8DCD14))(plantType, -1, -1, 0);

	bool cannotPlant = 0;

	for (auto& plantPtr : r0_0->m_plants) {
		RtWeakPtr<Plant> plantCopy;
		plantCopy.FromOther(&plantPtr);

		Plant* plant = plantCopy.get();
		if (FrogJumpSubsystem::checkIgnoreFrogs && 
			(plant->m_conditionTracker.HasCondition(plant_sheeped) 
				|| plant->m_conditionTracker.HasCondition(plantgroup_sheeped))) {
			continue;
		}

		typedef int (*plantIsDisabled)(Plant*);
		int disabled = ((plantIsDisabled)getActualOffset(0xE8BE88))(plant);
		if (disabled) {
			typedef int (*v14)(int, RtWeakPtr<Plant>*);
			v14 v14Fun = *(v14*)(*(uint*)plantType + 0x40);

			int v15 = v14Fun(plantType, &plantCopy);
			if (!v15) {
				typedef int (*hasIceblockCond)(Plant*);
				int iceblocked = ((hasIceblockCond)getActualOffset(0xE81BBC))(plant);
				if (iceblocked) {
					reasonsVec->push_back(0xF);
				}
				else {
					reasonsVec->push_back(0x2F);
				}
			}
		}

		int plantTypeLayer = *(int*)(plantTypeProps + 0x64);
		
		RtWeakPtr<int> propSheet;
		propSheet.FromOther((RtWeakPtr<int>*) &plant->m_propertySheet);

		int plantProps = (int) propSheet.get();
		if (*(int*)(plantProps + 0x64) == plantTypeLayer || plantTypeLayer == plant->m_multiPlantGridLayer) {
			typedef int (*canReceiveFirstAid)(int, Plant*);
			canReceiveFirstAid canReceiveFirstAidFunc = *(canReceiveFirstAid*)(*(uint*)plantType + 0x3C);
			bool canReceive = canReceiveFirstAidFunc(plantType, plant);

			cannotPlant |= (canReceive ^ 1);
			continue;
		}
	}

	if (cannotPlant) {
		reasonsVec->push_back(0x32);
	}
}

void registerPlantConditions() {
	LOGI("Registering custom plant conditions");

	typedef void* (*func12280C8)(); 
	func12280C8 func_0x12280C8 = (func12280C8)GetActualOffset(0x12280C8); 
	void* v0 = (void*)func_0x12280C8();

	if (v0)
	{
		typedef void* (*func122811C)(void*);
		func122811C func_0x122811C = (func122811C)GetActualOffset(0x122811C);
		auto* builder = (Reflection::CRefManualSymbolBuilder*)func_0x122811C(v0);
		if (builder)
		{
			std::vector<NamedFlagWithId> enumMembers;
			enumMembers.emplace_back("rapidfire", 0);
			enumMembers.emplace_back("squidifed", 1);
			enumMembers.emplace_back("sheeped", 2);
			enumMembers.emplace_back("damageflash", 3);
			enumMembers.emplace_back("plantfoodflash", 4);
			enumMembers.emplace_back("highlighted", 5);
			enumMembers.emplace_back("icecubed", 6);
			enumMembers.emplace_back("froststage1", 7);
			enumMembers.emplace_back("froststage2", 8);
			enumMembers.emplace_back("notfiring", 9);
			enumMembers.emplace_back("stunnedbyzombielove", 0xA);
			enumMembers.emplace_back("shadowboosted", 0xB);
			enumMembers.emplace_back("supershadowboosted", 0xC);
			enumMembers.emplace_back("swapping", 0xD);
			enumMembers.emplace_back("lifted_off", 0xE);
			enumMembers.emplace_back("sauced", 0x10);
			enumMembers.emplace_back("pvineboosted1", 0x11);
			enumMembers.emplace_back("pvineboosted2", 0x12);
			enumMembers.emplace_back("pvineboosted3", 0x13);
			enumMembers.emplace_back("groupicecubed", 0x14);
			enumMembers.emplace_back("groupsquidified", 0x15);
			enumMembers.emplace_back("groupsheeped", 0x16);
			enumMembers.emplace_back("groupsauced", 0x18);
			enumMembers.emplace_back("piss", 0x19);

			typedef void (*registerEnum)(Reflection::CRefManualSymbolBuilder*, SexyString*, std::vector<NamedFlagWithId>*, int);
			registerEnum func = *(registerEnum*)(*(uint*)builder + 0x18);

			SexyString enumName = "PlantConditions";
			func(builder, &enumName, &enumMembers, 0);
		}
	}
}

void initPlantCustomConditionsHooks() {
	PVZ2HookFunction(0xEA9EFC, (void*)hkGetConditionsCount, NULL, "GetConditionsCount");
	PVZ2HookFunction(0x77A5CC, (void*)hkWizardTurnTargetToSheep, (void**)&oWizardTurnTargetToSheep, "wizard turn target to sheep");
	PVZ2HookFunction(0x778140, (void*)hkRestorePlants, (void**)&oRestorePlants, "wizard restore plants");

	// PVZ2HookFunction(0xEB3EFC, (void*)hkCallback, (void**)&oCallback, "plant group planting callback");
	PVZ2HookFunction(0xF3ABC0, (void*)hkCanPlantOnGroup, (void**)&oCanPlantOnGroup, "can plant on group?");

	registerPlantConditions();

	Effect_FrogAnim::ModInit();
	FrogJumpSubsystem::ModInit();
}
