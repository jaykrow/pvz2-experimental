#include "pch.h"
#include "GridItemRailAltProps.h"

#define RTOBJECT_GETTYPE_INDEX 0
#define RTOBJECT_FUNC1_INDEX 1
#define RTOBJECT_CONSTRUCT_INDEX 2
#define RTOBJECT_DESTRUCT_INDEX 3
#define RTOBJECT_ISTYPE_INDEX 4
#define RTOBJECT_FUNC5_INDEX 5
#define RTOBJECT_FUNC6_INDEX 6

void GridItemCraterProps::emptyFun() { }

void* GridItemRailAltProps::vftable = NULL;
void* GridItemRailAltProps::s_rtClass = NULL;

void GridItemRailAltProps::modInit() {
	LOGI("GridItemRailAltProps mod init");

	vftable = copyVFTable(getActualOffset(0x1BC1FE4), 14);
	patchVFTable(vftable, (void*)GridItemRailAltProps::getRTClass, RTOBJECT_GETTYPE_INDEX);

	GridItemRailAltProps::getRTClass();

	LOGI("GridItemRailAltProps finish init");
}
