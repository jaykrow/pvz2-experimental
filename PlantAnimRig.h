#pragma once
#include "PopAnimRig.h"

#define PLANTANIMRIG_DTOR_ADDR 0x276F0C

class PlantCustomLayers {
	SexyString baseName;
};

class PlantAnimRig : public PopAnimRig
{
public:
	int stateAnimsCompleted;
	char unk1[24];
	int plantfoodPlayCount;
	int currentDamageStateIndex;
	int unk2;
	Sexy::DelegateBase plantfoodCallback;
	Sexy::DelegateBase firingCallback;
	ZombieEvent onPlantfoodMainStarted;
	ZombieEvent onPlantfoodMainLooped;
	ZombieEvent onPlantfoodMainEnded;
	std::vector<PlantCustomLayers> m_customizableLayers;
	int unk3;
	RtWeakPtr<void> props;
	int tier;
	int unk4;

	/*
	~PlantAnimRig() {
		typedef void (*unk1Destroy)(char*);
		((unk1Destroy)getActualOffset(0x276ED4))(unk1);

		typedef void (*unk2Destroy)(PlantAnimRig*);
		((unk2Destroy)getActualOffset(0x276FF8))(this);
	}
	*/ // I'm not the owner of these objects, so I don't worry about maintaining them

	virtual void Function32();
	virtual void Function33();
	virtual void PlayAttackBasedOnTier();
	virtual void PlayPlantfoodWithNoCallbacks();
	virtual void Function36();
	virtual void Function37();
	virtual void PlayHelpedOn();
	virtual void PlayHelped();
	virtual void PlayHelpedOff();
	virtual void PlayReload();
	virtual void PlayReloadLoop();
	virtual void PlayReloadEnd();
	virtual void PlayCharging();
	virtual void PlayCycling();
	virtual void Function46();
	virtual void Function47();
	virtual void Function48();
	virtual void Function49();
	virtual void Function50();
	virtual void Function51();
	virtual void PlayPlantfoodOn();
	virtual void PlayPlantfood();
	virtual void PlayPlantfoodOff();
	virtual void GetAttack();
	virtual void GetDamage();
	virtual void GetPlantfoodOn();
	virtual void GetPlantfood();
	virtual void GetPlantfoodOff();
	virtual void GetHelpedOn();
	virtual void GetHelped();
	virtual void GetHelpedOff();
	virtual void GetReload();
	virtual void GetReloadLoop();
	virtual void GetReloadEnd();
	virtual void GetCharging();
	virtual void GetCycling();
};

static_assert(sizeof(PlantAnimRig) == 0x278);
static_assert(offsetof(PlantAnimRig, plantfoodCallback) == 0x198);

