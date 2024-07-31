#include "pch.h"
#include "RoofStageProps.h"
#include "RoofStage.h"

DEFINE_STATIC_RT_CLASS_MEMBERS(RoofStageProps);

void* RoofStageProps::GetModuleType() {
	return RoofStage::GetRTClass();
}

void RoofStageProps::ModInit() {
	LOGI("RoofStageProps mod init");

	vftable = copyVFTable(getActualOffset(STAGEMODULEPROPS_VTBL), STAGEMODULEPROPS_VFUNCS);

	patchVFTable(vftable, (void*)RoofStageProps::GetRTClass, 0);
	patchVFTable(vftable, (void*)RoofStageProps::GetModuleType, 14);

	RoofStageProps::GetRTClass();

	LOGI("RoofStageProps finish init");
}
