#include "RailAltHooks.h"
#include "PvZ2/Board.h"
#include "memUtils.h"
#include "GridItem.h"
#include "assert.h"
#include <vector>
#include "GridItemRailAlt.h"
#include "GridItemRailAltProps.h"

bool doesRailAllowDirection(GridItemRailAlt* rail, int dirX, int dirY) {
	// LOGI("Does rail allow direction %d %d", dirX, dirY);
	RtWeakPtr<GridItemPropertySheet> propsCopy;
	propsCopy.FromOther(&rail->props);

	int props = (int)propsCopy.get();
	
	typedef int (*isTypeFun)(int, int);
	isTypeFun checkType = *(isTypeFun*)(*(int*)props + 0x10);
	int railPropsType = (int)GridItemRailAltProps::getRTClass();
	if (checkType(props, railPropsType)) {
		GridItemRailAltProps* realProps = (GridItemRailAltProps*)props;
		int dirType = realProps->dirType;

		int curDir;
		if (dirY == -1) curDir = 0;
		else if (dirX == 1) curDir = 1;
		else if (dirY == 1) curDir = 2;
		else if (dirX == -1) curDir = 3;
		else curDir = 4;

		// LOGI("curDir = %d allowed dirs = %d", curDir, dirType);

		if (dirType & (1 << curDir)) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return true;
	}
}

bool canMoveInDirection(int gridX, int gridY, int dirX, int dirY) { // does the rail allow movement to here?
	// LOGI("at %d %d, can we move in direction %d %d?", gridX, gridY, dirX, dirY);
	std::vector<int> targets;

	typedef void (*getGridItems)(std::vector<int>*, int, int, int);
	((getGridItems)getActualOffset(0x50F97C))(&targets, 4, gridX, gridY);

	int railType = (int)GridItemRailAlt::getRTClass();
	for (int gridItem : targets) {
		typedef int (*isTypeCheck)(int, int);
		isTypeCheck checkType = *(isTypeCheck*)(*(int*)gridItem + 0x10);
		if (checkType(gridItem, railType)) {
			return doesRailAllowDirection((GridItemRailAlt*)gridItem, dirX, dirY);
		}
	}
}

bool checkCartCanMoveTo(int gridX, int gridY) {
	std::vector<int> targets;

	typedef void (*getGridItems)(std::vector<int>*, int, int, int);
	((getGridItems)getActualOffset(0x50F97C))(&targets, 4, gridX, gridY);

	int railType = (int)GridItemRailAlt::getRTClass();
	typedef int (*getCartType)();
	int cartType = ((getCartType)getActualOffset(0x9A4D14))();

	bool hasRail = false;

	for (int gridItem : targets) {
		typedef int (*isTypeCheck)(int, int);
		isTypeCheck checkType = *(isTypeCheck*)(*(int*)gridItem + 0x10);
		if (checkType(gridItem, railType)) {
			hasRail = true;
		}
		if (checkType(gridItem, cartType)) {
			return false;
		}
	}

	return hasRail;
}

typedef void (*gridItemRailcartMove)(GridItem*, int);
gridItemRailcartMove oGridItemRailcartMove = NULL;

