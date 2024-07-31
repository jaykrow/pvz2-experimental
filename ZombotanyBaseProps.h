#pragma once
#include "ZombieBullProps.h"
#include "Logging.h"
#include "memUtils.h"

typedef void* (*ParameterlessConstructorFunc)();
typedef int (*zombiePropertySheetCtor)(int);

static void constructZombiePropertySheet(int v0) {
	((zombiePropertySheetCtor)getActualOffset(0x88542C))(v0);
}

static void constructZombieBullProperties(int* v0) {
	v0[0x66] = 0;
	v0[0x67] = 0;
	v0[0x68] = 0;
	v0[0x69] = 0;
	v0[0x6A] = 0x3F000000; // 0.5
	v0[0x6B] = 0x44480000; // 800.0
	v0[0x6C] = 0x3E99999A; // 0.3
}

class ZombotanyBaseProps : public ZombieBullProps
{
public:
	SexyString zombieType;
	static void* vftable;
	static void* s_rtClass;
	static void modInit();

	static void* construct() { // this corresponds to A10A40 for ZombieBullVet
		LOGI("Constructing ZombotanyBaseProps");

		ZombotanyBaseProps* v0 = new ZombotanyBaseProps();

		LOGI("Allocated space for props");

		constructZombiePropertySheet((int)v0);
		constructZombieBullProperties((int*)v0);
		LOGI("Constructed parent props");
		v0->zombieType = "cowboy_armor1";

		const char* zombieTypeName = v0->zombieType.c_str();
		LOGI("Set own zombie type %d %s", strlen(zombieTypeName), zombieTypeName);

		*(uint*)v0 = (uint) vftable;

		LOGI("Updated vftable %p", vftable);

		LOGI("Constructed ZombotanyBaseProps addr %p %p", v0, &v0->zombieType);
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
			
			typedef int (*ZombieBullPropsGetClass)();
			int parentClass = ((ZombieBullPropsGetClass)getActualOffset(0xA10460))();
			v2(v1, "ZombotanyBaseProps", parentClass, ZombotanyBaseProps::construct);

			LOGI("Registering class");
			ZombotanyBaseProps::registerClass();
		}
		// LOGI("Got RTClass");
		return s_rtClass;
	}

	static void buildSymbols(int* a1, int a2) { // A10BC0 equivalent
		LOGI("Building symbols for ZombotanyBaseProps");

		void* rtClass = ZombotanyBaseProps::getRTClass();
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
		SexyString v15 = "ZombieBullProps"; // parent class
		int v6 = v5(a1, &v15);
		int v7 = (*(fun6*)(*(uint*)v6 + 0x34))(v6);
		(*(fun7*)(*a1 + 0x30))(a1, a2, v7, 0);
		
		v15 = "ZombieType";

		typedef int (*fun14)(int*, int, int, int);
		fun14 v14 = *(fun14*)(*a1 + 0x28);
		int propType = funGetPropType(a1, 4, sizeof(char)); // 4 is Type_Char
		
		int v16 = v14(a1, 0, propType, 0); // special function to get string propType

		funRegisterProps(a1, a2, &v15, 440, v16);
		
		LOGI("Symbols built");
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

				SexyString v4 = "ZombotanyBaseProps";
				v3(builder, &v4, ZombotanyBaseProps::buildSymbols, sizeof(ZombotanyBaseProps), 0);
			} 
		}

		LOGI("Class registered");
	}
};

ZombotanyBaseProps* getZombotanyBaseProps(int bullZombie); // returns NULL if the props are not the correct type
