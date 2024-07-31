#include "pch.h"
#include "TideChecker.h"
#include "PvZ2/RtWeakPtr.h"
#include "assert.h"
#include "PvZ2/Board.h"
#include "GridItemPuddle.h"
#include <vector>

const int GRID_HEIGHT = 76;
const int GRID_WIDTH = 64;
const int GRID_TOP = 160;
const int GRID_LEFT = 200;

const int NO_TIDE_REGION = 1;

int poolCellsCount = 13;
int poolCells[][2] = { {3, 0}, {3, 1}, {3, 2}, {3, 3}, {7, 0}, {8, 0}, {7, 2}, {7, 3}, {8, 4}, {8, 3}, {9, 3}, {10, 3}, {11, 3} };

/*
int poolCellsCount = 18;
int poolCells[][2] = { {0,2},{1,2},{2,2},{3,2},{4,2},{5,2},{6,2},{7,2},{8,2}, 
					{0, 3},{1,3},{2,3},{3,3},{4,3},{5,3},{6,3},{7,3},{8,3} };
*/

bool isPosInPoolCell(SexyVector3* vec) {
	float mX = vec->mX;
	float mY = vec->mY;

	if (mX < GRID_LEFT || mY < GRID_TOP) {
		return false;
	}

	/*for (int i = 0; i < poolCellsCount; ++i) {
		int* cell = poolCells[i];
		int cellX = cell[0], cellY = cell[1];
		int x1 = GRID_LEFT + cellX * GRID_WIDTH, x2 = x1 + GRID_WIDTH;
		int y1 = GRID_TOP + cellY * GRID_HEIGHT, y2 = y1 + GRID_HEIGHT;

		if (mX >= x1 && mX <= x2 && mY >= y1 && mY <= y2) {
			return true;
		}
	}*/

	int gridX = (mX - GRID_LEFT) / GRID_WIDTH;
	int gridY = (mY - GRID_TOP) / GRID_HEIGHT;
	if (hasPuddleAt(gridX, gridY)) {
		return true;
	}

	return false;
}

typedef int (*fun18)(int, SexyVector3*, int);
fun18 oFun18 = NULL;

int res18 = -1;

int hkFun18(int board, SexyVector3* vec, int flag) {
	if (flag != 2) {
		return oFun18(board, vec, flag);
	}

	res18 = isPosInPoolCell(vec); // I no longer have to hijack the results

	bool res18Saved = res18;
	int finalRes = oFun18(board, vec, flag);
	if (res18Saved) {
		if (finalRes == 0) { // we are running on a non BWB map
			finalRes = NO_TIDE_REGION;
		}
		assert(finalRes != 0); // this check will crash for non BWB levels because they have no tide region
	}
	/*else {
		assert(finalRes == 0);
	}*/ // special code to make tide not work
	// LOGI("72A6D0 called %f %f result %d finalRes = %u addr = %u", vec->mX, vec->mY, res18Saved, (uint)finalRes, getOriginalOffset(*(uint*)finalRes + 0x40));
	return finalRes;
}

typedef int (*fun17)(int, SexyVector3*, int);
fun17 oFun17 = NULL;

int hkFun17(int board, SexyVector3* vec, int a3) {
	float ogmX = vec->mX;
	float ogmY = vec->mY;
	if (res18 == true) {
		vec->mX = GRID_LEFT + 10 * GRID_WIDTH;
		vec->mY = GRID_TOP + GRID_HEIGHT;
		// this should guarantee vec falls into the tide region (for BWB only)
	}
	//else if (res18 == false) {
	//	vec->mX = GRID_LEFT - GRID_WIDTH; // guarantee it's out of tide
	//} // special code to make tide not work
	res18 = -1;

	int res = oFun17(board, vec, a3);
	vec->mX = ogmX;
	vec->mY = ogmY;
	return res;
}

