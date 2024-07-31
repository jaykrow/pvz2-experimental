#pragma once
#include "DelegateBase.h"

class ZombieState
{
	int id;
	Sexy::DelegateBase onEnter;
	Sexy::DelegateBase onLoop;
	Sexy::DelegateBase onExit;
	SexyString name;
};

