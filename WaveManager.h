#pragma once

#include "RtObject.h"
#include "PvZ2/RtWeakPtr.h"
#include <random>
#include "WaveManagerProperties.h"

#define WAVEMANAGER_CONSTRUCT_ADDR 0x56D7D4

enum WaveManagerState {
	wavemanagerunk = 0,
	wavemanageractive = 1,
	flagdelay = 2,
	wavemanagersuppressed = 3,
	wavemanagerended = 4
};

struct UnkMTRand {
	std::mt19937 mt;
	int unk1;
	int unk2;
};

static_assert(sizeof(UnkMTRand) == 2508);

enum WaveType {
	wavetypeunk = 0,
	wavetypenormal = 1,
	wavetypehugewave = 2
};

class Wave {
public:
	int type;
	std::vector<RtWeakPtr<void>> actions;
};

class WaveManager : public Sexy::RtObject
{
public:
	int unk;
	RtWeakPtr<void> m_props;
	std::vector<Wave> m_waves;
	int m_currentWave;
	float m_nextWaveTime;
	float m_lastWaveTime;
	int m_currentWaveTotalHealth;
	int m_nextWaveHealthThreshold;
	WaveManagerState m_state;
	bool m_paused;
	bool m_forceSpawnNextWave;
	bool m_hasDynamicZombies;
	UnkMTRand unkMtRand;
	int unk2;

	void SetPaused(bool paused);

	bool IsPaused();

	WaveManagerProperties* GetProps();
};

static_assert(sizeof(WaveManager) == 0xA08);
static_assert(offsetof(WaveManager, m_waves) == 0x10);
static_assert(offsetof(WaveManager, m_currentWaveTotalHealth) == 0x28);
static_assert(offsetof(WaveManager, m_state) == 0x30);

void DoActionsForCurrentWave(WaveManager*);
// void SpawnNextWaveIfBelowThreshold(WaveManager*)
void UpdateProgressMeter(WaveManager*);
void SetSuppressedState(WaveManager*, bool);
WaveManager* GetWaveManager();
