#pragma once
#include "Zombie.h"

// int sub_8E9FB0()

class CurveSegment_Float {
	float StartValue;
	float EndValue;
	float StartTime;
	float EndTime;
	int InterpMethod;
};

class CurveSequence_Float {
	std::vector<CurveSegment_Float> m_segments;
};

class CurveCollection_Float {
	std::vector<SexyString> m_sequenceNames;
	std::vector<CurveSequence_Float> m_sequences;
};

static_assert(sizeof(CurveCollection_Float) == 0x18);

class CurveSegment_Color {
	Color StartValue;
	Color EndValue;
	float StartTime;
	float EndTime;
	int InterpMethod;
};

class CurveSequence_Color {
	std::vector<CurveSegment_Color> m_segments;
};

static_assert(sizeof(CurveSequence_Color) == 12);

class ZombieZombossMech : public ZombieWithActions
{
public:
	float m_scheduledStateEndTime;
	int m_currentStageIndex;
	CurveCollection_Float m_movementCurves;
	CurveCollection_Float m_effectCurves;
	CurveSequence_Color m_colorCurve;
	float m_zombossScaledTime;
	bool m_dyingAnimDone;
	bool m_exitAnimDone;
	bool m_playingStunStart;
	bool m_playingStunEnd;
	bool m_takenStunAttack;
	bool m_haveShownHalfDamagePhase;
	bool m_cheatKillZombie;
	bool m_slideWithStreetView;
	bool m_canShowDamageEffects;
	int unk;
	RtWeakPtr<void> m_zombieDamageSparks;
	int m_droppedDebrisCount;
	int m_plantfoodDamageTaken;
	int m_zombieWavesFullySpawned;
	int m_nextZombieWaveWithPlantfood;
	RtWeakPtr<void> m_activeRockets;
	int unk2;
	std::vector<int> m_lootForSpawnedZombies; // actually std::vector<Loot>
	RtWeakPtr<void> unkPtr3;
};

static_assert(sizeof(ZombieZombossMech) == 0x3C8);
static_assert(offsetof(ZombieZombossMech, m_movementCurves) == 0x340);
static_assert(offsetof(ZombieZombossMech, m_playingStunStart) == 0x382); 
static_assert(offsetof(ZombieZombossMech, m_zombieDamageSparks) == 0x390);
static_assert(offsetof(ZombieZombossMech, m_activeRockets) == 0x3A8);

class ZombieAnimRig_ZombossMech : public ZombieAnimRig {

};

static_assert(sizeof(ZombieAnimRig_ZombossMech) == 0x19C);

class ZombieAnimRig_ZombossMech_Future : public ZombieAnimRig_ZombossMech {
public:
	int m_queuedTileClass;

	DEFINE_RT_CLASS_GET_CLASS_WRAPPER(0x3C88EC)
};
