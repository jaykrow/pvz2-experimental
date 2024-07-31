#pragma once
#include "PvZ2/RtWeakPtr.h"
#include "LevelModuleProperties.h"

#define LEVELMODULE_GETCLASS_ADDR 0xCE8100
#define LEVELMODULE_VTBL 0x1C41DE4
#define LEVELMODULE_VTBL_NFUNC 19
#define LEVELMODULE_DTOR_ADDR 0x2ED868

class LevelModule : public GameObject
{
public:
	RtWeakPtr<LevelModuleProperties> propSheetPtr;
};

