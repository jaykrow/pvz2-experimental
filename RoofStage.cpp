#include "pch.h"
#include "RoofStage.h"

DEFINE_STATIC_RT_CLASS_MEMBERS(RoofStage);

float GetSlopeOffset(float mX, float mY) {
	auto roofStage = RoofStage::Get();
	if (roofStage) {
		return roofStage->GetDepth(mX, mY);
	}
	else {
		return 0.0f;
	}
}

void RoofStage::ModInit() {
	LOGI("RoofStage mod init");

	vftable = copyVFTable(getActualOffset(STAGEMODULE_VTBL), STAGEMODULE_VFUNCS);
	patchVFTable(vftable, (void*)RoofStage::GetRTClass, 0);

	RoofStage::GetRTClass();

	LOGI("RoofStage finish init");
}