float getPuddlesSinkPercentage(SexyVector3* a1) {
	float mX = a1->mX;
	float mY = a1->mY;

	if (mX < GRID_LEFT || mY < GRID_TOP) {
		// zombies are out of bounds 
		return 0.0;
	}

	int gridX = (mX - GRID_LEFT) / GRID_WIDTH;
	int gridY = (mY - GRID_TOP) / GRID_HEIGHT;
	if (!hasPuddleAt(gridX, gridY)) {
		return 0.0; // should never happen anyways
	}

	int leftBound = GRID_LEFT + GRID_WIDTH * gridX;
	int rightBound = leftBound + GRID_WIDTH;

	const int MAGIC = 5;

	// Make sink and float anim on the 20% of the sides
	if (MAGIC * (mX - leftBound) < GRID_WIDTH) {
		// it's closer to the left, start checking to make floating up
		// 
		// if puddle on the left then no floating
		if (hasPuddleAt(gridX - 1, gridY)) {
			return -1.0; // no floating up
		}
		float dist = (mX - leftBound) / GRID_WIDTH;
		return -dist * MAGIC; // we can interpolate with better functions later
	}

	if (MAGIC * (rightBound - mX) < GRID_WIDTH) {
		if (hasPuddleAt(gridX + 1, gridY)) {
			return -1.0;
		}
		float dist = (rightBound - mX) / GRID_WIDTH;
		return -dist * MAGIC;
	}

	return -1.0;
}

typedef float (*fun32)(int, SexyVector3*, int);
fun32 oFun32 = NULL;

float hkFun32(int a1, SexyVector3* a2, float a3) {
	// float res = oFun32(a1, a2, a3);
	// LOGI("Fun 32 called position at %f %f res = %f", a2->mX, a2->mY, res);

	Board* board = getBoard();
	uint v7 = (uint) hkFun18((int) board, a2, 2);
	float v11 = 0.0;
	if (v7) {
		/*if (a2->mX == 424 && a2->mY == 440) {
			LOGI("Fun 32 called position at %f %f %u", a2->mX, a2->mY, v7);
		}*/
		float v8 = a3;

		float v9 = 0.0;
		if (v7 != NO_TIDE_REGION) {
			typedef float (*v9Fun)(int*, SexyVector3*);
			v9 = ((v9Fun)getActualOffset(0x4ADA04))((int*)v7, a2); // this tries to animate the exiting water anim. Our hacky coordinates will fail outside of tide
		}
		
		// if v7 = NO_TIDE_REGION, we are running on a non-BWB map and the zombie must be subemrged in a puddle
		if (v9 == 0) {
			// v9 = -1.0; // force the zombies to sink since the if check said they should be in water
			v9 = getPuddlesSinkPercentage(a2);
		}
		
		typedef float (*v10Fun)(int);
		float v10 = (*(v10Fun*)(*(uint*)a1 + 0x7C))(a1);

		Rect v13;
		typedef void (*temp)(Rect*, int);
		(*(temp*)(*(uint*)a1 + 0x50))(&v13, a1);

		v11 = v9 * ((v10 * v13.mHeight) - v8);
	}

	return v11;
}

typedef void (*fun33)(int, int, int, int, int);
fun33 oFun33 = NULL;

void hkFun33(int a1, int a2, int a3, int a4, int a5) {
	LOGI("Planting bad reason = %d", a5);
	oFun33(a1, a2, a3, a4, a5);
}

typedef int (*fun34)(int*, int*, int, int, int);
fun34 oFun34 = NULL;

int hkFun34(int* gridItem, int* gridCoords, int a3, int a4, int a5) {
	// LOGI("Checking canPlant gridItem coords = %d %d gridCoords = %d %d plantType = %s", gridItem[0x38], gridItem[0x39], gridCoords[0], gridCoords[1], "a");
	return oFun34(gridItem, gridCoords, a3, a4, a5);
}

typedef void (*gridItemLilyPadDie)(int*);
gridItemLilyPadDie oGridItemLilyPadDie = NULL;

void hkGridItemLilyPadDie(int* lilyPad) {
	// I exclusively want to filter for GridItemLilyPad to keep myself updated
	LOGI("Board entity dies! Tiles = %d %d", lilyPad[0x38], lilyPad[0x39]);
	int x = lilyPad[0x38];
	int y = lilyPad[0x39];
	// lilyPads.erase(std::make_pair(x, y));

	oGridItemLilyPadDie(lilyPad);
}

typedef int (*gridItemLilyPadRegisterCallbacks)(int*);
gridItemLilyPadRegisterCallbacks oGridItemLilyPadRegisterCallbacks = NULL;

int hkGridItemLilyPadRegisterCallbacks(int* lilyPad) {
	int x = lilyPad[0x38];
	int y = lilyPad[0x39];
	// lilyPads.insert(std::make_pair(x, y));

	return oGridItemLilyPadRegisterCallbacks(lilyPad);
}

typedef void (*fun35)(int, int, int);
fun35 oFun35 = NULL;

