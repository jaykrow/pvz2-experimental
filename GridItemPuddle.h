#pragma once

#include "Logging.h"
#include "memUtils.h"
#include <set>
#include "GridItemCrater.h"
#include "PvZ2/RtWeakPtr.h"
#include "FrogJumpSubsystem.h"

typedef void* (*ParameterlessConstructorFunc)();
typedef void* (*gridItemDefaultCtor)(int);

static void constructDefaultGridItem(int v0) {
	((gridItemDefaultCtor)getActualOffset(0x411DBC))(v0);
}

// so I'll have to create an unique typeInfo as well?
class GridItemPuddle : public GridItemCrater
{
public:
	static void* vftable; // if typeinfo is changed later make sure to update the vftable as well
	static void* s_rtClass;
	static void modInit();

	static void* construct() { // refer to 364378 and 364098
		LOGI("Constructing GridItemPuddle");

		GridItemPuddle* v0 = new GridItemPuddle();
		
		constructDefaultGridItem((int)v0);
		*(uint*)v0 = (uint) vftable;
		((int*)v0)[4] = getActualOffset(0x1BC2144); // just refer to crater's typeInfo. Idk if this can cause issues

		return (void*)v0;
	}

	static void* getRTClass() {
		if (!s_rtClass) {
			void* v1;
			typedef void* (*initRtClass)();
			v1 = ((initRtClass)getActualOffset(0x123CF6C))();
			s_rtClass = v1;
			
			typedef int (*rtClassRegisterClass)(void*, const char*, int, ParameterlessConstructorFunc);
			rtClassRegisterClass v2 = *(rtClassRegisterClass*)(*(uint*)v1 + 0x20);

			typedef int (*gridItemCraterGetClass)();
			int parentClass = ((gridItemCraterGetClass)getActualOffset(0x364020))();
			v2(v1, "GridItemPuddle", parentClass, GridItemPuddle::construct);

			GridItemPuddle::registerClass();
		}
		return s_rtClass;
	}

	static void buildSymbols(int* a1, int a2) { // 3644E4
		LOGI("Building symbols for GridItemPuddle rClass = %u", (uint) a2);

		void* rtClass = GridItemPuddle::getRTClass();
		*(int*)((uint)rtClass + 0x24) = a2; // set RClass
		*(int*)((uint)a2 + 0x58) = (int)rtClass;

		typedef int (*fun5)(int*, SexyString*);
		typedef int (*fun6)(int);
		typedef int (*fun7)(int*, int, int, int);
		//typedef void (*registerProperty)(int*, int, SexyString*, int, int);
		//typedef int (*getPropertyOfType)(int*, int, int);

		//registerProperty funRegisterProps = *(registerProperty*)(*a1 + 0x34);
		//getPropertyOfType funGetPropType = *(getPropertyOfType*)(*a1 + 0x1C);

		fun5 v5 = *(fun5*)(*a1 + 0x2C);
		SexyString v15 = "GridItemCrater"; // parent class
		int v6 = v5(a1, &v15);
		int v7 = (*(fun6*)(*(uint*)v6 + 0x34))(v6);
		(*(fun7*)(*a1 + 0x30))(a1, a2, v7, 0);

		LOGI("GridItemPuddle symbols built");
	}

	static void registerClass() {
		typedef void* (*func12280C8)();
		func12280C8 func_0x12280C8 = (func12280C8)getActualOffset(0x12280C8);
		void* v0 = (void*)func_0x12280C8();
		if (v0)
		{
			typedef void* (*func122811C)(void*); \
				func122811C func_0x122811C = (func122811C)getActualOffset(0x122811C);
			void* builder = func_0x122811C(v0);
			if (builder)
			{
				typedef void (*symBuilder)(int*, int);
				typedef void (*fun3)(void*, SexyString*, symBuilder, int, int);
				fun3 v3 = *(fun3*)(*(int*)builder + 0x14);

				SexyString v4 = "GridItemPuddle";
				v3(builder, &v4, GridItemPuddle::buildSymbols, sizeof(GridItemPuddle), 0);
			}
		}

		LOGI("GridItemPuddle registered");
	}
};

bool hasPuddleAt(int x, int y);

std::set<std::pair<int, int>>* getPuddles();

void insertPuddle(int x, int y, bool createRegion);

void erasePuddle(int x, int y);

RtWeakPtr<int>* getRegionForPuddle(int x, int y);