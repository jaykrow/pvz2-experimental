#include "pch.h"
#include "WaveManager.h"
#include "PvZ2/Board.h"

void DoActionsForCurrentWave(WaveManager* self) {
	typedef void (*doActionsFunc)(WaveManager*);
	((doActionsFunc)getActualOffset(0x56EAC8))(self);
}

// void SpawnNextWaveIfBelowThreshold(WaveManager*)

void UpdateProgressMeter(WaveManager* self) {
	typedef void (*updateProgress)(WaveManager*);
	((updateProgress)getActualOffset(0x56ECE4))(self);
}

void SetSuppressedState(WaveManager* self, bool suppressed) {
	typedef void (*setSuppressed)(WaveManager*, int);
	((setSuppressed)getActualOffset(0x56E8CC))(self, suppressed);
}

WaveManager* GetWaveManager() {
	typedef WaveManager* (*getWaveManager)(Board*);
	return ((getWaveManager)getActualOffset(0x72099C))(getBoard());
}

void WaveManager::SetPaused(bool paused) {
	// this->m_paused = paused;
	SetSuppressedState(this, paused);
}

bool WaveManager::IsPaused() {
	// return this->m_paused;
	return this->m_state == wavemanagersuppressed;
}

WaveManagerProperties* WaveManager::GetProps() {
	RtWeakPtr<WaveManagerProperties> propsPtr;
	propsPtr.FromOther((RtWeakPtr<WaveManagerProperties>*) & m_props);
	return propsPtr.get();
}