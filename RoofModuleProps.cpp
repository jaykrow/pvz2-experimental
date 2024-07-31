#include "pch.h"
#include "RoofModuleProps.h"
#include "RoofModule.h"

DEFINE_STATIC_RT_CLASS_MEMBERS(RoofModuleProps);

void* RoofModuleProps::GetModuleType() {
	return RoofModule::GetRTClass();
}

void RoofModuleProps::ModInit() {
	LOGI("RoofModuleProps mod init");

	vftable = copyVFTable(getActualOffset(0x1C4AEC0), 18);
	
	patchVFTable(vftable, (void*)RoofModuleProps::GetRTClass, 0);
	patchVFTable(vftable, (void*)RoofModuleProps::GetModuleType, 14);

	RoofModuleProps::GetRTClass();

	LOGI("RoofModuleProps finish init");
}