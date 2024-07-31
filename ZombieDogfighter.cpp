#include "pch.h"
#include "ZombieDogfighter.h"
#include "ZombieStateHelper.h"

DEFINE_STATIC_RT_CLASS_MEMBERS(ZombieDogfighter);

DECLARE_DELEGATES_SETUP(ZombieDogfighter);

DECLARE_CALLBACK(ZombieDogfighter, onWalkCompleted);

DEFINE_CALLBACK_FUNC(ZombieDogfighter, onWalkCompleted) {

}

void ZombieDogfighter::buildEventCallbacks(Reflection::CRefManualSymbolBuilder* builder, void* rtClass) {
	IF_CALLBACK_NOTSETUP(ZombieDogfighter) {
		DEFINE_CALLBACK(ZombieDogfighter, onWalkCompleted);
	}
}

void ZombieDogfighter::buildStates() {

}

void ZombieDogfighter::ModInit() {
	LOGI("ZombieDogfighter mod init");

	vftable = createChildVFTable(ZOMBIE_VFUNC_COUNT, getActualOffset(ZOMBIE_VFTABLE_ADDR), ZOMBIE_VFUNC_COUNT);
	patchVFTable(vftable, (void*)ZombieDogfighter::GetRTClass, 0);

	LOGI("ZombieDogfighter finished init");
}

