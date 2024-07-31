#pragma once

#include "PropertySheetBase.h"

#define LEVELMODULEPROPS_GETCLASS_ADDR 0xDE4004
#define LEVELMODULEPROPS_DTOR_ADDR 0x2A37A4

class LevelModuleProperties : public PropertySheetBase
{
public:
	std::vector<SexyString> resourceGroupNames;
	RtWeakPtr<void> difficultyProps;
	SexyString iconImage;
	SexyString iconText;
};

static_assert(sizeof(LevelModuleProperties) == 0x48); // 0x50 is the true size
