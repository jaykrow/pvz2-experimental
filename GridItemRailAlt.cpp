#include "pch.h"
#include "GridItemRailAlt.h"
#include "PvZ2/Board.h"
#include <vector>
#include "GridItemRailAltProps.h"
#include "SexyApp.h"

#define RTOBJECT_GETTYPE_INDEX 0
#define RTOBJECT_FUNC1_INDEX 1
#define RTOBJECT_CONSTRUCT_INDEX 2
#define RTOBJECT_DESTRUCT_INDEX 3
#define RTOBJECT_ISTYPE_INDEX 4
#define RTOBJECT_FUNC5_INDEX 5
#define RTOBJECT_FUNC6_INDEX 6

void* GridItemRailAlt::vftable = NULL;
void* GridItemRailAlt::s_rtClass = NULL;

void drawRailAlt(GridItemRailAlt* gridItem, int graphicsState);

void GridItemRailAlt::modInit() {
	LOGI("GridItemRailAlt mod init");
	
	vftable = copyVFTable(getActualOffset(0x1BCCC94), 67);
	patchVFTable(vftable, (void*)GridItemRailAlt::getRTClass, RTOBJECT_GETTYPE_INDEX);
	patchVFTable(vftable, (void*)GridItemRailAlt::canPlantGoHere, 55);
	patchVFTable(vftable, (void*)drawRailAlt, 27);

	GridItemRailAlt::getRTClass();

	LOGI("GridItemRailAlt finish init");
}

void GridItemRailAlt::registerDelegates(GridItemRailAlt* a1) {
	typedef void (*oRegisterDelegates)(GridItemRailAlt* a1);
	((oRegisterDelegates)getActualOffset(0x4A6964))(a1);
}

void GridItemRailAlt::canPlantGoHere(GridItemRailAlt* gridItem, int plant, std::vector<int>* reasonsVec) {
	// 0x18 if trying to plant directly on rail
	typedef int (*railcartExistsAt)(int, int);
	if (((railcartExistsAt)getActualOffset(0x3AF8D0))(gridItem->gridX, gridItem->gridY)) {
		return;
	}

	reasonsVec->push_back(0x18);

	return;
}

SexyString convertDirTypeToSuffix(int dirType) {
	switch (dirType) {
		case 0:
			return "_MID";
		case 1: // can only go up
			return "_BOTTOM";
		case 2: // can only go right
			return "_LEFT"; 
		case 3: // can go both up and right
			return "_TURN2";
		case 4: // can only go down
			return "_TOP";
		case 5: // can go down and up
			return "_MID";
		case 6: // can go down and right
			return "_TURN1";
		case 7: // can go down right and up
			return "_CROSS";
		case 8: // can only go left
			return "_RIGHT";
		case 9: // can go left and up
			return "_TURN4";
		case 10: // can go left and right
			return "_MID_HORIZONTAL";
		case 11: // can go left, right and up
			return "_CROSS";
		case 12: // can go left and down
			return "_TURN3";
		case 13: // can go left, down and up:
			return "_CROSS";
		case 14: // can go left, down and right:
			return "_CROSS";
		case 15: // can go in all 4 directions
			return "_CROSS";
		default:
			return "_INVALID";
	}
}

int getImageFromSuffix(SexyString imgPrefix, SexyString suffix) {
	SexyString fullName = imgPrefix + suffix;
	int textureLoader = *(int*)(getLawnApp() + 0x58C);
	typedef int (*getResInfo)();
	int resInfo = ((getResInfo)getActualOffset(0x1243018))();
	typedef int (*getTexture)(int, int, SexyString*);

	//LOGI("got everything");
	int texture = ((getTexture)getActualOffset(0x1246A14))(textureLoader, resInfo, &fullName);
	//LOGI("got texture %d", texture);

	RtWeakPtr<int> textureWeakPtr = RtWeakPtr<int>();
	textureWeakPtr.FromOther((RtWeakPtr<int>*)(texture + 0x18));

	//LOGI("Got the texture weak ptr");

	int finalImg = (int) textureWeakPtr.get();
	//LOGI("got final img");
	return finalImg;
}

void drawRailAlt(GridItemRailAlt* gridItem, int graphicsState) {
	// LOGI("graphicsState = %p", graphicsState);
	if (gridItem->storedImg == NULL) {
		RtWeakPtr<GridItemPropertySheet> propsCopy;
		propsCopy.FromOther(&gridItem->props);

		int props = (int)propsCopy.get();

		SexyString imgPrefix = "IMAGE_RAILCART_PIRATE";
		int dirType = 0; // placeholder direction
		
		typedef int (*isTypeFun)(int, int);
		isTypeFun checkType = *(isTypeFun*)(*(int*)props + 0x10);
		int railPropsType = (int) GridItemRailAltProps::getRTClass();
		if (checkType(props, railPropsType)) {
			GridItemRailAltProps* realProps = (GridItemRailAltProps*)props;
			
			imgPrefix = realProps->imgPrefix;
			dirType = realProps->dirType;
		}

		gridItem->storedImg = getImageFromSuffix(imgPrefix, convertDirTypeToSuffix(dirType));
	}
	int theImg = gridItem->storedImg;

	//LOGI("got the image = %d", theImg);
	//LOGI("img height = %d img width = %d", *(int*)(theImg + 0x30), *(int*)(theImg + 0x34));

	int gridX = gridItem->gridX;
	int gridY = gridItem->gridY;
	
	float mX = GRID_TILE_LEFT + gridX * GRID_TILE_WIDTH;
	float mY = GRID_TILE_TOP + gridY * GRID_TILE_HEIGHT;

	//LOGI("draw at %d %d %d %d", gridX, gridY, (int)mX, (int)mY);

	typedef float (*sub3776F8)(int, float);
	auto sexyApp = SexyApp::Get();
	float renderX = sexyApp->Scale(mX);
	float renderY = sexyApp->Scale(mY);

	//LOGI("renderX = %d renderY = %d", (int)renderX, (int)renderY);

	typedef void (*drawImg)(int, int, int, int);
	((drawImg)getActualOffset(0x11255D4))(graphicsState, theImg, (int)renderX, (int)renderY);
	//LOGI("drew img");
}