#pragma once
#include "PvZ2/RtWeakPtr.h"
#include "GridItemPropertySheet.h"
#include "BoardEntity.h"
#include "EntityConditionTracker.h"
#include "PopAnimRig.h"

class GridItem : public BoardEntity
{
public:
	int unk;
	EntityConditionTracker m_conditionTracker;
	char unk2[76];
	RtWeakPtr<void> m_type;
	float m_health;
	float m_healthMax;
	int gridX;
	int gridY;
	RtWeakPtr<GridItemPropertySheet> props;
};

static_assert(sizeof(GridItem) == 0xF0);
static_assert(offsetof(GridItem, unk2) == 0x84);
static_assert(offsetof(GridItem, m_conditionTracker) == 0x68);

class GridItemAnimation : public GridItem {
public:
	bool m_hasLinkedAnimRig;
	int unk3;
	RtWeakPtr<PopAnimRig> m_animRig;
};

static_assert(sizeof(GridItemAnimation) == 0x100);

class GridItemGravestone : public GridItemAnimation {
	int m_state;
	int m_currDamageState;
	char groundEffect[24];
	float m_stateTime;
	float m_stateDuration;
	float m_eateNprogress;
	int m_damageStates;
};

static_assert(sizeof(GridItemGravestone) == 0x130);
