#include "pch.h"
#include "ZombieSteamMechanic.h"
#include "Zombie.h"
#include "ZombieState.h"
#include "memUtils.h"

typedef void (*zombieEnterState)(Zombie*, int, int);
typedef int (*zombieIsDeadOrDying)(Zombie*);
typedef Zombie* (*zombieAllowMovement)(Zombie*, int);

void* ZombieSteamMechanic::s_rtClass = NULL;
void* ZombieSteamMechanic::vftable = NULL;

static bool delegatesSetup;
static Sexy::DelegateBase walkCompleteDelegate;
static Sexy::DelegateBase transitionCompleteDelegate;
static Sexy::DelegateBase walkContinueDelegate;
static Sexy::DelegateBase machineIdleLoopDelegate;
static Sexy::DelegateBase attackCompletedDelegate;

void nullsub(ZombieSteamMechanic* zombie) { }

void mechanicWalkOnEnter(ZombieSteamMechanic* zombie) {
	LOGI("Entering walk");

	((zombieAllowMovement)getActualOffset(0x8C26B8))(zombie, 1); // allow movement?

	// RegisterEventAfterAnim(zombie, "walk", "onWalkCompleted");
	// LOGI("Event registered");
	RegisterEventOnWalkLoop(zombie, "onWalkAnimContinued"); // for looping anims, we also have to set a desired velocity
	// SetDesiredSpeed(zombie, 1.2);
}

void mechanicWalkOnLoop(ZombieSteamMechanic* zombie) {
	float mX = *(float*)((uint)zombie + 0x14);
	if (zombie->m_walkCycleFinished && mX <= 200.0 + 64 * 9.1) {
		((zombieEnterState)getActualOffset(0x8AEC44))(zombie, 0x11, 0);
	}
	zombie->m_walkCycleFinished = false;
}

void mechanicWalkOnExit(ZombieSteamMechanic* zombie) {
	((zombieAllowMovement)getActualOffset(0x8C26B8))(zombie, 0);
	LOGI("Exiting walk");
}

void mechanicTransitionOnEnter(ZombieSteamMechanic* zombie) {
	LOGI("Entering transition");
	RegisterEventAfterAnim(zombie, "transition", "onTransitionCompleted");
}

void mechanicMachineIdleOnEnter(ZombieSteamMechanic* zombie) {
	LOGI("Entering machine idle");

	RegisterEventOnIdleLoop(zombie, "idle_machine", "onMachineIdleContinued");
}

void mechanicMachineIdleOnLoop(ZombieSteamMechanic* zombie) {
	if (zombie->m_idleCycleFinished) {
		((zombieEnterState)getActualOffset(0x8AEC44))(zombie, 0x13, 0);
	}
	zombie->m_idleCycleFinished = false;
}

void mechanicAttackOnEnter(ZombieSteamMechanic* zombie) {
	RegisterEventAfterAnim(zombie, "attack", "onAttackCompleted");
}

void mechanicOnSpawn(ZombieSteamMechanic* zombie) {
	LOGI("Mechanic spawns");
	typedef void (*zombieFun49)(Zombie* zombie);
	((zombieFun49)getActualOffset(0x8AEAF8))(zombie);

	((zombieEnterState)getActualOffset(0x8AEC44))(zombie, 0x10, 0);
}

void ZombieSteamMechanic::modInit() {
	LOGI("ZombieSteamMechanic mod init");

	delegatesSetup = false;

	vftable = createChildVFTable(ZOMBIE_VFUNC_COUNT + 12, getActualOffset(ZOMBIE_VFTABLE_ADDR), ZOMBIE_VFUNC_COUNT);

	patchVFTable(vftable, (void*)ZombieSteamMechanic::getRTClass, 0);
	patchVFTable(vftable, (void*)mechanicOnSpawn, 49);

	patchVFTable(vftable, (void*)mechanicWalkOnEnter, 204);
	patchVFTable(vftable, (void*)mechanicWalkOnLoop, 205);
	patchVFTable(vftable, (void*)mechanicWalkOnExit, 206);

	patchVFTable(vftable, (void*)mechanicTransitionOnEnter, 207);
	patchVFTable(vftable, (void*)nullsub, 208);
	patchVFTable(vftable, (void*)nullsub, 209);

	patchVFTable(vftable, (void*)mechanicMachineIdleOnEnter, 210);
	patchVFTable(vftable, (void*)mechanicMachineIdleOnLoop, 211);
	patchVFTable(vftable, (void*)nullsub, 212);

	patchVFTable(vftable, (void*)mechanicAttackOnEnter, 213);
	patchVFTable(vftable, (void*)nullsub, 214);
	patchVFTable(vftable, (void*)nullsub, 215);

	ZombieSteamMechanic::getRTClass();

	LOGI("ZombieSteamMechanic finish init");
}

