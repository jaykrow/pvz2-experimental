#include "pch.h"
#include "ZombieStateHelper.h"
#include "memUtils.h"
#include "ZombieState.h"
#include "DelegateBase.h"
#include "Zombie.h"

#define ZOMBIE_STATE_CONSTRUCT_ADDR 0x743530
#define ZOMBIE_STATE_REGISTER_ADDR 0x743458
#define ZOMBIE_EVENT_CONSTRUCT_ADDR 0x3A56A0

const uint delegateZombieState = 0x1C08B30;
const uint delegateGeneric = 0x1BB8A90;
const uint executeCallbackFunc = 0x88AA78; // they all look the same

typedef int (*sub3B13D0)(Reflection::CRefManualSymbolBuilder*);
typedef int (*sub121FF54)(Reflection::CRefManualSymbolBuilder*, int, void*, void*, uint, int);

typedef ZombieState* (*ConstructState)(ZombieState*, int, Sexy::DelegateBase&, Sexy::DelegateBase&, Sexy::DelegateBase&, const SexyString&);
typedef void* (*RegisterState)(void* stateMachine, int, ZombieState*);

typedef uint (*sub272A98)();
typedef void* (*getStateMachine)(uint, void*);

typedef uint (*getAnimRig)(Zombie*);
typedef int (*playAnimWithCallback)(uint, const SexyString&, int, ZombieEvent& event);

typedef int (*playLoopAnimWithCallback)(uint, ZombieEvent& event);

typedef ZombieEvent* (*ConstructEvent)(ZombieEvent*, RtWeakPtr<int>& owner, const SexyString& eventName);

void RegisterEventCallback(Reflection::CRefManualSymbolBuilder* builder, void* rClass, const SexyString& eventName, Sexy::DelegateBase &delegate) {
	void* voidPropType = builder->GetPropertyOfType(Reflection::Type_Void, 0u);
	int unk1 = ((sub3B13D0)getActualOffset(0x3B13D0))(builder);
	int unk2 = ((sub121FF54)getActualOffset(0x121FF54))(builder, 6, rClass, voidPropType, 1u, unk1);
	builder->RegisterCallback(rClass, eventName, &delegate, unk2, 0);
	// LOGI("Callback registered");
}

void setVftable(Sexy::DelegateBase* a1, uint vtableAddr) {
	*(uint*)a1 = getActualOffset(vtableAddr);
}

void setupDelegate(Sexy::DelegateBase& a1, uint owner, int funcOffset) {
	a1.callbackFunc = funcOffset;
	a1.useOwnerVtable = true;
	a1.callbackOwner = owner;
	a1.unk0x10 = 1;
	a1.executeCallbackFunction = getActualOffset(executeCallbackFunc);
}

void RegisterStateByOffsets(void* stateMachine, int stateID, int onEnterOffset, int onLoopOffset, int onExitOffset, const SexyString &stateName) {
	// onLoop uses a normal Sexy::DelegateBase vtable, while the other 2 uses a special DelegateBase<ZombieState> one
	Sexy::DelegateBase onEnter, onLoop, onExit;
	setVftable(&onEnter, delegateZombieState);
	setupDelegate(onEnter, NULL, onEnterOffset);

	setVftable(&onExit, delegateZombieState);
	setupDelegate(onExit, NULL, onExitOffset);

	setVftable(&onLoop, delegateGeneric);
	setupDelegate(onLoop, NULL, onLoopOffset);

	ZombieState state;
	((ConstructState)getActualOffset(ZOMBIE_STATE_CONSTRUCT_ADDR))(&state, stateID, onEnter, onLoop, onExit, stateName);
	((RegisterState)getActualOffset(ZOMBIE_STATE_REGISTER_ADDR))(stateMachine, stateID, &state);
}

void* GetStateMachine(void* rClass) {
	uint unk = ((sub272A98)getActualOffset(0x272A98))();
	return ((getStateMachine)getActualOffset(0x743344))(unk, rClass);
}

