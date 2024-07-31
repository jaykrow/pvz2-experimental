#pragma once
#include "LevelModule.h"
#include "Logging.h"
#include "memUtils.h"

typedef void* (*ParameterlessConstructorFunc)();
typedef void* (*gridItemDefaultCtor)(int);

static void constructDefaultLevelModule(int v0) {
	((gridItemDefaultCtor)getActualOffset(0x54B4BC))(v0);
}

class SixLaneModule : public LevelModule
{
public:
	static void* vftable; // if typeinfo is changed later make sure to update the vftable as well
	static void* s_rtClass;
	static void modInit();

	static void* construct() { // refer to 364378 and 364098
		LOGI("Constructing SixLaneModule");

		SixLaneModule* v0 = new SixLaneModule();

		constructDefaultLevelModule((int)v0);
		*(uint*)v0 = (uint)vftable;
		
		v0->propSheetPtr = RtWeakPtr<LevelModuleProperties>();

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

			typedef int (*levelModuleGetClass)();
			int parentClass = ((levelModuleGetClass)getActualOffset(0xCE8100))();
			v2(v1, "SixLaneModule", parentClass, SixLaneModule::construct);

			SixLaneModule::registerClass();
		}
		return s_rtClass;
	}

	static void buildSymbols(int* a1, int a2) { // 3644E4
		LOGI("Building symbols for SixLaneModule");

		void* rtClass = SixLaneModule::getRTClass();
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
		SexyString v15 = "LevelModule"; // parent class
		int v6 = v5(a1, &v15);
		int v7 = (*(fun6*)(*(uint*)v6 + 0x34))(v6);
		(*(fun7*)(*a1 + 0x30))(a1, a2, v7, 0);

		LOGI("SixLaneModule symbols built");
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

				SexyString v4 = "SixLaneModule";
				v3(builder, &v4, SixLaneModule::buildSymbols, sizeof(SixLaneModule), 0);
			}
		}

		LOGI("SixLaneModule registered");
	}

	static void registerDelegates(SixLaneModule *self);
};