void hkGridItemRailcartMove(GridItem* railcart, int theTouch) {
	int gridX = railcart->gridX;
	int gridY = railcart->gridY;
	oGridItemRailcartMove(railcart, theTouch);
	int newX = railcart->gridX;
	int newY = railcart->gridY;

	// LOGI("moved past original code");

	if (gridX != newX || gridY != newY) {
		return; // the railcart has moved based on the old system
	}

	// LOGI("old system doesn't exist");

	// check for our new system
	int touchX = *(int*)(theTouch + 0x8);
	int touchY = *(int*)(theTouch + 0xC);

	// LOGI("found touch positions");
	
	typedef float (*sub3777B8)(int, float);
	float mY = ((sub3777B8)getActualOffset(0x3777B8))(getSexyApp(), (float)touchY);
	float mX = ((sub3777B8)getActualOffset(0x3777B8))(getSexyApp(), (float)touchX);
	
	gridY = (mY - GRID_TILE_TOP) / GRID_TILE_HEIGHT;
	gridX = (mX - GRID_TILE_LEFT) / GRID_TILE_WIDTH;

	gridY = std::max(gridY, 0);
	gridX = std::max(gridX, 0);
	gridY = std::min(gridY, getBoard()->m_rowCount - 1);
	gridX = std::min(gridX, getBoard()->m_columnCount - 1);

	int railX = railcart->gridX;
	int railY = railcart->gridY;

	// prioritize y changes over x
	int dirX = 1, dirY = 1;
	if (gridY < railY) {
		dirY = -1;
		dirX = 0;
	}
	else if (gridY > railY) {
		dirY = 1;
		dirX = 0;
	}
	else if (gridY == railY) {
		dirY = 0;
		if (gridX < railX) {
			dirX = -1;
		}
		else if (gridX > railX) {
			dirX = 1;
		}
		else {
			dirX = 0;
		}
	}

	// LOGI("got the directions to go: %d %d", dirX, dirY);

	if (dirX != 0 || dirY != 0) {
		assert(dirX == 0 || dirY == 0);
		// check if rail exists at next position
		int railAltType = (int)GridItemRailAlt::getRTClass();
		if (checkCartCanMoveTo(dirX + railX, dirY + railY) && canMoveInDirection(railX, railY, dirX, dirY)) {
			railX += dirX;
			railY += dirY;
		}
	}

	// LOGI("check cart can move to");

	if (railcart->gridX == railX && railcart->gridY == railY) {
		// the cart stayed still, nothing more to do
		return;
	}

	// LOGI("cart doesn't stay still. Old %d %d new %d %d", railcart->gridX, railcart->gridY, railX, railY);
	
	// the cart move. We have to move the PlantGroup as well
	typedef int (*getBoardPlanting)();
	int boardPlanting = ((getBoardPlanting)getActualOffset(0x2AD5A0))();
	int curCoords[2] = { railcart->gridX, railcart->gridY };
	int newCoords[2] = { railX, railY };

	// LOGI("got board planting");

	typedef int (*sub6485EC)(int, int*);
	// move the plants on cart
	int v11 = ((sub6485EC)getActualOffset(0x6485EC))(boardPlanting, curCoords);
	// LOGI("got the plant group here");
	if (v11) { // this is a plantGroup for sure
		RtWeakPtr<int> v12 = RtWeakPtr<int>();
		v12.FromOther((RtWeakPtr<int>*)(v11 + 8));
	
		typedef int (*sub9A5E58)(GridItem*, RtWeakPtr<int>*);
		int v13 = ((sub9A5E58)getActualOffset(0x9A5E58))(railcart, &v12);
		// LOGI("got v13 = %d", v13);
		if (v13) {
			return;
		}

		std::vector<RtWeakPtr<int>> v37; // this is a vector of RtWeakPtr
		typedef int (*sub6750D0)(std::vector<RtWeakPtr<int>>*, int*);
		((sub6750D0)getActualOffset(0x6750D0))(&v37, (int*)(v11 + 0x70));

		// LOGI("got the plants size = %d", v37.size());

		for (RtWeakPtr<int> plant : v37) {
			int realPlant = (int)plant.get();
			// LOGI("got real plant");
			typedef int (*sub9A62B8)(GridItem*, int);
			if (!((sub9A62B8)getActualOffset(0x9A62B8))(railcart, realPlant)) {
				// LOGI("can move plant");
				typedef void (*sub648C24)(int, int, int*, int); // move plant to new coords
				((sub648C24)getActualOffset(0x648C24))(boardPlanting, realPlant, newCoords, 1);
				// LOGI("plant moved");
			}
		}
	}

	// LOGI("moved all plants");

	// move the GIs on cart, including the cart itself
	std::vector<int> targets;
	typedef void (*getGridItems)(std::vector<int>*, int, int, int);
	((getGridItems)getActualOffset(0x50F97C))(&targets, 4, railcart->gridX, railcart->gridY);

	// LOGI("grid items got");

	int railType = (int)GridItemRailAlt::getRTClass();
	for (int gridItem : targets) {
		//RtWeakPtr<int> v36 = RtWeakPtr<int>();
		//v36.FromOther((RtWeakPtr<int>*)(gridItem + 0x34)); // idk what this section is even for
		//int realGridItem = (int) v36.get();

		typedef int (*isTypeCheck)(int, int); // don't move the rails
		isTypeCheck checkType = *(isTypeCheck*)(*(int*)gridItem + 0x10);
		if (checkType(gridItem, railType)) {
			continue;
		}

		typedef int (*sub9A638C)(GridItem*);
		int v33 = ((sub9A638C)getActualOffset(0x9A638C))(railcart);
		if (!v33) {
			typedef void (*sub4A72D4)(int, int*, int); // move grid item to new coords
			((sub4A72D4)getActualOffset(0x4A72D4))(gridItem, newCoords, 1);
		}
	}

	// LOGI("moved all grid items");

	typedef int (*sub72E520)(Board*, int, GridItem*);
	((sub72E520)getActualOffset(0x72E520))(getBoard(), theTouch, railcart);

	typedef int (*getAudioMgr)();
	int audioMgr = ((getAudioMgr)getActualOffset(0x279750))();
	typedef int (*playSound)(int, SexyString*);
	SexyString audioName = "Play_UI_Game_RailRoadTrack_Move";
	((playSound)getActualOffset(0x6BEA78))(audioMgr, &audioName);

	// LOGI("audio played");

	int gMessageRouter = *(int*)getActualOffset(0x1C85050);
	typedef int* (*getListeners)(int, int);
	getListeners fun = *(getListeners*)(*(int*)gMessageRouter + 0xC);
	// LOGI("function got");
	int* v27 = fun(gMessageRouter, getActualOffset(0x9A4D10));
	// LOGI("listeners list got");
	typedef int (*sub9A6734)(int, int, int*, GridItem*);
	((sub9A6734)getActualOffset(0x9A6734))(gMessageRouter, getActualOffset(0x9A4D10), v27, railcart);

	// LOGI("processed all listeners");
}

void initRailAlt() {
	PVZ2HookFunction(0x9A5920, (void*)hkGridItemRailcartMove, (void**)&oGridItemRailcartMove, "GridItemRailcart::Move");
}
