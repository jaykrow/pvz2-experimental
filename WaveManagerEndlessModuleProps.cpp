#include "pch.h"
#include "WaveManagerEndlessModuleProps.h"
#include "WaveManagerEndlessModule.h"

DEFINE_STATIC_RT_CLASS_MEMBERS(WaveManagerEndlessModuleProps);

void* WaveManagerEndlessModuleProps::GetModuleType() {
	return WaveManagerEndlessModule::GetRTClass();
}

void WaveManagerEndlessModuleProps::ModInit() {
	LOGI("WaveManagerEndlessModuleProps init");

	vftable = copyVFTable(getActualOffset(WAVEMANAGERMODULEPROPS_VTBL), WAVEMANAGERMODULEPROPS_VFUNCS);

	patchVFTable(vftable, (void*)WaveManagerEndlessModuleProps::GetRTClass, 0);
	patchVFTable(vftable, (void*)WaveManagerEndlessModuleProps::GetModuleType, 14);

	GetRTClass();
	LOGI("WaveManagerEndlessModuleProps finish init");
}
