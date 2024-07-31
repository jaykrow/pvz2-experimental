#pragma once
#include "PropertySheetBase.h"
#include "StageModuleProperties.h"
#include "LevelModule.h"

class LevelDefinition : public PropertySheetBase
{
public:
	SexyString name;
	SexyString description;
	SexyString mainObjective;
	int16_t startingSun;
	bool addBonusStartingSun;
	bool suppressSunSpawn;
	int levelNumber;
	int flickPowerupRightEdgeOFffset;
	int unk;
	RtWeakPtr<StageModuleProperties> stageModuleProps;
	RtWeakPtr<void> victoryModuleProps;
	RtWeakPtr<void> lossModuleProps;
	RtWeakPtr<void> levelMutatorModuleProps;
	std::vector<LevelModule> modules;
	int unk2;
	RtWeakPtr<void> lootConfig;
	SexyString normalPresentTable;
	SexyString shinyPresentTable;
	bool suppressAwardScreen;
	int firstRewardType;
	SexyString firstRewardParam;
	int replayRewardType;
	SexyString replayRewardParam;
	bool gameOverDialogShowBrain;
	SexyString firstIntroNarrative;
	SexyString replayIntroNarrative;
	SexyString firstOutroNarrative;
	SexyString replayOutroNarrative;
	SexyString forceNextLevel;
	bool forceWarpTunnel;
	bool forceToWorldMap;
	bool repeatPlayForceToWorldMap;
	bool isVasebreaker;
	bool isDangerRoom;
	bool isLevelOfTheDay;
	bool disableRestart;
	SexyString completionAchievement;
	std::vector<int> gameFeaturesToUnlock; // vector<GameFeature>
	SexyString musicType;
	SexyString draperPromo;
	SexyString levelPowerupSet;
	SexyString levelJam;
	bool doMusicChanges;
	std::vector<SexyString> resourceGroupNames;
	std::vector<SexyString> levelPrerequisites;
	std::vector<SexyString> groupsToUnloadForAds;
	std::vector<SexyString> randomLevelSet;
	bool loadDefaultMusic;
	bool suppressDynamicTutorial;
	int zombieLevel;
	int gridItemLevel;
	int fixedPlantLevel;
	bool suppressPlantfoodPurchase;
	char suppressAccountUpgrades[2]; // it's actually a wide, but I need this to allow non-alignment
	bool disablePlantBoosts;
	bool disableSaveGame;
	bool pad19D[2];
	char generatorResults[192]; // this is its own object
	std::vector<SexyString> perksDisabledInLevel;
	std::vector<std::vector<SexyString>> perksDisabledPerDifficulty;
};

static_assert(sizeof(LevelDefinition) == 0x278);
static_assert(offsetof(LevelDefinition, firstRewardType) == 0xA4);
static_assert(offsetof(LevelDefinition, startingSun) == 0x40);
static_assert(offsetof(LevelDefinition, addBonusStartingSun) == 0x42);
static_assert(offsetof(LevelDefinition, forceNextLevel) == 0xF8);
static_assert(offsetof(LevelDefinition, gameFeaturesToUnlock) == 0x118);
static_assert(offsetof(LevelDefinition, doMusicChanges) == 0x154);
static_assert(offsetof(LevelDefinition, suppressAccountUpgrades) == 0x199);
