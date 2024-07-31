#pragma once
#include "PvZ2/RtWeakPtr.h"
#include "DelegateBase.h"

class ZombieEvent
{
	RtWeakPtr<int> ownerPtr;
	SexyString eventName;
	Sexy::DelegateBase delegate;
	int isDelegateSetup;
};

