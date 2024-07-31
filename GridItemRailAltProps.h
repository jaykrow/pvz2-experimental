#pragma once
#include "memUtils.h"

class GridItemCraterProps 
{
	char unk[0x8C];
	
	virtual void emptyFun();
};

typedef void* (*ParameterlessConstructorFunc)();
typedef void* (*gridItemDefaultCtor)(int);

class GridItemRailAltProps : public GridItemCraterProps
{
public:
	SexyString imgPrefix;
	int dirType;

	static void* vftable; // 01BCCC94
	static void* s_rtClass;
	static void modInit();

	~GridItemRailAltProps() {
		typedef int (*ogDtor2)(GridItemCraterProps*);
		((ogDtor2)getActualOffset(0x365338))(this);
	}

	static void* construct() { // this corresponds to A10A40 for ZombieBullVet
		LOGI("Constructing GridItemRailAltProps");

		GridItemRailAltProps* v0 = new GridItemRailAltProps();

		((gridItemDefaultCtor)getActualOffset(0x3654BC))((int)v0);

		// v0->imgPrefix = "IMAGE_RAILCART_PIRATE";

		*(uint*)v0 = (uint)vftable;

		LOGI("Constructed GridItemRailAltProps");
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

			typedef int (*GridItemPropsGetClass)();
			int parentClass = ((GridItemPropsGetClass)getActualOffset(0x3647AC))();
			v2(v1, "GridItemRailAltProps", parentClass, GridItemRailAltProps::construct);

			LOGI("Registering class");
			GridItemRailAltProps::registerClass();
		}
		// LOGI("Got RTClass");
		return s_rtClass;
	}

	static void buildSymbols(int* a1, int a2) { // A10BC0 equivalent
		LOGI("Building symbols for GridItemRailAltProps");

		void* rtClass = GridItemRailAltProps::getRTClass();
		*(int*)((uint)rtClass + 0x24) = a2; // set RClass
		*(int*)((uint)a2 + 0x58) = (int)rtClass;

		typedef int (*fun5)(int*, SexyString*);
		typedef int (*fun6)(int);
		typedef int (*fun7)(int*, int, int, int);
		typedef void (*registerProperty)(int*, int, SexyString*, int, int);
		typedef int (*getPropertyOfType)(int*, int, int);

		registerProperty funRegisterProps = *(registerProperty*)(*a1 + 0x34);
		getPropertyOfType funGetPropType = *(getPropertyOfType*)(*a1 + 0x1C);

		fun5 v5 = *(fun5*)(*a1 + 0x2C);
		SexyString v15 = "GridItemCraterProps"; // parent class
		int v6 = v5(a1, &v15);
		int v7 = (*(fun6*)(*(uint*)v6 + 0x34))(v6);
		(*(fun7*)(*a1 + 0x30))(a1, a2, v7, 0);

		v15 = "ImgPrefix";

		typedef int (*fun14)(int*, int, int, int);
		fun14 registerPointerProperty = *(fun14*)(*a1 + 0x28);
		int propType = funGetPropType(a1, 4, sizeof(char)); // 4 is Type_Char
		int stringPropType = registerPointerProperty(a1, 0, propType, 0); // convert it to a char pointer type
		funRegisterProps(a1, a2, &v15, 144, stringPropType);
		
		v15 = "DirType";
		propType = funGetPropType(a1, 6, sizeof(int)); // 6 is Type_Int
		funRegisterProps(a1, a2, &v15, 156, propType);

		LOGI("GridItemRailAltProps symbols built");
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

				SexyString v4 = "GridItemRailAltProps";
				v3(builder, &v4, GridItemRailAltProps::buildSymbols, sizeof(GridItemRailAltProps), 0);
			}
		}

		LOGI("GridItemRailAlt registered");
	}
};

