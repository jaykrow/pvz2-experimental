#pragma once

#include "CRefManualSymbolBuilder.h"
#include "ZombieEvent.h"
#include "Zombie.h"

#define DECLARE_DELEGATES_SETUP(zombie) \
    static bool zombie##_delegatesSetup = false; 

#define DECLARE_CALLBACK(zombie, eventName) \
	static Sexy::DelegateBase zombie##_##eventName##Delegate;

#define IF_CALLBACK_NOTSETUP(zombie) \
    if (! zombie##_delegatesSetup)

#define DEFINE_CALLBACK(zombie, event) \
    SetupLiteralDelegate(& zombie##_##event##Delegate, (uint) zombie##_##event##Callback);

#define REGISTER_EVENT_CALLBACK(builder, rclass, zombie, event) \
    RegisterEventCallback(builder, rclass, #event, zombie##_##event##Delegate);

#define DEFINE_CALLBACK_FUNC(zombie1, event) \
    void zombie1##_##event##Callback(zombie1 *a1)


void RegisterEventCallback(Reflection::CRefManualSymbolBuilder* builder, void* rClass, const SexyString& eventName, Sexy::DelegateBase &delegate);

void RegisterStateByOffsets(void* stateMachine, int stateID, int onEnterOffset, int onLoopOffset, int onExitOffset, const SexyString &stateName);

void* GetStateMachine(void* rClass);

void RegisterEventAfterAnim(Zombie* zombie, const SexyString& animName, const SexyString& eventName);

void RegisterEventOnWalkLoop(Zombie* zombie, const SexyString& eventName);

void RegisterEventOnIdleLoop(Zombie* zombie, const SexyString& animName, const SexyString& eventName);

void RegisterEventOnLoop(Zombie* zombie, const SexyString& animName, const SexyString & eventName);

void SetupLiteralDelegate(Sexy::DelegateBase* delegate, void (*delegateFun)(Zombie*));

void SetupLiteralDelegate(Sexy::DelegateBase* delegate, uint delegateAddr);

void SetDesiredSpeed(Zombie* zombie, float speed);
