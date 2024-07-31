#pragma once

#include "UIWidget.h"

class SeedChooser : public SlidingWidget
{
	char unk3[0xE8];
};

static_assert(sizeof(SeedChooser) == 0x238);

