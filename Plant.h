#pragma once
#include "BoardEntity.h"
#include "Projectile.h"
#include "PlantConditionTracker.h"
#include "PopAnimRig.h"
#include "PlantAction.h"

enum PlantState
{
	STATE_NOTREADY,
	STATE_READY,
	STATE_DOINGSPECIAL,
	STATE_BEINGREMOVED,
	STATE_IDLE,
	STATE_PLANTFOOD,
	STATE_BEING_HELPED,
	STATE_EXHAUSTED,
	STATE_CHARGING,
	STATE_CYCLING,
	STATE_FRAMEWORK_BEGIN
};

class Plant;

class PlantFramework : public Sexy::RtObject
{
public:
	Plant* mPlant;

	virtual void Func7() {};
	virtual void Func8() {};
	virtual void Func9() {};
	virtual void RenderAnimRigs(Graphics* g) {};
	virtual void Func11() {};
	virtual void Func12() {};
	virtual void Func13() {};
	virtual void SetupAnimRigFiringCallbacks(PopAnimRig* rig) {}; // should be changed to PlantAnimRig later
	virtual void PlayChompSound() {};
	virtual void Die() {};
	virtual Projectile* CreateProjectile() {};
	virtual float GetLaunchIntervalMultiplier() {};
	virtual std::vector<RtWeakPtr<void>> FindAttackTargets(char unk1, int unk2) {};
	virtual void Func21() {};
	virtual void Func22() {};
	virtual void Func23() {};
	virtual void Func24() {};
	virtual void Func25() {};
	virtual void Func26() {};
	virtual SexyVector3 GetProjectileOffset(PlantAction* action, float scale) {};
	virtual int GetPlantfoodPlayCount() {};
	virtual void Func29() {};
	virtual void Func30() {};
	virtual void Func31() {};
	virtual void Func32() {};
	virtual void Func33() {};
	virtual void Func34() {};
	virtual void Func35() {};
	virtual void Func36() {};
	virtual bool HasPlantFoodEnded() {};
	virtual void FirePlantFood() {};
	virtual bool IsInPlantFoodState() {};
	virtual void Func40() {};
	virtual void ActivatePlantFood() {};
	virtual void TriggerPlantFoodFlash() {};
	virtual void StartExhausted(float duration) {};
	virtual void FinishExhausted() {};
	virtual bool IsExhausted() {};
	virtual void Func46() {};
	virtual void PlayCharging() {};
	virtual void PlayCycling() {};
	virtual void Func49() {};
	virtual void Func50() {};
	virtual void PlayBeingHelped() {};
	virtual bool NotDeactivated() {}; // check if not being helped nor exhausted
	virtual void Func53() {};
	virtual void PlayHelpedOff() {}; // set state to READY
	virtual bool IsBeingHelped() {};
	virtual void Func56() {};
	virtual void Func57() {};
	virtual void Func58() {};
	virtual void PlayAttackBasedOnTier() {};
	virtual void Func60() {};
	virtual void Func61() {};
	virtual void Func62() {};
	virtual void Func63() {};
	virtual void Func64() {};
	virtual void Func65() {};
	virtual bool CanSink() {};
	virtual float GetSinKPercentage() {};
	virtual void Func68() {};
	virtual void Func69() {};
	virtual void Func70() {};
	virtual void Func71() {};
	virtual void Func72() {};
	virtual void Func73() {};
	virtual Rect GetActionTriggerRectGridCoords(int actionIdx) {};
	virtual Rect GetActionTriggerRect(int actionIdx) {};
	virtual void Func76() {};
	virtual void OnDamaged() {}; // TODO: I'm not sure the fields and return value for this
	virtual void Func78() {};
	virtual SexyString GetProjectileLaunchSound() {};
	virtual int GetHeight() {};
	virtual void AttemptKill() {};
	virtual void Func82() {};
	virtual bool IsInvincible() {};
	virtual void Func84() {};
	virtual void Func85() {};
	virtual void Func86() {}; // TODO: this one is for use_action frames
	virtual void Func87() {};
	virtual void Func88() {};
	virtual void Func89() {};
	virtual void Func90() {};
	virtual float GetCost() {};
	virtual void Func92() {};
	virtual void Func93() {};
	virtual void Func94() {};
	virtual void Func95() {};
	virtual void Func96() {};
	virtual void Func97() {};
	virtual void PlantfoodStartedCallback() {}; // TODO: Check parameters for these 3
	virtual void PlantfoodLoopedCallback() {};
	virtual void PlantfoodEndedCallback() {};
	virtual void Func101() {};
	virtual void Func102() {};
	virtual void Func103() {};
	virtual void Func104() {};
	virtual void Func105() {};
	virtual void Func106() {};
	virtual void Func107() {};
	virtual void Func108() {};
	virtual void Func109() {};
	virtual void Func110() {};
	virtual bool CanPlantBasedOnCellType() {};
	virtual void Func112() {};
	virtual void Func113() {};
	virtual void Func114() {};
	virtual void Func115() {};
	virtual void Func116() {};
	virtual void Func117() {};
	virtual void Func118() {};
	virtual void SetCollisionRect() {}; // Idk the return value of this one
	virtual void Func120() {};
};

