#pragma once

#include "RtObject.h"
#include "ReflectionBuilder.h"
#include "PvZ2/RtWeakPtr.h"

class BaseCursor : public Sexy::RtObject { };

class PacketCursor : public BaseCursor {
public:
	int mX;
	int mY;
	Rect bounds;
	char pad001C[0xC];
	RtWeakPtr<void> unkPtr;

	virtual void Function7() {};
	virtual void Function8() {};
	virtual void Function9() {};
	virtual void Function10() {};
	virtual void Function11() {};
	virtual bool CanPlantOnCursor() { return true;  }; // stub implementation
	virtual void GetGridCoords() {}; // cannot accurately recreate atm i think
	virtual void AttemptPlant() {};

	DEFINE_RT_CLASS_GET_CLASS_WRAPPER(0x929E00);
};

static_assert(sizeof(PacketCursor) == 0x30);