void hkFun35(int a1, int a2, int a3) {
	SexyString* v7 = (SexyString*)(a2 + 4);
	LOGI("calling fun35 for %s", v7->c_str());
	std::vector<int>* reasons = (std::vector<int>*)a3;
	LOGI("reasons.size = %d", reasons->size());
	oFun35(a1, a2, a3);
	LOGI("reasons.size = %d", reasons->size());
}

typedef int (*fun36)(int, int*, int, int);
fun36 oFun36 = NULL;

int hkFun36(int a1, int* a2, int a3, int a4) {
	// return oFun36(a1, a2, a3, a4);
	
	typedef void (*V7)(int, int, int*, int);
	V7 v7 = *(V7*)(*(uint*)a3 + 0x30);
	uint board = (uint) getBoard();
	v7(a3, board, a2, a4);

	int v13[3]; // this is probably a vector type

	typedef void (*fun72AF48)(uint, int, int*);
	((fun72AF48)getActualOffset(0x72AF48))((uint)v13, board, a2);
}

//void gridItemPuddlesCanPlantGoHere(int* puddle, int plant, std::vector<int>* reasonsVec) {
//	SexyString plantName = *(SexyString*)(plant + 4);
//
//	RtWeakPtr<int> v7 = RtWeakPtr<int>();
//	v7.FromOther((RtWeakPtr<int>*)(plant + 0x38));
//	int plantProps = (int) v7.get();
//	uint8_t plantLivesOnWaves = *(uint8_t*)(plantProps + 0x11B);
//
//	if (plantLivesOnWaves) {
//		return;
//	}
//
//	int gridX = puddle[0x38], gridY = puddle[0x39];
//	typedef int (*lilyPadExistsAtPosition)(int, int);
//	lilyPadExistsAtPosition check = (lilyPadExistsAtPosition)getActualOffset(0x4AD150);
//	if (check(gridX, gridY)) {
//		return;
//	}
//
//	// cannot plant here
//	reasonsVec->push_back(0x20);
//}
//
//void gridItemPuddleDtor(int* puddle) {
//	int mX = puddle[0x38], mY = puddle[0x39];
//	// LOGI("Puddle dtor %d %d", mX, mY);
//	puddles.erase({ mX, mY });
//
//	typedef int (*fun411E14)(int*);
//	int v1 = ((fun411E14)getActualOffset(0x411E14))(puddle);
//	operator delete((void*)v1);
//}
//
//int gridItemPuddleSetSpawnLocation(int puddle, SexyVector3* loc) {
//	float mX = loc->mX;
//	float mY = loc->mY;
//	int gridX = (int)((mX - GRID_LEFT) / GRID_WIDTH);
//	int gridY = (int)((mY - GRID_TOP) / GRID_HEIGHT);
//	// LOGI("Puddles spawn %f %f %d %d", mX, mY, gridX, gridY);
//	puddles.insert({ gridX, gridY });
//
//	typedef int (*oSetSpawnLocation)(int, SexyVector3*);
//	return ((oSetSpawnLocation)getActualOffset(0x2D874C))(puddle, loc);
//}
//
//int gridItemPuddleLoadObj(int* puddle, int a2) {
//	typedef int (*propSheetBaseFun6)(int*, int);
//	((propSheetBaseFun6)getActualOffset(0x54B7A0))(puddle, a2);
//
//	int mX = puddle[0x38], mY = puddle[0x39];
//	// LOGI("Loaded crater %d %d", mX, mY);
//	puddles.insert({ mX, mY });
//
//	return 1;
//}

//void* puddlesVftable = NULL;
//
//typedef void* (*gridItemCraterConstruct)();
//gridItemCraterConstruct oGridItemCraterConstruct = NULL;
//
//void* hkGridItemCraterConstruct() {
//	void* res = oGridItemCraterConstruct();
//
//	if (!puddlesVftable) {
//		puddlesVftable = copyVFTable(getActualOffset(0x1BC2030), 67);
//
//		patchVFTable(puddlesVftable, (void*)gridItemPuddlesCanPlantGoHere, 55);
//		patchVFTable(puddlesVftable, (void*)gridItemPuddleDtor, 3);
//		patchVFTable(puddlesVftable, (void*)gridItemPuddleSetSpawnLocation, 13);
//		patchVFTable(puddlesVftable, (void*)gridItemPuddleLoadObj, 6);
//		// better route is to override setSpawnPosition
//	}
//
//	*(void**)res = puddlesVftable;
//
//	return res;
//}

typedef void (*boardRegionDrySandCanPlantGoHere)(int, int*, int, std::vector<int>*);
boardRegionDrySandCanPlantGoHere oBoardRegionDrySandCanPlantGoHere = NULL;

