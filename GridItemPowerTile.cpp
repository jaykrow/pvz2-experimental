#include "pch.h"
#include "GridItemPowerTile.h"
#include "PvZ2/Board.h"

GridItemPowerTile* GetPowerTileAt(int gridX, int gridY) {
	std::vector<GridItem*> res;

	GetBoard()->GetGridItemsAt(gridX, gridY, &res);
	for (auto& gridItem : res) {
		if (gridItem->IsType(GridItemPowerTile::GetRTClass())) {
			return (GridItemPowerTile*) gridItem;
		}
	}
}