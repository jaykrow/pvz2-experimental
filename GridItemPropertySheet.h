#pragma once
#include "PropertySheetBase.h"

class GridItemPropertySheet : public PropertySheetBase
{
public:
	float Hitpoints;
	int HitRectOffsetX;
	int HitRectOffsetWidth;
	int Height;
	bool CanBeMowed;
	char PlantingRestrictions[0x14];
	int unk;
};

static_assert(sizeof(GridItemPropertySheet) == 0x48);

class GridItemAnimationProps : public GridItemPropertySheet {
public:
	SexyString PopAnim;
	SexyString PopAnimRigClass;
	SexyVector2 PopAnimRenderOffset;
	SexyVector2 PopAnimRenderScale;
	SexyString LinkedPopAnim;
	SexyVector3 LinkedOffset;
	int LinkedLayerOffset;
};

static_assert(sizeof(GridItemAnimationProps) == 0x8C);

class GridItemGravestonePropertySheet : public GridItemAnimationProps {
public:
	int DamageStateCount;
	SexyString BreakEffect;
	SexyString BreakEffectSound;
	SexyVector2 ArtCenter;
	bool CanBeEatenByGraveBuster;
	float GraveBusterEatTimeOverride;
	Point GridExtents;
	bool IsAnimated;
};

static_assert(sizeof(GridItemGravestonePropertySheet) == 0xC4);

class GridItemSurfboardProps : public GridItemGravestonePropertySheet {
public:
	int CanBeEatenByGraveBusterAtLevel;
};