void hkBoardRegionDrySandCanPlantGoHere(int a1, int* gridCoords, int plant, std::vector<int>* reasonsVec) {
	if (plant) {
		SexyString plantName = *(SexyString*)(plant + 4);
		// LOGI("Can plant here %d %d %s", gridCoords[0], gridCoords[1], plantName.c_str());
		if (plantName == "lilypad") {
			int gridX = gridCoords[0], gridY = gridCoords[1];
			if (hasPuddleAt(gridX, gridY)) {
				return;
			}
			reasonsVec->push_back(7);
		}
	}
}

typedef void (*BoardPlantingCanPlantAt)(int, int*, int, std::vector<int>*);
BoardPlantingCanPlantAt oBoardPlantingCanPlantAt = NULL;

void hkBoardPlantingCanPlantAt(int a1, int* gridCoords, int plant, std::vector<int>* reasonsVec) {
	oBoardPlantingCanPlantAt(a1, gridCoords, plant, reasonsVec);

	if (reasonsVec->empty()) {
		if (!plant) {
			return;
		}
		SexyString plantName = *(SexyString*)(plant + 4);
		if (plantName != "lilypad") {
			return;
		}

		Board* board = getBoard();
		// first, check if this board has a tide region. Use fake coordinates for this
		SexyVector3 alwaysTide = SexyVector3(GRID_LEFT + 10 * GRID_WIDTH, GRID_TOP + GRID_HEIGHT, 0.0);
		typedef bool (*waterExistsAtPosition)(Board*, SexyVector3*);
		waterExistsAtPosition checkFun = (waterExistsAtPosition)getActualOffset(0x72AA84);
		if (!checkFun(board, &alwaysTide)) { // there is no tide region in this board
			if (!hasPuddleAt(gridCoords[0], gridCoords[1])) {
				reasonsVec->push_back(7);
			}
		}
	}
}

typedef void (*plantTangleKelpApplyPlantFood)(int*);
plantTangleKelpApplyPlantFood oPlantTangleKelpApplyPlantFood = NULL;

bool fromTangleKelpPF = false;

void hkPlantTangleKelpApplyPlantFood(int* delegate) {
	fromTangleKelpPF = true;
	oPlantTangleKelpApplyPlantFood(delegate);
	fromTangleKelpPF = false;
	return;

	//Rect v46;
	//Board* board = getBoard();
	//int plant = delegate[1];
	//SexyVector3* coords = (SexyVector3*)(plant + 0x14);
	//float mX = coords->mX, mY = coords->mY;

	//int tideRegion = hkFun18((int) board, coords, 2); // REQUIRES FUN18 TO BE HOOKED

	//Rect tideBoundaries = Rect(0, 0, 0, 0);

	//if (tideRegion && tideRegion != NO_TIDE_REGION) {
	//	int v8 = *(int*)((int)board + 0x340);
	//	int v9 = *(int*)((int)board + 0x348);

	//	typedef void (*getBoardRegionBoundaries)(Rect*, int);
	//	((getBoardRegionBoundaries)getActualOffset(0x104FEF4))(&tideBoundaries, tideRegion);
	//}

	//std::vector<int> zombieTargets;
	//typedef void (*fun50F9D8)(std::vector<int>*, int, Rect*, int, int);
	//((fun50F9D8)getActualOffset(0x50F9D8))(&zombieTargets, 2, &tideBoundaries, -1, -1);

	//typedef int (*getPlantPropsFromDelegate)(int*);
	//int tangleKelpProps = ((getPlantPropsFromDelegate)getActualOffset(0xA4E4F8))(delegate);
	//
	//int minPlantfoodWaterKills = *(int*)(tangleKelpProps + 0x144);
	//int maxPlantfoodWaterKills = *(int*)(tangleKelpProps + 0x148);

	//std::uniform_int_distribution<> dist(minPlantfoodWaterKills, maxPlantfoodWaterKills);
	/*int amountToKill = dist(mt_rand);
	if (amountToKill >= 1) {
		
	}*/
}

typedef void (*fun40)(Rect*, int);
fun40 oFun40 = NULL;

void hkFun40(Rect* a1, int tideRegion) {
	if (tideRegion == NO_TIDE_REGION) {
		a1->mX = 0;
		a1->mY = 0;
		a1->mWidth = 0;
		a1->mHeight = 0;
	}
	else {
		oFun40(a1, tideRegion);
	}
}

