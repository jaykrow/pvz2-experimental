#pragma once
#include "ModularRealObject.h"
#include "BoardEntity.h"
#include "DamageInfo.h"
#include "PropertySheetBase.h"

// #define PROJECTILE_NUM_VIRTUALFUNCS 53

// #define PROJECTILE_COLLIDEWITHOBJECT_INDEX 35
// #define PROJECTILE_AFFECTTARGET_INDEX 47
// #define PROJECTILE_FUNCTION_COLLIDEWITHOBJECT_OFFSET 0x909AE0

using namespace Sexy;

class ZombieConditionEntry {
	ZombieConditions Condition;
	ValueRange Duration;
};

enum CollisionTypeFlags {
	none = 0,
	groundzombies = 1,
	offgroundzombies = 2,
	dyingzombies = 4,
	allzombies = 7,
	griditems = 8,
	lowplants = 0x20,
	normalplants = 0x40,
	allplants = 0x80,
	plants = 0xF0,
	ground = 0x100,
	instigator = 0x200,
	everything = 0x3FF
};

class ProjectilePropertySheet : public PropertySheetBase {
public:
	SexyString ClassName;
	float BaseDamage;
	float HealAmount;
	char pad0030[0x10];
	float SplashDamage;
	float StunDuration;
	float SplashRadius;
	float SplashRadiusBaseDamageCutoff;
	bool ShakeBoardOnSplash;
	bool DiesOnImpact;
	bool HasShadow;
	bool ImpactShowsAtZombieFeet;
	bool RotateToMatchVelocity;
	bool ImpactSoundForce;
	bool FollowsGround;
	ZombieConditions OverrideStunCondition;
	std::vector<ValueRange> InitialVelocity;
	std::vector<ValueRange> InitialAcceleration;
	std::vector<ValueRange> InitialVelocityScale;
	ValueRange InitialHeight;
	ValueRange InitialRotation;
	ValueRange InitialAngularVelocity;
	ValueRange InitialScale;
	SexyString AttachedPAM;
	SexyString AttachedPAMAnimRigClass;
	SexyVector2 AttachedPAMOffset;
	std::vector<SexyString> AttachedPAMAnimationToPlay;
	SexyString ShadowImage;
	SexyString RenderImage;
	Color RenderColor;
	FRect CollisionRect;
	SexyString ImpactSoundEvent;
	float ImpactSoundThrottleTimer;
	SexyString ImpactPAM;
	std::vector<SexyString> ImpactPAMAnimationToPlay;
	std::vector<ValueRange> ImpactOffset;
	SexyString SpawnPAM;
	std::vector<SexyString> SpawnPAMAnimationToPlay;
	std::vector<ValueRange> SpawnPAMOffset;
	std::vector<ZombieConditionEntry> Conditions;
	SexyVector2 AttachedPAMEffectOffset;
	std::vector<CollisionTypeFlags> CollisionFlags;
	std::vector<DamageTypeFlags> DamageFlags;
};

static_assert(sizeof(ProjectilePropertySheet) == 0x188);
static_assert(offsetof(ProjectilePropertySheet, ImpactPAM) == 0x114);
static_assert(offsetof(ProjectilePropertySheet, InitialScale) == 0x98);
static_assert(offsetof(ProjectilePropertySheet, OverrideStunCondition) == 0x58);

