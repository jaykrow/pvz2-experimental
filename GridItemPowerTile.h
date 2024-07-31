#pragma once
#include "GridItem.h"

enum PowerTileClass {
	alpha = 0,
	beta = 1,
	gamma1 = 2, // idk why this is a "redefinition of gamma". Fix later
	delta = 3,
	epsilon = 4
};

class GridItemPowerTileProps : public GridItemAnimationProps {
public:
	PowerTileClass TileClass;
	Color TileColor;
};

static_assert(sizeof(GridItemPowerTileProps) == 0xA0);

class GridItemPowerTile : public GridItemAnimation
{
public:
	bool m_isPlantFoodActive;
	int unk;

	DEFINE_RT_CLASS_GET_CLASS_WRAPPER(0x906B78);
};

static_assert(sizeof(GridItemPowerTile) == 0x108);

GridItemPowerTile* GetPowerTileAt(int gridX, int gridY);