typedef void (*fun41)(int, SexyVector3*);
fun41 oFun41 = NULL;

void hkFun41(int a1, SexyVector3* a2) {
	if (a2->mY == 0) {
		a2->mY -= 40;
	}
	oFun41(a1, a2);
}

typedef void (*fun42)(std::vector<int>*, uint8_t, Rect*, int, int);
fun42 oFun42 = NULL;

void hkFun42(std::vector<int>* targets, uint8_t targetType, Rect* area, int i_row, int a5) {
	/*if (fromTangleKelpPF) {
		LOGI("Before targets size = %d", targets->size());
	}*/
	oFun42(targets, targetType, area, i_row, a5);
	if (fromTangleKelpPF) {
		// LOGI("Tide region targets size = %d", targets->size());
		// find targets in puddles and add them in as well
		std::set<std::pair<int, int>>& puddlesSet = *getPuddles();
		for (std::pair<int, int> i: puddlesSet) {
			int gridX = i.first, gridY = i.second;
			float mX = gridX * GRID_WIDTH + GRID_LEFT;
			float mY = gridY * GRID_HEIGHT + GRID_TOP;
			
			Rect puddleArea = Rect(mX, mY, GRID_WIDTH - 1, GRID_HEIGHT - 1);

			oFun42(targets, targetType, &puddleArea, i_row, a5);
		}

		// LOGI("after targets size = %d", targets->size());
		fromTangleKelpPF = false;
	}
}

int getTargetGridItemForShovel(int gridX, int gridY) {
	std::vector<int> targets;
	typedef void (*getGridItemsAtTile)(std::vector<int>*, uint8_t flagNum, int, int);
	((getGridItemsAtTile)getActualOffset(0x50F97C))(&targets, 4, gridX, gridY);
	for (int target : targets) {
		if (target) {
			typedef int (*isType)(int, int);
			isType checkType = *(isType*)(*(int*)target + 0x10);
			typedef int (*gridItemGetClass)();
			int v6 = ((gridItemGetClass)getActualOffset(0x4A59E0))();
			if (checkType(target, v6)) {
				// add a shovelable check here
				typedef int (*canShovel)(int);
				canShovel checkCanShovel = *(canShovel*)(*(int*)target + 0xA4);
				if (checkCanShovel(target)) {
					return target;
				}
			}
		}
	}
	return 0;
}

typedef int (*shovelGridItem)(int);
shovelGridItem oShovelGridItem = NULL;
int hkShovelGridItem(int shovelCursor) { // returns a boolean actually
	int mX = *(int*)(shovelCursor + 4);
	int mY = *(int*)(shovelCursor + 8);
	
	typedef int (*convertCoordsToGrid)(int);
	int gridX = ((convertCoordsToGrid)getActualOffset(0x6404B0))(mX);
	int gridY = ((convertCoordsToGrid)getActualOffset(0x6404CC))(mY);

	int target = getTargetGridItemForShovel(gridX, gridY);

	if (target) {
		typedef int (*canShovel)(int);
		canShovel checkCanShovel = *(canShovel*)(*(int*)target + 0xA4);
		int v7 = checkCanShovel(target);
		if (v7) {
			typedef void (*shovelTarget)(int, int, int, int);
			((shovelTarget)getActualOffset(0x104B6CC))(v7, target, gridX, gridY);
			return true;
		}
	}

	return false;
	//return oShovelGridItem(shovelCursor);
}

typedef void (*fun50)(int);
fun50 oFun50 = NULL;

void hkFun50(int region) {
	oFun50(region);
	LOGI("Fun50");
	LOG_RETURN_ADDRESS(0);
}

typedef void (*fun51)(int, SexyVector3*);
fun51 oFun51 = NULL;

void hkFun51(int region, SexyVector3* a2) {
	oFun51(region, a2);
	LOGI("Fun51");
	LOG_RETURN_ADDRESS(0);
	/*LOG_RETURN_ADDRESS(1);
	LOG_RETURN_ADDRESS(2);
	LOG_RETURN_ADDRESS(3);*/
}