void RegisterEventAfterAnim(Zombie* zombie, const SexyString& animName, const SexyString& eventName) {
	uint animRig = ((getAnimRig)getActualOffset(0x74142C))(zombie);
	RtWeakPtr<int> zombiePtr = RtWeakPtr<int>();
	zombiePtr.FromOther((RtWeakPtr<int>*)((uint)zombie + 8));

	ZombieEvent zombieEvent;
	((ConstructEvent)getActualOffset(ZOMBIE_EVENT_CONSTRUCT_ADDR))(&zombieEvent, zombiePtr, eventName);

	playAnimWithCallback func = *(playAnimWithCallback*)(*(uint*)animRig + 0x9C);
	func(animRig, animName, 0, zombieEvent);
}

void RegisterEventOnWalkLoop(Zombie* zombie, const SexyString& eventName) {
	uint animRig = ((getAnimRig)getActualOffset(0x74142C))(zombie);
	RtWeakPtr<int> zombiePtr = RtWeakPtr<int>();
	zombiePtr.FromOther((RtWeakPtr<int>*)((uint)zombie + 8));

	ZombieEvent zombieEvent;
	((ConstructEvent)getActualOffset(ZOMBIE_EVENT_CONSTRUCT_ADDR))(&zombieEvent, zombiePtr, eventName);

	// this is hardcoded to only respond to animations with name "walk"
	playLoopAnimWithCallback func = *(playLoopAnimWithCallback*)(*(uint*)animRig + 0x84);
	func(animRig, zombieEvent);
}

void RegisterEventOnIdleLoop(Zombie* zombie, const SexyString& animName, const SexyString& eventName) {
	uint animRig = ((getAnimRig)getActualOffset(0x74142C))(zombie);
	RtWeakPtr<int> zombiePtr = RtWeakPtr<int>();
	zombiePtr.FromOther((RtWeakPtr<int>*)((uint)zombie + 8));

	ZombieEvent zombieEvent;
	((ConstructEvent)getActualOffset(ZOMBIE_EVENT_CONSTRUCT_ADDR))(&zombieEvent, zombiePtr, eventName);

	typedef int (*playAnimWithCallback)(uint, SexyString&, int, ZombieEvent&);
	SexyString animNameCopy = animName;
	((playAnimWithCallback)getActualOffset(0x667C88))(animRig, animNameCopy, 3, zombieEvent);
}

void RegisterEventOnLoop(Zombie* zombie, const SexyString& animName, const SexyString& eventName) {
	ZombieAnimRig* animRig = (ZombieAnimRig*)((getAnimRig)getActualOffset(0x74142C))(zombie);
	RtWeakPtr<int> zombiePtr;
	zombiePtr.FromOther((RtWeakPtr<int>*) & zombie->m_thisPtr);

	ZombieEvent zombieEvent;
	((ConstructEvent)getActualOffset(ZOMBIE_EVENT_CONSTRUCT_ADDR))(&zombieEvent, zombiePtr, eventName);

	typedef int (*playAnimWithCallback)(ZombieAnimRig*, SexyString&, int, ZombieEvent&);
	SexyString animNameCopy = animName;
	((playAnimWithCallback)getActualOffset(0x667C88))(animRig, animNameCopy, 0, zombieEvent);
}

void SetupLiteralDelegate(Sexy::DelegateBase* delegate, void (*delegateFun)(Zombie*)) {
	uint delegateAddr = (uint)delegateFun;

	delegate->callbackFunc = delegateAddr;
	delegate->useOwnerVtable = false;
	delegate->callbackOwner = NULL;
	delegate->unk0x10 = 1;
	delegate->executeCallbackFunction = getActualOffset(executeCallbackFunc);
}

void SetupLiteralDelegate(Sexy::DelegateBase* delegate, uint delegateAddr) {
	delegate->callbackFunc = delegateAddr;
	delegate->useOwnerVtable = false;
	delegate->callbackOwner = NULL;
	delegate->unk0x10 = 1;
	delegate->executeCallbackFunction = getActualOffset(executeCallbackFunc);
}

void SetDesiredSpeed(Zombie* zombie, float speed) {
	uint animRig = ((getAnimRig)getActualOffset(0x74142C))(zombie);

	typedef void (*sub5774F0)(uint, float);
	((sub5774F0)getActualOffset(0x5774F0))(animRig, speed);
}
