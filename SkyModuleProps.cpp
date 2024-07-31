#include "pch.h"
#include "SkyModuleProps.h"
#include "SkyModule.h"

DEFINE_STATIC_RT_CLASS_MEMBERS(SkyModuleProps);

void* SkyModuleProps::GetModuleType() {
	return SkyModule::GetRTClass();
}

void SkyModuleProps::ModInit() {
	LOGI("SkyModuleProps mod init");

	vftable = copyVFTable(getActualOffset(0x1C4AEC0), 18);

	patchVFTable(vftable, (void*)SkyModuleProps::GetRTClass, 0);
	patchVFTable(vftable, (void*)SkyModuleProps::GetModuleType, 14);

	SkyModuleProps::GetRTClass();

	LOGI("SkyModuleProps finish init");
}

