#pragma once
#include "UIWidget.h"

enum PacketFlag {
	PACKETFLAG_NONE = 0,
	PACKETFLAG_AFFORDABLE = 1,
	PACKETFLAG_DISABLED = 2,
	PACKETFLAG_EXCLUDELISTED = 4,
	PACKETFLAG_ALWAYS_PLANTABLE = 8,
	PACKETFLAG_ON_DISPLAY = 0x10,
	PACKETFLAG_FREE_PLANT = 0x20,
	PACKETFLAG_FLASHING = 0x40,
	PACKETFLAG_SELECTED = 0x80,
	PACKETFLAG_IMITATED = 0x100,
	PACKETFLAG_PURCHASABLE = 0x200,
	PACKETFLAG_CLAIMABLE = 0x400,
	PACKETFLAG_UNOWNED_ON_SPINE = 0x800,
	PACKETFLAG_PURCHASABLE_FOR_GEMS = 0x1000,
	PACKETFLAG_RENTED = 0x2000,
	PACKETFLAG_FAVORITE = 0x4000,
	PACKETFLAG_ENVELOPE = 0x8000,
	PACKETFLAG_SHOW_LEVEL = 0x10000,
	PACKETFLAG_UNLOCKABLE_FOR_PACKETS = 0x80000
};

class SeedPacket : public UIWidget
{
public:
	int unused;
	RtWeakPtr<void> m_plantType;
	SexyString m_contentsTypeName;
	int m_packetFlags;
	float m_cooldownEndTime;
	int m_flashingColor;
	int unk;
	bool m_usesCooldownTimer;
	bool unusedBool;
	bool m_useBoostedBackground;
	bool m_renderBackground;
	bool unusedBool2;
	bool m_shouldShowLevel;
	bool m_shouldShowXp;
	char pad13B[9];
	int m_scoreBonusFactor;
	bool m_shouldShowBonusFactor;
	int m_plantLevel;
	int m_plantMasteryLevel;
	int m_plantLevelBadgeTier;
	bool m_preselected;
	bool m_skipRVentIconRendering;
	char pad15A[0x2E];
};

static_assert(sizeof(SeedPacket) == 0x188);
static_assert(offsetof(SeedPacket, m_packetFlags) == 0x124);
static_assert(offsetof(SeedPacket, m_shouldShowLevel) == 0x139);
static_assert(offsetof(SeedPacket, m_scoreBonusFactor) == 0x144);
static_assert(offsetof(SeedPacket, m_preselected) == 0x158);