void initTideHooks() {
	// PVZ2HookFunction(0x72AA84, (void*)hkBoardWaterExistsAtPos, (void**)&oBoardWaterExistsAtPos, "Board::WaterExistsAtPosition");
	// PVZ2HookFunction(0x72A148, (void*)hkFun19, (void**)&oFun19, "fun19");
	PVZ2HookFunction(0x72A6D0, (void*)hkFun18, (void**)&oFun18, "GetRegionWithFlag");
	PVZ2HookFunction(0x729F7C, (void*)hkFun17, (void**)&oFun17, "fun17");
	PVZ2HookFunction(0xCE0054, (void*)hkFun32, (void**)&oFun32, "fun32");

	// PVZ2HookFunction(0x728D50, (void*)hkFun33, (void**)&oFun33, "fun33");
	// PVZ2HookFunction(0x4A6A3C, (void*)hkFun34, (void**)&oFun34, "fun34");

	// PVZ2HookFunction(0xA4C144, (void*)hkGridItemLilyPadDie, (void**)&oGridItemLilyPadDie, "GridItemLilyPad::Die");
	// PVZ2HookFunction(0xA4C974, (void*)hkGridItemLilyPadRegisterCallbacks, (void**)&oGridItemLilyPadRegisterCallbacks, "GridItemLilyPad::RegisterCallbacks");
	// PVZ2HookFunction(0xA4CD20, (void*)hkFun35, (void**)&oFun35, "fun35");
	// PVZ2HookFunction(0x6484F8, (void*)hkFun36, (void**)&oFun36, "fun36");

	// so now for my own puddles, I add my own callback to the messageRouter on GridItem_Function_7
	//	when planted, it should call my callback to check. I will then check if there is a lily pad at my grid item. If yes, allow planting. Else, disallow it
	// 
	//	the dry sand code will have to be modified as well to allow lily pads on land. Just check if the spot has 1 of my grid items. I'll just use an internal set to maintain all of these, hook into the GridItem_Function_7 and dtor to sync the list

	// Zombies sinking should work just fine afaik

	// and then I'll still have to fucking make a new class

	/*
	Plan of action:
	- Override GridItemCrater's ctor code to return to my copied vftable (at _DWORD *sub_364098())
	- Write a canPlantAt function that checks for lily pad's existence on the grid tile (0x38 and 0x39). If none, adds error code 20 to the vector of reasons. Refer to TideRegion's code
	- Patch that into address 0xDC of the vftable. Now GridItem_Function_7 should automatically refer to this
	- Also override GridItem_Function_7 to keep track of the list of puddles internally within a set. Override 411E4 to delete it from our set on craters' destruction.
	- NOT OVERRIDE!!!!. Patch it into our new vftable and make them custom functions.
	- Finally, patch BoardRegionDrySand to allow lilypads on sand if there is a puddle there. Refer to our internal set.
	- And also, change our existing code to refer to this internal set as well.
	*/
	// make a new GridItemPuddles class that inherits from GridItemCrater
	// PVZ2HookFunction(0x364098, (void*)hkGridItemCraterConstruct, (void**)&oGridItemCraterConstruct, "GridItemCrater::construct");
	
	// disallow planting lily pads on non-water tiles
	PVZ2HookFunction(0x4ADBA8, (void*)hkBoardRegionDrySandCanPlantGoHere, (void**)&oBoardRegionDrySandCanPlantGoHere, "BoardRegionDrySand::CanPlantGoHere");
	PVZ2HookFunction(0x6484F8, (void*)hkBoardPlantingCanPlantAt, (void**)&oBoardPlantingCanPlantAt, "BoardPlanting::CanPlantGoHere");

	// plans for kelp's PF
	// just rewrite the whole fucking thing lmfao
	// or we can do 3 clean fixes
	PVZ2HookFunction(0xA4F894, (void*)hkPlantTangleKelpApplyPlantFood, (void**)&oPlantTangleKelpApplyPlantFood, "PlantTangleKelp::ApplyPlantFood");
	PVZ2HookFunction(0x104FEF4, (void*)hkFun40, (void**)&oFun40, "GetBoardRegionBoundaries");
	PVZ2HookFunction(0xA4FDDC, (void*)hkFun41, (void**)&oFun41, "TangleKelpSpawnTentacleAtPosition");
	PVZ2HookFunction(0x50F9D8, (void*)hkFun42, (void**)&oFun42, "GetTargetsInRegion");

	// fix shovel bug: if puddle spawn on lily pad, lily pad becomes unshovelable
	// fix: in shovel's targeting function, make sure the GI is shovelable first before returning it
	PVZ2HookFunction(0x104B658, (void*)hkShovelGridItem, (void**)&oShovelGridItem, "ShovelGridItem");

	// more testing
	//PVZ2HookFunction(0x4AD240, (void*)hkFun50, (void**)&oFun50, "fun50");
	// PVZ2HookFunction(0x4AD754, (void*)hkFun51, (void**)&oFun51, "fun51");


}
