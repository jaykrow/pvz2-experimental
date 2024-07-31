#pragma once

#include "LevelModule.h"
#include "SeedBankNew.h"

class PlantBoostPair {
public:
	int PlantID;
	int ActiveBoostType;
};

class LevelPurchasesData : public Sexy::RtObject {
public:
	bool purchasedSeedSlot;
};

static_assert(sizeof(LevelPurchasesData) == 8);

class SeedBankModule : public LevelModule {
public:
	RtWeakPtr<SeedBankNew> m_seedBank;
	RtWeakPtr<SeedChooser> m_seedChooser;
	bool m_resourcesLoaded;
	bool mUsesSeedChooser;
	int m_selectionMethod;
	std::vector<PlantBoostPair> m_boostsApplied;
	bool m_hasAppliedCombatBoosts;
	LevelPurchasesData m_stagedPurchases;
	LevelPurchasesData m_completedPurchases;
	std::vector<SexyString> m_availableSeeds;
	char pad05C[28];
};

static_assert(sizeof(SeedBankModule) == 0x78);

