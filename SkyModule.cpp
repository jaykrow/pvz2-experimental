#include "pch.h"
#include "SkyModule.h"
#include "BoardRegionSky.h"
#include "PvZ2/Board.h"

DEFINE_STATIC_RT_CLASS_MEMBERS(SkyModule);

void SkyModuleInitRegion(SkyModule* skyModule) {
	LOGI("Sky module init region");
	
	Sexy::RtClass* regionType = BoardRegionSky::GetRTClass(); 

	typedef BoardRegion* (*createRegion)(Board*, Sexy::RtClass*);
	BoardRegion* region = ((createRegion)getActualOffset(0x72AAA0))(getBoard(), regionType);

	FRect rect = FRect(GRID_TILE_LEFT + 3 * GRID_TILE_WIDTH, -40.0, 976.0, 850.0);

	typedef void (*setRegionBundaries)(BoardRegion*, FRect*);
	((setRegionBundaries)getActualOffset(0x104FD9C))(region, &rect);

	LOGI("Set region boundaries");
	LOG_RETURN_ADDRESS(0);
}

void SkyModule::RegisterCallbacks(SkyModule *self) {
	typedef uint (*subCE856C)();
	uint levelModuleManager = ((subCE856C)getActualOffset(0xCE856C))();
	typedef int (*sub2A417C)(uint, Sexy::DelegateBase*, int);

	// Takes code from 4AADF8
	Sexy::DelegateBase delegate = CreateEmptyDelegate(0x1BB8A90);
	delegate.useOwnerVtable = false;
	delegate.callbackFunc = (uint) SkyModuleInitRegion;
	delegate.callbackOwner = (uint) self;
	delegate.unk0x10 = true;
	delegate.executeCallbackFunction = getActualOffset(0x4ADF24);

	((sub2A417C)getActualOffset(0x2A417C))(levelModuleManager + 0x54, &delegate, 0);

	LOGI("SkyModule callbacks registered");
}

void SkyModule::ModInit() {
	LOGI("SkyModule init");
	
	vftable = copyVFTable(getActualOffset(0x1C41DE4), 19);

	patchVFTable(vftable, (void*)SkyModule::GetRTClass, 0);
	patchVFTable(vftable, (void*)SkyModule::RegisterCallbacks, 7);

	GetRTClass();
	LOGI("Finished initializing SkyModule");
}