class Projectile : public ModularRealObject
{
public:
	bool m_shouldAffectTarget; //0x0038
	bool m_unk0x39; //0x0039
	bool m_unk0x3A; //0x003A
	bool m_unk0x3B; //0x003B
	void* m_propertySheet; //0x003C
	RtWeakPtr<void> m_propertySheetPtr; //0x0040
	RtWeakPtr<RtObject> m_instigator; //0x0048
	int32_t m_instigatorLevel; //0x0050
	int32_t m_instigatorPowerUpFlags; //0x0054
	RtWeakPtr<RtObject> m_instigatorType; //0x0058
	float m_timeStepOverride; //0x0060
	SexyVector3 m_previousPosition; //0x0064
	SexyVector3 m_velocity; //0x0070
	SexyVector3 m_acceleration; //0x007C
	SexyVector3 m_velocityScale; //0x0088
	int32_t m_boardRow; //0x0094
	int32_t m_previousBoardRow; //0x0098
	int32_t m_boardRowHeight; //0x009C
	int32_t m_boardRowFiredOrDeflectedFrom; //0x00A0
	int m_unk0xA4; //0x00A4
	RtWeakPtr<RtObject> m_renderImage; //0x00A8
	int32_t m_renderOrder; //0x00B0
	float m_scale; //0x00B4
	float m_rotation; //0x00B8
	float m_baseRotation; //0x00BC
	float m_angularVelocity; //0x00C0
	int m_unk0xC4; //0x00C4
	bool m_mirrored; //0x00C8
	bool m_collideLeftToRight; //0x00C9
	char pad_00CA[2]; //0x00CA
	float m_damageAmount; //0x00CC
	float m_splashDamageAmount; //0x00D0
	float m_stunDurationAmount; //0x00D4
	int64_t m_damageFlags; //0x00D8
	std::vector<int> m_damageConditions; //0x00E0
	int32_t m_collisionFlagsOverride; //0x00EC
	int32_t m_propertyFlags; //0x00F0
	char pad_00F4[4]; //0x00F4
	RtWeakPtr<RtObject> m_attachedAnimRig; //0x00F8
	class SexyVector2 m_attachedAnimRigOffset; //0x0100
	RtWeakPtr<RtObject> m_target; //0x0108
	char pad_0110[24]; //0x0110
	std::vector<std::string> m_plantFamilies; //0x0128
	float m_resistancePiercingOverride; //0x0134
	int32_t m_plantTier; //0x0138

	template<typename T>
	T* GetProps()
	{
		if (m_propertySheet)
			return reinterpret_cast<T*>(m_propertySheet);

		auto* props = m_propertySheetPtr.get();
		m_propertySheet = props;
		return reinterpret_cast<T*>(props);
	}

	/*void LobAt(const SexyVector3& pos, PultProjectileProps* pultProps)
	{
		typedef void(*func)(Projectile*, const SexyVector3&, PultProjectileProps*);
		func pFunc = (func)GetActualOffset(0x916D68);
		pFunc(this, pos, pultProps);
	}*/

	/*
	void SetTarget(RtWeakPtr<RtObject>* targetPtr)
	{
		//this->m_target.FromOther(targetPtr);
		this->m_target = targetPtr;
	}

	bool CollideWithObject_Base(GameObject* object)
	{
		typedef bool(*func)(Projectile*, GameObject*);
		func pFunc = (func)GetActualOffset(PROJECTILE_FUNCTION_COLLIDEWITHOBJECT_OFFSET);
		return pFunc(this, object);
	}
	*/

	void PlayImpact(BoardEntity* target) {
		typedef void (*PlayImpactFunc)(Projectile*, BoardEntity*);
		((PlayImpactFunc)GetActualOffset(0x914678))(this, target);
	}

	virtual bool CollideWithObject(GameObject* object) {};
	virtual void OnCollision() {};
	virtual void Function37() {};
	virtual void Function38() {};
	virtual void Function39() {};
	virtual void UpdateVelocity() {};
	virtual void Function41() {};
	virtual void Function42() {};
	virtual void Function43() {};
	virtual void Function44() {};
	virtual void Function45() {};
	virtual void Function46() {};
	virtual void AffectTarget(BoardEntity* target) {};
	virtual void Function48() {};
	virtual void Function49() {};
	virtual void GetDamageInfo(DamageInfo* outDmgInfo) {};
	virtual void FindCollisionTargets(std::vector<BoardEntity*>& targets) {};
	virtual void Function52() {};
};
static_assert(offsetof(Projectile, m_shouldAffectTarget) == 0x38);
static_assert(offsetof(Projectile, m_damageAmount) == 0xCC);
static_assert(offsetof(Projectile, m_splashDamageAmount) == 0xD0);
static_assert(offsetof(Projectile, m_plantTier) == 0x138);
static_assert(sizeof(Projectile) == 0x140);
