#pragma once
#include "ObjectTypeDescriptor.h"
#include <vector>
#include "PvZ2/RtWeakPtr.h"

#define ZOMBIETYPEDIRECTORY_GET 0x28FE30
#define ZOMBIETYPEDIRECTORY_GETTYPE 0x28FE9C

enum ZombieFlagType {
	noflag = 0,
	flag_normal = 1,
	flag_veteran = 2
};

class ZombieType : public ObjectTypeDescriptor
{
public:
	SexyString ZombieClass;
	SexyString HomeWorld;
	SexyString DisplayTypeName;
	SexyString AlmanacBackdropName;
	SexyString PopAnim;
	SexyString AnimRigClass;
	std::vector<SexyString> ResourceGroups;
	std::vector<SexyString> AudioGroups;
	RtWeakPtr<void> Properties; // ZombiePropertySheet
	bool Placeable;
	bool HastyOnStart;
	bool IsBasicZombie;
	bool IsFemaleZombie;
	bool HideFromAlmanac;
	ZombieFlagType FlagType;
	int unk;

	static void GetFromName(RtWeakPtr<ZombieType>* res, SexyString zombieName);
};

static_assert(sizeof(ZombieType) == 0x88);
static_assert(offsetof(ZombieType, AudioGroups) == 0x64);
static_assert(offsetof(ZombieType, FlagType) == 0x80);
