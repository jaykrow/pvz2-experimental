#pragma once

#include "BoardEntity.h"
#include "ZombieAnimRig.h"

static uint ZOMBIEPARTICLE_VTABLE_ADDR = 0x1BD0C84;

class ZombieParticle : public BoardEntity
{
public:
	int unk;
	RtWeakPtr<ZombieAnimRig> m_zombieRig;
	float m_rotation;
	float m_currentRotation;
	float m_scale;
	float m_shadowScale;
	float m_motionBeginTime;
	float m_fadeOutBeginTime;
	bool m_settled;
	Rect m_newParticleRect;
	SexyVector3 m_velocity;
	SexyVector3 m_acceleration;
	Rect m_spriteRectInZombie;
	SexyString m_particleName;
	int m_motionType;
	int m_attributes;
	char pad0D8[8];
};

static_assert(sizeof(ZombieParticle) == 0xE0);
static_assert(offsetof(ZombieParticle, m_settled) == 0x88);
static_assert(offsetof(ZombieParticle, m_newParticleRect) == 0x8C);

