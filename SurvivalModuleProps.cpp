#include "pch.h"
#include "SurvivalModuleProps.h"
#include "SurvivalModule.h"

DEFINE_STATIC_RT_CLASS_MEMBERS(SurvivalModuleProps);

void* SurvivalModuleProps::GetModuleType() {
	return SurvivalModule::GetRTClass();
}

void* SurvivalModuleProps::Destroy(SurvivalModuleProps* self) {
	typedef void* (*levelModulePropsDtor)(LevelModuleProperties*);
	return ((levelModulePropsDtor)getActualOffset(LEVELMODULEPROPS_DTOR_ADDR))(self);
}

void SurvivalModuleProps::ModInit() {
	LOGI("SurvivalModuleProps mod init");

	vftable = copyVFTable(getActualOffset(0x1C4AEC0), 18);

	patchVFTable(vftable, (void*)SurvivalModuleProps::GetRTClass, 0);
	patchVFTable(vftable, (void*)SurvivalModuleProps::Destroy, 2);
	patchVFTable(vftable, (void*)SurvivalModuleProps::DestroyAndDealloc, 3);
	patchVFTable(vftable, (void*)SurvivalModuleProps::GetModuleType, 14);

	GetRTClass();

	LOGI("Finish init SurvivalModuleProps");
}
