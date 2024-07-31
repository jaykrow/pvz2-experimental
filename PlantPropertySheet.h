#pragma once
#include "PropertySheetBase.h"
#include "PlantAction.h"

/*
class PlantAction {
	RtWeakPtr<void> Type;
	RtWeakPtr<void> Projectile;
	int ExplodeType;
	float ExplodeRadius;
	int TriggerType;
	bool DiesAfterExplode;
	bool Burns;
	bool SeocndaryAction;
	float InitialMinCooldownTime;
	float CooldownTimeMin;
	float CooldownTimeMax;
	int Damage;
	int SplashDamage;
	float XVelocityOverride;
	int PushbackAmount;
	float ChillDuration;
	float FreezeDuration;
	float StunDuration;
	SexyString ProjectileLaunchSound;
	Rect RectTriggerRange;
	SexyVector2 SpawnOffset;
	int unk;
};
*/

class PlantPropertySheet : public PropertySheetBase
{
public:
	int unk0;
	int unk;
	int unk2;
	int Cost;
	int BeghouledCost;
	float Hitpoints;
	float PacketCooldown;
	float StartingCooldown;
	float PlantfoodDurationSeconds;
	int PlantfoodPlayCount;
	float PlantfoodShineTime;
	SexyString PlantfoodActivationSound;
	SexyString HelpedActivationSound;
	int PlantGridType;
	bool MultiPlantLayer;
	std::vector<PlantAction> Actions;
	char Powers[12]; // PowerSet type
	int GridWidth;
	int GridHeight;
	int GridItemDamage;
	float PlantingOffsetY;
	SexyVector2 CursorPreviewOffset;
	SexyVector2 ArtCenter;
	SexyVector2 ZenGardenOffset;
	float ZenGardenSmallScale;
	float ZenGardenLargeScale;
	Rect HitRectAdjust;
	int Height;
	int DangerRoomWeight;
	bool IsZenGardenTreasurePlant;
	bool IsZenGardenWaterPlant;
	int ZenGardenSproutWeight;
	int ZenGardenBoostCost;
	int ZenGardenDurationOverrideSeconds;
	std::vector<int> Boosts; // std::vector<PlantBoost> type
	Rect RestrictPlantingToGridArea;
	std::vector<int> PlantStats; // std::vector<BoardEntityStat>
	std::vector<SexyString> ValidStages;
	std::vector<SexyString> SuggestionAlts;
	bool CanReciveFirstAid;
	bool IsConsumable;
	bool IsInstant;
	bool CanLiveOnWaves;
	bool CanPlantAgainToUpgrade;
	int IdleAnimationWeights[4];
	RtWeakPtr<void> AnimRigProps;
	bool HidePlantingEffects;
	float MasteryBoostChance;
	float MasteryResistancePiercing;
};

static_assert(sizeof(PlantPropertySheet) == 0x144);
static_assert(offsetof(PlantPropertySheet, PlantGridType) == 0x60);
static_assert(offsetof(PlantPropertySheet, ArtCenter) == 0x98);
static_assert(offsetof(PlantPropertySheet, IdleAnimationWeights) == 0x120);
