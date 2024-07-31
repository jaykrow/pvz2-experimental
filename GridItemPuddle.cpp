#include "pch.h"
#include "GridItemPuddle.h"
#include "PvZ2/RtWeakPtr.h"
#include <vector>
#include <set>
#include <map>
#include "PvZ2/Board.h"
#include <assert.h>

const int GRID_HEIGHT = 76;
const int GRID_WIDTH = 64;
const int GRID_TOP = 160;
const int GRID_LEFT = 200;

void* GridItemPuddle::s_rtClass = NULL;
void* GridItemPuddle::vftable = NULL;

typedef int (*createBoardRegionOfType)(int, int);
createBoardRegionOfType oCreateBoardRegionOfType = NULL;

typedef void (*setRegion)(int, Rect*);
setRegion oSetRegion = NULL;

typedef int (*removeRegion)(int, RtWeakPtr<int>*);
removeRegion oRemoveRegion = NULL;

typedef int (*boardRegionTideWaterGetType)();
boardRegionTideWaterGetType oBoardRegionTideWaterGetType = NULL;

int tideWaterType = 0;

std::set<std::pair<int, int>> puddles;

void gridItemPuddlesCanPlantGoHere(int* puddle, int plant, std::vector<int>* reasonsVec) {
	return; // mini water regions created by puddles should handle the logic for planting

	//SexyString plantName = *(SexyString*)(plant + 4);

	//RtWeakPtr<int> v7 = RtWeakPtr<int>();
	//v7.FromOther((RtWeakPtr<int>*)(plant + 0x38));
	//int plantProps = (int)v7.get();
	//uint8_t plantLivesOnWaves = *(uint8_t*)(plantProps + 0x11B);

	//if (plantLivesOnWaves) {
	//	return;
	//}

	//int gridX = puddle[0x38], gridY = puddle[0x39];
	//typedef int (*lilyPadExistsAtPosition)(int, int);
	//lilyPadExistsAtPosition check = (lilyPadExistsAtPosition)getActualOffset(0x4AD150);
	//if (check(gridX, gridY)) {
	//	return;
	//}

	//// cannot plant here
	//reasonsVec->push_back(0x20);
}

void gridItemPuddleDtor(int* puddle) {
	int mX = puddle[0x38], mY = puddle[0x39];
	// LOGI("Puddle dtor %d %d", mX, mY);
	erasePuddle(mX, mY);

	typedef int (*fun411E14)(int*);
	int v1 = ((fun411E14)getActualOffset(0x411E14))(puddle);
	operator delete((void*)v1);
}

int gridItemPuddleSetSpawnLocation(int puddle, SexyVector3* loc) {
	float mX = loc->mX;
	float mY = loc->mY;
	int gridX = (int)((mX - GRID_LEFT) / GRID_WIDTH);
	int gridY = (int)((mY - GRID_TOP) / GRID_HEIGHT);
	// LOGI("Puddles spawn %f %f %d %d", mX, mY, gridX, gridY);
	insertPuddle(gridX, gridY, true); // create new region for this puddle

	typedef int (*oSetSpawnLocation)(int, SexyVector3*);
	return ((oSetSpawnLocation)getActualOffset(0x2D874C))(puddle, loc);
}

int gridItemPuddleLoadObj(int* puddle, int a2) {
	typedef int (*propSheetBaseFun6)(int*, int);
	((propSheetBaseFun6)getActualOffset(0x54B7A0))(puddle, a2);

	int mX = puddle[0x38], mY = puddle[0x39];
	// LOGI("Loaded crater %d %d", mX, mY);
	insertPuddle(mX, mY, false); // region already is saved and loaded

	return 1;
}

void GridItemPuddle::modInit() {
	LOGI("GridItemPuddle mod init");

	oBoardRegionTideWaterGetType = (boardRegionTideWaterGetType)getActualOffset(0x4ACF3C);
	oRemoveRegion = (removeRegion)getActualOffset(0x72AB94);
	oCreateBoardRegionOfType = (createBoardRegionOfType)getActualOffset(0x72AAA0); // create a tide water region
	oSetRegion = (setRegion)getActualOffset(0x104FE10); // set the boundaries
	tideWaterType = oBoardRegionTideWaterGetType();

	vftable = copyVFTable(getActualOffset(0x1BC2030), 67);

	patchVFTable(vftable, (void*)GridItemPuddle::getRTClass, 0);
	patchVFTable(vftable, (void*)gridItemPuddlesCanPlantGoHere, 55); 
	patchVFTable(vftable, (void*)gridItemPuddleDtor, 3);
	patchVFTable(vftable, (void*)gridItemPuddleSetSpawnLocation, 13);
	patchVFTable(vftable, (void*)gridItemPuddleLoadObj, 6);

	GridItemPuddle::getRTClass();

	LOGI("GridItemPuddle finish init");
}

bool hasPuddleAt(int x, int y) {
	return puddles.count({ x, y }) != 0;
}

std::set<std::pair<int, int>>* getPuddles() {
	return &puddles;
}

// so technically all puddles are stored in boardRegions now at E0

void insertPuddle(int x, int y, bool createRegion) {
	if (!puddles.count({ x, y })) {
		puddles.insert({ x, y });

		if (createRegion) {
			int region = oCreateBoardRegionOfType((int)getBoard(), tideWaterType);
			Rect area = Rect(x, y, 1, 1);
			oSetRegion(region, &area);
		}
	}
}

void erasePuddle(int x, int y) {
	if (!puddles.count({ x, y })) {
		return;
	}

	puddles.erase({ x, y });

	RtWeakPtr<int>* regionPtr = getRegionForPuddle(x, y);
	if (regionPtr == NULL) {
		return;
	}
	oRemoveRegion((int)getBoard(), regionPtr);
}

RtWeakPtr<int>* getRegionForPuddle(int x, int y) {
	Board* board = getBoard();
	int gridCoords[2] = { x, y };
	typedef void (*getRegionsContainingCoords)(std::vector<int>*, Board*, int*);
	std::vector<int> regions;
	((getRegionsContainingCoords)getActualOffset(0x72AF48))(&regions, board, gridCoords);

	int correctRegion = 0;
	LOGI("Regions for %d %d", x, y);
	for (int region : regions) {
		// 14, 18, 1C and 20 has the rect of region
		float mWidth = *(float*)(region + 0x1C);
		float mHeight = *(float*)(region + 0x20);

		LOGI("mX = %f mY = %f mWidth = %f mHeight = %f", *(float*)(region + 0x14), *(float*)(region + 0x18), mWidth, mHeight);

		if (mWidth == GRID_WIDTH && mHeight == GRID_HEIGHT) {
			correctRegion = region;
			LOGI("region = %u", (uint)region);
			//break;
		}
	}
	
	if (!correctRegion) { // this happens when exiting the game I believe
		return NULL;
	}

	LOGI("Yes, we have a correct region for %d %d", x, y);

	return (RtWeakPtr<int>*)(correctRegion + 8);
}