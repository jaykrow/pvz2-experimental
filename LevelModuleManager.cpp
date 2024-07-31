#include "pch.h"
#include "LevelModuleManager.h"

LevelModule* LevelModuleManager::GetModule(RtClass* rtClass) {
	typedef LevelModule* (*getModule)(LevelModuleManager*, RtClass* rtClass);
	return ((getModule)getActualOffset(0x70655C))(this, rtClass);
}

void Sexy::Event::AddDelegate(DelegateBase& delegate) {
	typedef void (*AddDelegate)(Sexy::Event*, Sexy::DelegateBase*, int);
	((AddDelegate)GetActualOffset(0x2A417C))(this, &delegate, 0);
}