bool checkZombieIsDeadOrDying(Zombie* zombie) {
	return ((zombieIsDeadOrDying)getActualOffset(0x8AF73C))(zombie);
}

void walkCompleteCallback(Zombie* zombie) {
	if (!checkZombieIsDeadOrDying(zombie)) {
		ZombieSteamMechanic* mechanicZombie = static_cast<ZombieSteamMechanic*>(zombie);
		if (mechanicZombie) {
			mechanicZombie->m_walkCycleFinished = true;
		}
		// ((zombieEnterState)getActualOffset(0x8AEC44))(zombie, 0x11, 0);
	}
}

void transitionCompleteCallback(Zombie* zombie) {
	if (!checkZombieIsDeadOrDying(zombie)) {
		((zombieEnterState)getActualOffset(0x8AEC44))(zombie, 0x12, 0);
	}
}

int mechanicOnWalkContinued(ZombieSteamMechanic* zombie) {
	LOGI("Walk cycle finished");
	zombie->m_walkCycleFinished = true;
	return true;	
}

void mechanicOnMachineIdleLoop(ZombieSteamMechanic* zombie) {
	LOGI("Machine idle cycle finished");
	zombie->m_idleCycleFinished = true;
}

void mechanicOnAttackCompleted(ZombieSteamMechanic* zombie) {
	LOGI("Attack completed");
	if (!checkZombieIsDeadOrDying(zombie)) {
		((zombieEnterState)getActualOffset(0x8AEC44))(zombie, 0x12, 0);
	}
}

void ZombieSteamMechanic::buildEventCallbacks(Reflection::CRefManualSymbolBuilder* builder, void* rClass) {
	if (!delegatesSetup) {
		SetupLiteralDelegate(&walkCompleteDelegate, walkCompleteCallback);
		SetupLiteralDelegate(&transitionCompleteDelegate, transitionCompleteCallback);
		SetupLiteralDelegate(&walkContinueDelegate, (uint) mechanicOnWalkContinued);
		SetupLiteralDelegate(&machineIdleLoopDelegate, (uint) mechanicOnMachineIdleLoop);
		SetupLiteralDelegate(&attackCompletedDelegate, (uint)mechanicOnAttackCompleted);
		
		delegatesSetup = true;
	}

	RegisterEventCallback(builder, rClass, "onWalkCompleted", walkCompleteDelegate);
	RegisterEventCallback(builder, rClass, "onTransitionCompleted", transitionCompleteDelegate);
	RegisterEventCallback(builder, rClass, "onWalkAnimContinued", walkContinueDelegate);
	RegisterEventCallback(builder, rClass, "onMachineIdleContinued", machineIdleLoopDelegate);
	RegisterEventCallback(builder, rClass, "onAttackCompleted", attackCompletedDelegate);

	LOGI("Event callbacks built");
}

void ZombieSteamMechanic::buildStates() {
	void* rtClass = ZombieSteamMechanic::getRTClass();
	void* stateMachine = GetStateMachine(rtClass);

	// setup the walk in state. 330, 334 and 338 are used
	RegisterStateByOffsets(stateMachine, 0x10, 0x330, 0x334, 0x338, "ZS_Mechanic_Walk");

	// setup the transition state
	RegisterStateByOffsets(stateMachine, 0x11, 0x33C, 0x340, 0x344, "ZS_Mechanic_Transition");

	// Setup the machine_idle state
	RegisterStateByOffsets(stateMachine, 0x12, 0x348, 0x34C, 0x350, "ZS_Mechanic_MachineIdle");

	RegisterStateByOffsets(stateMachine, 0x13, 0x354, 0x358, 0x35C, "ZS_Mechanic_Attack");
}
