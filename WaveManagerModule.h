#pragma once

#include "LevelModule.h"
#include "WaveManager.h"

#define WAVEMANAGERMODULE_GETCLASS_ADDR 0x56979C
#define WAVEMANAGERMODULE_VTBL 0x1BD219C
#define WAVEMANAGERMODULE_VFUNCS 19

class WaveManagerModule : public LevelModule
{
public:
	DEFINE_RT_CLASS_GET_CLASS_WRAPPER(WAVEMANAGERMODULE_GETCLASS_ADDR);

	WaveManager waveManager;
};

static_assert(sizeof(WaveManagerModule) == 0xA20);
static_assert(offsetof(WaveManagerModule, waveManager) == 0x18);

