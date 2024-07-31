#include "pch.h"
#include "DummyModule.h"
#include "ZombieStateHelper.h"

DEFINE_STATIC_RT_CLASS_MEMBERS(DummyModule);

void DummyModule::ModInit() {
	LOGI("DummyModule init");

	vftable = copyVFTable(getActualOffset(0x1C41DE4), 19);

	patchVFTable(vftable, (void*)DummyModule::GetRTClass, 0);

	GetRTClass();

	LOGI("Finished DummyModule");
}
