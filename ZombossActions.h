#pragma once

#include "PropertySheetBase.h"
#include "Zombie.h"
#include "ZombieZombossMech.h"

class ZombieActionDefinition: public PropertySheetBase {
public:
	int Weight;
	int RepeatMin;
	int RepeatMax;
	bool CanPickAgain;
	bool Invulnerable;
	SexyString SoundOnStart;
	int Phase;
	int PhaseAfter;

	virtual void Function14() {};
	virtual void Function15() {};
};

static_assert(sizeof(ZombieActionDefinition) == 0x40);

class ZombossFireActionDefinition : public ZombieActionDefinition {
public:
	int FireMinColumn;
	int FireMaxColumn;
	int FireRowRange;
	int FireTargetPlantWeight;
	int FireTargetEmptyWeight;
	int FireTargetRailsWeight;
	int FireTargetTilesWeight;
	float FireRocketHitTime;
	float FireRocketSpeed;
	SexyString FireRocketDropSound;
	ValueRange FireRocketCount;

	virtual bool GetTargets(ZombieZombossMech* zomboss, std::vector<Point>* res);
	virtual void GetAmountTargets(ZombieZombossMech* zomboss, int rocketsAmount, std::vector<Point>* potentialTargets, std::vector<Point>* res);
	virtual void GetTargetsForType(ZombieZombossMech* zomboss, Rect* rangeRect, std::vector<Point>* res, Sexy::DelegateBase* targetChecker);
};

static_assert(sizeof(ZombossFireActionDefinition) == 0x78);

class ZombieActionHandler : public GameObject {
public:
	RtWeakPtr<ZombieActionDefinition> m_definition;
	RtWeakPtr<Zombie> m_zombie;
	bool m_done;
};

static_assert(sizeof(ZombieActionHandler) == 0x24);

class ZombieFireActionHandler : public ZombieActionHandler {
public:
	std::vector<Point> m_targetPoints;
	bool m_canLeaveRocketState;
	bool m_doneLaunching;
};

static_assert(sizeof(ZombieFireActionHandler) == 0x34);

class TargetingType {
public:
	int m_type = 0;
	int m_weight;
	Sexy::DelegateBase m_checkTarget;
	std::vector<Point> m_targets;
};

static_assert(sizeof(TargetingType) == 0x2C);
