#pragma once

#include "RtObject.h"
#include "PvZ2/RtWeakPtr.h"

class CachedResource : public Sexy::RtObject // should probably be a template
{
	CachedResource* staticPtr;
	int unk;
	SexyString* resourceName;
	RtWeakPtr<void> resource; // Image type for current use case
};

