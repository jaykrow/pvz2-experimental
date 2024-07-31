#pragma once

#include "../BoardRegion.h"
#include "../AnimationMgr.h"
#include "../LevelModuleManager.h"
#include "../LevelDefinition.h"
#include <locale>
#include <string>
#include <codecvt>
#include "../Zombie.h"
#include "../GridItem.h"

#define GRID_TILE_WIDTH 64.0f
#define GRID_TILE_HEIGHT 76.0f
#define GRID_TILE_TOP 160.0
#define GRID_TILE_LEFT 200.0

#define BOARD_START_POS 232.0f
#define BOARD_END_POS 776.0f

class PresentRecord {
	SexyString presentTableName;
	uint seed;
	uint timeAwarded;
	uint timeOpened;
};

class RowPickingItem {
	int row;
	float weight;
	float lastPicked;
	float secondLastPicked;
};

class LootRollStats {
	int lootTotalCount;
	int silverCount;
	int goldCount;
	int diamondCount;
	int powerupCount;
	int noKeyCount;
	int keyCount;
};

class Board
{
public:
	char pad_0000[148]; //0x0000
	int m_columnCount; //0x0094
	int m_rowCount; //0x0098
	int m_boardState;
	SexyString level;
	SexyString previousLevel;
	int levelNumber;
	int elvelRandomIndex;
	float levelStartTime;
	std::vector<SexyString> gameplayResources;
	int plantfoodCount;
	std::vector<PresentRecord> pendingPresentRewards;
	std::vector<RtWeakPtr<BoardRegion>> boardRegions;
	SexyVector3 finalDestroyedZombiePosition;
	int unk;
	int chooseRowUnk[5];
	float timeRowGotLawnMowered[5];
	RowPickingItem rowPickingArray[5];
	int cellType[9][5];
	bool cellLocked[9][5];
	uint m_plantfoodCountMax;
	LootRollStats m_lootRollStats;
	int activeStarIndex;
	bool levelPreviouslyCompleted;
	bool sunSpawningSuppressed;
	bool plantGrowthAndDecayPaused;
	bool plantsCanAttack;
	bool manualPlantsActive;
	bool pauseGameplayObjects;
	bool onlyDrawFade;
	bool unk2;
	bool gridItemsEnabled;
	char pad285[0xB];
	RtWeakPtr<AnimationMgr> m_animationMgr;
	char pad298[0x38];
	int mowerLaunchesLeft;
	char pad2D4[32];
	SexyString collectableID_SunFromSky;
	RtWeakPtr<LevelDefinition> levelDefinition;
	LevelModuleManager* levelModuleManager;
	char pad30C[0x4];
	std::vector<void*> cursors;
	char pad31C[0x10];
	std::u32string levelNameOverride;
	char pad338[8];
	Rect m_lawnRect;
	char pad350[8];
	int levelNameTextWidgetPtr;
	char pad35C[0x40];
	std::vector<Zombie*> destroyedZombiesVec;
	std::vector<Zombie*> addedZombies;
	int fastForwardState;
	char pad3B8[8];
	int replayCost;
	char pad3C4[4];
	int bloverBlowAwayZombieCount;
	int tangleKelpedCount;
	int thawedIceblockCount;
	bool jestersSpawnedInLevel;
	bool jestersJuggled;
	bool wizardsSpawnedInLevel;
	bool wizardsSheepedPlant;
	bool unk3D8;
	bool zombieKilledByBanana;
	bool plantedPhatBeet;
	bool plantedNonPhatBeet;
	bool fishermanSpawnedInLevel;
	bool fishermanHookedAPlant;
	bool weaselTouchedASliderTile;
	bool weaselSpawnedInLevel;
	int trogolobitePushMinGridX;
	bool trogolobitePushABlock;
	bool excavatorSpawnedInLevel;
	bool excavatorShoveledAPlant;
	bool newspaperBurned;
	int goldLeafsPlantedCount;
	int tentSpawnedCount;
	int thymewarpPlantedCount;
	bool boomboxSpawnedInLevel;
	bool boomboxBoomed;
	bool dinoSpawnedInLevel;
	bool dinoAffectedAZombie;
	bool allStarSpawned;
	bool allStarDestroyedPlant;
	bool pad3FA[2];
	int tRexKillCount;
	char pad400[0x190];

	void SetPaused(bool paused) {
		typedef void (*setBoardPaused)(Board*, bool);
		((setBoardPaused)getActualOffset(0x73206C))(this, paused);
	}

	void ChangeState(int newState) {
		typedef void (*changeState)(Board*, int);
		((changeState)getActualOffset(0x71B1AC))(this, newState);
	}
	
	void ChangeLevelName(SexyString levelName) {
		std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
		std::u32string levelNameU32 = convert.from_bytes(levelName);

		levelNameOverride = levelNameU32;
		levelNameTextWidgetPtr = NULL;
	}

	void GetGridItemsAt(int gridX, int gridY, std::vector<GridItem*>* res) {
		typedef void (*func)(Board*, int, int, std::vector<GridItem*>*);
		func pFunc = (func) getActualOffset(0x720E7C);
		return pFunc(this, gridX, gridY, res);
	}
};

static_assert(sizeof(Board) == 0x590);
static_assert(offsetof(Board, boardRegions) == 0xE0);
static_assert(offsetof(Board, cellLocked) == 0x228);
static_assert(offsetof(Board, m_plantfoodCountMax) == 0x258);
static_assert(offsetof(Board, plantsCanAttack) == 0x27F);
static_assert(offsetof(Board, gridItemsEnabled) == 0x284);
static_assert(offsetof(Board, m_animationMgr) == 0x290);
static_assert(offsetof(Board, pad298) == 0x298);
static_assert(offsetof(Board, mowerLaunchesLeft) == 0x2D0);
static_assert(offsetof(Board, pad2D4) == 0x2D4);
static_assert(offsetof(Board, m_lawnRect) == 0x340);
static_assert(offsetof(Board, pad3C4) == 0x3C4);
static_assert(offsetof(Board, levelNameTextWidgetPtr) == 0x358);
static_assert(offsetof(Board, trogolobitePushMinGridX) == 0x3E0);
static_assert(offsetof(Board, allStarDestroyedPlant) == 0x3F9);
static_assert(offsetof(Board, addedZombies) == 0x3A8);

Board* getBoard();

Board* GetBoard();

void boardInitSixLanes(Board* board);

uint getLawnApp();

uint getSexyApp();