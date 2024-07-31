#include "pch.h"
#include "StageModule.h"

StageModule* StageModule::GetInstance(Board* board) {
	typedef StageModule* (*GetStageModule)(Board*);
	return ((GetStageModule)getActualOffset(0x721A4C))(board); // just get the first module and return that
}

StageModuleProperties* StageModule::GetProps() {
	RtWeakPtr<StageModuleProperties> propsPtr;
	propsPtr.FromOther((RtWeakPtr<StageModuleProperties>*) & this->propSheetPtr);
	return propsPtr.get();
}