class Plant : public BoardEntity
{
public:
	PlantFramework* plantFramework;
	ValueRange m_initialLaunchInterval;
	ValueRange m_launchInterval;
	float m_baseScale;
	uint32_t m_state;
	int32_t m_plantFlags;
	float m_PlantHealth;
	float m_overrideMinimumMaxHealth;
	float m_speedModifier;
	int32_t m_lastDamageType;
	int m_unk0x94;
	int32_t m_row;
	int32_t m_column;
	bool m_isOnBoard;
	bool m_isShooter;
	bool m_mirror;
	bool m_isDead;
	float m_disappearTime;
	float m_doSpecialTime;
	float m_stateEndTime;
	float m_launchTime;
	float m_plantFoodEndtime;
	float m_timeTillNextDamageFlash;
	float m_fadeInDuration;
	float m_fadeInEndTime;
	float m_sproutScalingDuration;
	float m_sproutScalingEndTime;
	char pad_00CC[8];
	int32_t m_damageStates;
	int32_t m_currentDamageState;
	int32_t m_currentPlantActionIdx;
	RtWeakPtr<int> m_type;
	RtWeakPtr<PopAnimRig> m_animRig;
	RtWeakPtr<PopAnimRig> m_plantFoodShine;
	bool m_isInPlantFoodState;
	bool m_protectedFromShovel;
	bool m_isDuplicate;
	char pad_00FB[1];
	int32_t m_level;
	int32_t m_masteryLevel;
	int32_t m_powerUpFlags;
	RtWeakPtr<void> m_propertySheet;
	int32_t m_multiPlantGridLayer;
	int32_t m_multiPlantDrawLayer;
	bool m_relocating;
	char pad_0119[3];
	float m_relocateFromX;
	float m_relocateFromY;
	float m_relocateToX;
	float m_relocateToY;
	float m_relocateApexHeight;
	struct SexyVector2 m_relocationOffsetFromDestination;
	float m_relocateStartTime;
	float m_relocateEndTime;
	int32_t m_relocationType;
	char pad_0144[4];
	int32_t m_groundEffect;
	char pad_014C[20];
	PlantConditionTracker m_conditionTracker;
	int32_t m_attachedBoardEntities;
	char pad_01CC[12];
	int32_t m_attachedEffects;
	char attachedEffectsVec[12];

	template <typename T>
	T* GetProps()
	{
		RtWeakPtr<void> propSheetPtr = m_propertySheet;

		T* props = reinterpret_cast<T*>(propSheetPtr.get());
		return props;
	}

	/*template <typename T>
	T* CreateProjectile()
	{
		typedef Projectile* (*func)(Plant*);
		func pFunc = (func)GetActualOffset(0xE952D8);
		return reinterpret_cast<T*>(pFunc(this));
	}*/

	DEFINE_RT_CLASS_GET_CLASS_WRAPPER(0xE7BE90);
};

static_assert(offsetof(Plant, m_type) == 0xE0);
static_assert(offsetof(Plant, m_conditionTracker) == 0x160);
static_assert(sizeof(Plant) == 0x1E8);