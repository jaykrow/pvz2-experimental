#pragma once
#include "PvZ2/RtWeakPtr.h"

class PlantAction
{
public:
	long long type;
	RtWeakPtr<int> projectile;
	int explodeType;
	float explodeRadius;
	int triggerType;
	bool dieAfterExplode;
	bool burns;
	bool secondaryAction;
	bool unkCond;
	float initialMinCooldownTime;
	float cooldownTimeMin;
	float cooldownTimeMax;
	int damage;
	int splashDamage;
	float xVelocityOverride;
	int pushbackAmount;
	float chillDuration;
	float freezeDuration;
	float stunDuration;
	SexyString projectileLaunchSound;
	Rect rectTriggerRange;
	float spawnOffsetXX;
	float spawnOffsetY;
	int unk;
};

