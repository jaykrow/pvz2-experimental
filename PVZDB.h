#pragma once

#include "memUtils.h"
#include "PvZ2/RtWeakPtr.h"

enum RtTypeId
{
    ScratchSpace = 0,
    GameSystem = 1,
    Version = 2,
    Manifest = 3,
    CDNConfig = 4,
    ForceUpdateConfig = 5,
    LiveConfig = 6,
    StartupConfig = 7,
    NewMapConversionMapping = 8,
    OldMapDataMapping = 9,
    PropertySheets = 10,
    PersonalConfig = 11,
    PinataTypes = 12,
    PlantTypes = 13,
    PlantLevels = 14,
    PlantMastery = 15,
    PlantPowerUps = 16,
    PlantAlmanacData = 17,
    PlantProperties = 18,
    Powers = 19,
    ZombieTypes = 20,
    ZombieActions = 21,
    ZombieProperties = 22,
    CreatureTypes = 23,
    ProjectileTypes = 24,
    GridItemTypes = 25,
    EffectObjectTypes = 26,
    CollectableTypes = 27,
    PresentTableTypes = 28,
    PresentTypes = 29,
    PlantFamilyTypes = 30,
    Quests = 31,
    QuestsCategories = 32,
    LoadingText = 33,
    QuestThemes = 34,
    QuestsActive = 35,
    UIWidgetSheets = 36,
    NPCDataSheets = 37,
    LevelModules = 38,
    HeroTypes = 39,
    PowerupTypes = 40,
    GameFeatures = 41,
    ToolPackets = 42,
    StreamingMusic = 43,
    Products = 44,
    MarketLayout = 45,
    MarketSchedule = 46,
    RAPSchedule = 47,
    PlayerSegments = 48,
    BoardGridMaps = 49,
    LevelModulesDifficulty = 50,
    LevelMutatorModules = 51,
    LevelMutatorTables = 52,
    LevelScoringRules = 53,
    HotUIConfig = 54,
    PinataRewards = 55,
    ArmorTypes = 56,
    News = 57,
    WorldMapList = 58,
    WorldMapListRift = 59,
    WorldMapListUtility = 60,
    WorldMapEditorData = 61,
    UIWidgets = 62,
    CurrentLevelDefinition = 63,
    BoardSaveState = 64,
    OtherGameObjects = 65,
    OtherBoardEntities = 66,
    Zombies = 67,
    Creatures = 68,
    Projectiles = 69,
    Plants = 70,
    PlantGroups = 71,
    Collectables = 72,
    GridItems = 73,
    EffectObjects = 74,
    StandaloneEffects = 75,
    PopAnimRigs = 76,
    GameSubSystems = 77,
    ActiveLevelModules = 78,
    ActiveWaveActions = 79,
    PowerupInstances = 80,
    ZenGardenEntities = 81,
    ZGStandaloneEffects = 82,
    ZGEffectsLayeredAboveUI = 83,
    PlayerProfiles = 84,
    ProgressionData = 85,
    HotUIWidgets = 86,
    HotUIComponents = 87,
    HotUIPropertySheets = 88,
    Fonts = 89,
    FontsAliases = 90,
    Colors = 91,
    ColorsAliases = 92,
    ServerConfig = 93,
    LootSaveData = 94,
    ProfileLocalSaveData = 95,
    GlobalSaveData = 96,
    DraperSaveData = 97,
    LODConfig = 98,
    AwardConfig = 99,
    EASquaredConfig = 100,
    ArcadeConfig = 101,
    MagentoFilter = 102,
    LootModifierSchedule = 103,
    QuestRewards = 104,
    LevelGen = 105,
    GridItemProps = 106,
    JoustConfig = 107,
    JoustSchedule = 108,
    JoustSeasonSchedule = 109,
    JoustSeasonRewards = 110,
    JoustLevels = 111,
    JoustLeaderboards = 112,
    JoustCrownRewards = 113,
    JoustProgressiveRewards = 114,
    JoustTournamentRewards = 115,
    RiftSchedule = 116,
    PerkProperties = 117,
    Perks = 118,
    PerkProgression = 119,
    RiftEventConfig = 120,
    PerkSubEvents = 121,
    RiftConfig = 122,
    RiftZombossRewards = 123,
    RiftLevelUnlocks = 124,
    RiftFirstClearRewards = 125,
    CalendarSchedule = 126,
    CalendarConfig = 127,
    CalendarThemes = 128,
    ThymedEventTrackDefinitions = 129,
    ThymedEventSchedule = 130,
    LocalizedStrings = 131,
    OttoBot = 132,
    JoustProfile = 133,
    LODEvents = 134,
    LODHolidays = 135,
    LODRewards = 136,
    EventSchedule = 137,
    Autocado = 138,
    ZombieSwapLists = 139,
    ZombieSwapProperties = 140,
    AllowedUniversalLinks = 141
};

class PVZDB
{
public:
    static PVZDB* GetInstance() {
        return *(PVZDB**)GetActualOffset(0x1CDC5F8);
    }

    // res should be an array with size 5
    void GetAllInstancesOfType(int* res, RtTypeId type) {
        typedef void (*getAllInstances)(int*, PVZDB*, RtTypeId);
        ((getAllInstances)getActualOffset(0xDDCEB4))(res, this, type);
    }
    
    template<typename T>
    static T* GetInstance(int* instancesArray) {
        RtWeakPtr<T> instancePtr;
        typedef void (*getInstancePtr)(RtWeakPtr<T>*, int*);
        ((getInstancePtr)getActualOffset(0x10C5EF8))(&instancePtr, instancesArray);

        return instancePtr.get();
    }

    static void Increment(int* instancesArray) {
        typedef void (*sub10C5F58)(int*);
        ((sub10C5F58)getActualOffset(0x10C5F58))(instancesArray);
    }
};

