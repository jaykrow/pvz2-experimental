#include "pch.h"
#include "SixLaneModuleProps.h"
#include "SixLaneModule.h"

void* SixLaneModuleProps::s_rtClass = NULL;
void* SixLaneModuleProps::vftable = NULL;

void* SixLaneModuleProps::getModuleType() {
	return SixLaneModule::getRTClass();
}

void SixLaneModuleProps::modInit() {
	LOGI("SixLaneModuleProps mod init");

	vftable = copyVFTable(getActualOffset(0x1C4AEC0), 18);

	patchVFTable(vftable, (void*)SixLaneModuleProps::getRTClass, 0);
	patchVFTable(vftable, (void*)SixLaneModuleProps::getModuleType, 14);

	SixLaneModuleProps::getRTClass();

	LOGI("SixLaneModuleProps finish init");
}
