#pragma once
#include "GridItemCrater.h"

typedef void* (*ParameterlessConstructorFunc)();
typedef void* (*gridItemDefaultCtor)(int);

class GridItemRailAlt : public GridItemCrater
{
public:
	int storedImg;

	static void* vftable; // 01BCCC94
	static void* s_rtClass;
	static void modInit();

	~GridItemRailAlt() {
		typedef int (*ogDtor2)(GridItem*);
		((ogDtor2)getActualOffset(0x411E14))(this);
	}

	static void* construct() { // this corresponds to A10A40 for ZombieBullVet
		LOGI("Constructing GridItemRailAlt");

		GridItemRailAlt* v0 = new GridItemRailAlt();

		((gridItemDefaultCtor)getActualOffset(0x411DBC))((int)v0);

		v0->storedImg = NULL;

		*(uint*)v0 = (uint)vftable;
		((int*)v0)[4] = getActualOffset(0x1BC2144);

		LOGI("Constructed GridItemRailAlt");
		return (void*)v0;
	}

	static void* getRTClass() { // A109C8
		if (s_rtClass == NULL) {
			LOGI("No global RTClass exists");
			void* v1;
			typedef void* (*initRtClass)();
			v1 = ((initRtClass)getActualOffset(0x123CF6C))();
			s_rtClass = v1;
			LOGI("Obtained RTClass");

			typedef int (*rtClassRegisterClass)(void*, const char*, int, ParameterlessConstructorFunc);
			rtClassRegisterClass v2 = *(rtClassRegisterClass*)(*(uint*)v1 + 0x20);
			// v2 refers to sub_123D950

			typedef int (*GridItemGetClass)();
			int parentClass = ((GridItemGetClass)getActualOffset(0x364020))();
			v2(v1, "GridItemRailAlt", parentClass, GridItemRailAlt::construct);

			LOGI("Registering class");
			GridItemRailAlt::registerClass();
		}
		// LOGI("Got RTClass");
		return s_rtClass;
	}

	static void buildSymbols(int* a1, int a2) { // A10BC0 equivalent
		LOGI("Building symbols for GridItemRailAlt");

		void* rtClass = GridItemRailAlt::getRTClass();
		*(int*)((uint)rtClass + 0x24) = a2; // set RClass
		*(int*)((uint)a2 + 0x58) = (int)rtClass;

		typedef int (*fun5)(int*, SexyString*);
		typedef int (*fun6)(int);
		typedef int (*fun7)(int*, int, int, int);
		/*typedef void (*registerProperty)(int*, int, SexyString*, int, int);
		typedef int (*getPropertyOfType)(int*, int, int);

		registerProperty funRegisterProps = *(registerProperty*)(*a1 + 0x34);
		getPropertyOfType funGetPropType = *(getPropertyOfType*)(*a1 + 0x1C);*/

		fun5 v5 = *(fun5*)(*a1 + 0x2C);
		SexyString v15 = "GridItemCrater"; // parent class
		int v6 = v5(a1, &v15);
		int v7 = (*(fun6*)(*(uint*)v6 + 0x34))(v6);
		(*(fun7*)(*a1 + 0x30))(a1, a2, v7, 0);

		//v15 = "ImgPrefix";

		//typedef int (*fun14)(int*, int, int, int);
		//fun14 v14 = *(fun14*)(*a1 + 0x28);
		//int propType = funGetPropType(a1, 4, sizeof(char)); // 4 is Type_Char

		//int v16 = v14(a1, 0, propType, 0); // convert it to a char pointer type

		//funRegisterProps(a1, a2, &v15, 240, v16);

		LOGI("GridItemRailAlt symbols built");
	}

	static void registerClass() { // A10ABC
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

				SexyString v4 = "GridItemRailAlt";
				v3(builder, &v4, GridItemRailAlt::buildSymbols, sizeof(GridItemRailAlt), 0);
			}
		}

		LOGI("GridItemRailAlt registered");
	}

	static void registerDelegates(GridItemRailAlt* a1);
	static void canPlantGoHere(GridItemRailAlt* gridItem, int plant, std::vector<int>* reasonsVec);
};

