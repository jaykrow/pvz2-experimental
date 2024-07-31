#include "pch.h"
#include "SixLaneModule.h"
#include "PvZ2/Board.h"

void* SixLaneModule::s_rtClass = NULL;
void* SixLaneModule::vftable = NULL;

void SixLaneModule::registerDelegates(SixLaneModule* self) {
	LOGI("Code called to setup board for 6 lanes");
	boardInitSixLanes(getBoard());
}

void SixLaneModule::modInit() {
	LOGI("SixLaneModule mod init");

	vftable = copyVFTable(getActualOffset(0x1C41DE4), 19);

	patchVFTable(vftable, (void*)SixLaneModule::getRTClass, 0);
	patchVFTable(vftable, (void*)SixLaneModule::registerDelegates, 7);

	SixLaneModule::getRTClass();

	LOGI("SixLaneModule finish init");
}
