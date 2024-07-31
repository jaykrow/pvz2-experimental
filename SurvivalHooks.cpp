#include "pch.h"
#include "SurvivalHooks.h"
#include "memUtils.h"
#include "PvZ2/RtWeakPtr.h"
#include "SurvivalModule.h"
#include "SurvivalModuleProps.h"
#include "WaveManager.h"
#include "TimeMgr.h"
#include "PvZ2/Board.h"
#include "MessageRouter.h"
#include "Color.h"
#include "CachedResource.h"
#include <string>
#include <codecvt>
#include <locale>
#include "EntityConditionTracker.h"
#include "StageModule.h"
#include "StageModuleProperties.h"
#include "PVZDB.h"
#include "Plant.h"
#include "SeedBankModule.h"
#include "DummyModule.h"
#include "DummyModuleProps.h"
#include "SurvivalModule.h"
#include "WaveManagerEndlessModule.h"
#include "math.h"
#include "Messages.h"
#include "WaveManagerEndlessModuleProps.h"
#include "LevelEscalationModule.h"
#include "LevelEscalationModuleProperties.h"

typedef void (*fun208)(int, int);
fun208 oFun208 = NULL;

void hkFun208(int a1, int a2) {
	LOGI("change state to %d", a2);
	if (a2 == 4) {
		RtWeakPtr<int> mgrPtr;
		mgrPtr.FromOther((RtWeakPtr<int>*)(a1 + 0x20));
		int animationMgr = (int)mgrPtr.get();
		LOGI("vftable = %u", getOriginalOffset(*(uint*)animationMgr));
	}
	// LOGI("vftable = %u", getOriginalOffset(*(uint*)animationMgr));
	/*if (a2 == 6) {
		typedef int (*getWidget)(SexyString*);
		SexyString widgetName = "UIViewBoardOrZombies";
		int widget = ((getWidget)getActualOffset(0x270228))(&widgetName);

		typedef void (*setEnabled)(int, bool);
		((setEnabled)getActualOffset(0x270E98))(widget, 1);

		typedef void (*subD51CE4)(int, int);
		((subD51CE4)getActualOffset(0xD51CE4))(a1, 0);

		return;
	}*/
	oFun208(a1, a2);
}

typedef void (*fun209)(int, int);
fun209 oFun209 = NULL;

void hkFun209(int a1, int a2) {
	// return;
	oFun209(a1, a2);
}

typedef void (*fun210)(CowboyMinigameModule*, int);
fun210 oFun210 = NULL;

void hkFun210(CowboyMinigameModule* a1, int a2) {
	if (a2 != 1) {
		oFun210(a1, a2);
		return;
	}
	LOGI("custom move called");

	typedef void (*addAnimController)(AnimationMgr*, AnimationController*, float, float*);
	addAnimController pAddAnimController = (addAnimController)getActualOffset(0x10328C0);

	int startX, endX;

	typedef void (*getStartAndEnd)(int, int*, int*);
	((getStartAndEnd)getActualOffset(0x671824))(5, &startX, &endX);

	typedef MoveBoard* (*getMovement)(float, int, int, int, int, int);
	LOGI("Move board startX = %d endX = %d", startX, endX);
	MoveBoard* moveboard = ((getMovement)getActualOffset(0x334ED8))(2.5, startX, endX, 0, 0, 4);

	RtWeakPtr<AnimationMgr> animMgrPtr;
	animMgrPtr.FromOther(&a1->m_animationMgr);
	if (!animMgrPtr.is_valid()) {
		return;
	}
	AnimationMgr* animMgr = animMgrPtr.get();
	float m_time = animMgr->m_time;
	float endTime;

	pAddAnimController(animMgr, moveboard, m_time, &endTime);
	LOGI("Initial controllers size = %d to add = %d", animMgr->m_animControllers.size(), animMgr->m_animControllersToAdd.size());

	SexyString eventName = "onViewZombiesComplete";
	typedef TimeEvent* (*makeTimedEvent)(RtWeakPtr<CowboyMinigameModule>*, SexyString*);
	RtWeakPtr<CowboyMinigameModule> minigamePtr;
	minigamePtr.FromOther((RtWeakPtr<CowboyMinigameModule>*) &a1->m_thisPtr);
	TimeEvent* timeEvent = ((makeTimedEvent)getActualOffset(0x334F34))(&minigamePtr, &eventName);
	
	float tmp;
	// pAddAnimController(animMgr, timeEvent, endTime, &tmp);
}

typedef void (*fun211)(CowboyMinigameModule*, int);
fun211 oFun211 = NULL;

void hkFun211(CowboyMinigameModule* a1, int a2) {
	RtWeakPtr<AnimationMgr> animMgrPtr;
	animMgrPtr.FromOther(&a1->m_animationMgr);

	AnimationMgr* animMgr = animMgrPtr.get();
	LOGI("Controllers size = %d to add = %d", animMgr->m_animControllers.size(), animMgr->m_animControllersToAdd.size());
	oFun211(a1, a2);
}

typedef void (*fun212)(int);
fun212 oFun212 = NULL;

void hkFun212(int a1) { // test when seed chooser is killed
	LOGI("Standard intro completed");
	return;
}

typedef int (*seedChooserDestroy)(int);
seedChooserDestroy oSeedChooserDestroy = NULL;

int hkSeedChooserDestroy(int a1) {
	LOGI("Seed chooser destroy");
	LOG_RETURN_ADDRESS(0);
	return oSeedChooserDestroy(a1);
}

typedef void (*fun213)(int);
fun213 oFun213 = NULL;

void hkFun213(int a1) { // disable destroying the seed chooser widget
	LOGI("Seed chooser try to kill");

	/*auto zenGarden = GetUIWidget("UIZenGarden");
	zenGarden->SetEnabled(false);

	auto alamanac = GetUIWidget("UIAlmanac");
	alamanac->SetEnabled(false);*/

	SurvivalModule* module = (SurvivalModule*)GetBoard()->levelModuleManager->GetModule(SurvivalModule::GetRTClass());
	if (module && module->m_state == pannedtozombies) {
		module->ChangeState(resumepanning); // to check for after the seed chooser is dismissed
	}
	
	oFun213(a1);
}

typedef void (*fun215)(int, int, int);
fun215 oFun215 = NULL;

void hkFun215(int a1, int a2, int a3) {
	LOGI("Seed chooser press");
	int v12 = *(int*)(a1 + 0x180);
	LOGI("vftable of v12 is %u", getOriginalOffset(*(uint*)v12));

	oFun215(a1, a2, a3);
}

typedef void (*fun214)(int);
fun214 oFun214 = NULL;

void hkFun214(int a1) {
	// oFun214(a1);
}

typedef void (*setWidget)(int, int);
setWidget oSetWidget = NULL;

void hkSetWidget(int a1, int a2) {
	LOGI("Tries to set widget %d to %d", a1, a2);
	LOGI("widget vftable = %x", getOriginalOffset(*(uint*)a1));
	//if (SurvivalModule::brah) {
		// LOGI("Tries to set invalid widget, nah");
		LOG_RETURN_ADDRESS(0);
		//return;
	//}
	oSetWidget(a1, a2);
}

typedef void (*fun216)(int);
fun216 oFun216 = NULL;

void hkFun216(int a1) {
	LOGI("called fun216");
	oFun216(a1);
	// LOGI("fun216 finished");
}

typedef void (*fun217)(int);
fun217 oFun217 = NULL;

void hkFun217(int a1) {
	LOGI("fun 217 called");
	// LOGI("obj at 0x1D0 has vtable = %u", getOriginalOffset(**(uint**)(a1 + 0x1D0)));
	oFun217(a1);
	LOG_RETURN_ADDRESS(0);
	LOGI("fun 217 finished");
}

typedef void (*fun218)(int, int);
fun218 oFun218 = NULL;

void hkFun218(int a1, int a2) {
	LOGI("fun218 called");
	oFun218(a1, a2);
	LOGI("fun218 finished");
}

typedef void (*fun219)(uint a1, int a2, int r2_0);
fun219 oFun219 = NULL;

void hkFun219(uint a1, int a2, int r2_0) {
	LOGI("Release %d %d %d", a1, a2, r2_0);
	int v17 = *(int*)(a1 + 0x1A4);
	LOGI("v17 = %d", v17);
	if (v17) {
		LOGI("Vftable of v17 is %u", getOriginalOffset(*(uint*)v17));
	}
	RtWeakPtr<int> v218;
	v218.FromOther((RtWeakPtr<int>*)(a1 + 0x218));
	int* tmp = v218.get();
	LOGI("vftable of tmp is %u", getOriginalOffset(*(uint*)tmp));

	int* a = (int*)(a1 + 0x150);
	for (int i = 0; i < 0x3A; ++i) {
		int lol = 0x150 + 4 * i;
		LOGI("i = %d a[%x] = %d", i, 0x150 + 4 * i, a[i]);

		if (lol == 0x1a8 || lol == 0x1ac || (lol >= 0x158 && lol <= 0x16c) || a[i] < 0) {
			LOGI("vftable is %u", getOriginalOffset(*(uint*)a[i]));
		}
	}

	oFun219(a1, a2, r2_0);
}

typedef int (*fun220)(int a1);
fun220 oFun220 = NULL;
int hkFun220(int a1) {
	LOGI("Seed chooser's destructor called");
	return oFun220(a1);
}

typedef void (*fun221)(int);
fun221 oFun221 = NULL;

bool from = false;

void hkFun221(int a1) {
	LOGI("On ready set plant completed NEW");
	LOGI("a1 vftable = %x", getOriginalOffset(*(uint*)a1));
	/*from = true;
	oFun221(a1);
	from = false;*/
	
	typedef void (*sub722C34)(Board*);
	((sub722C34)getActualOffset(0x722C34))(getBoard());

	typedef void (*fun)(int);
	fun pFun = *(fun*)(a1 + 0x2C);
	LOGI("Got fun address = %x", *(uint*)(a1 + 0x1C));
	LOGI("use owner vtable = %d", *(uint*)(a1 + 0x20));
	pFun(a1 + 0x18); // this calls onStandardIntroComplete
	LOGI("Called fun");

	/*
	*(int*)(a1 + 0x1C) = 0;
	*(int*)(a1 + 0x20) = 0;
	*(int*)(a1 + 0x24) = 0;
	*(int*)(a1 + 0x28) = 0;
	*/
	
	typedef int (*sub12B185C)();
	int v3 = ((sub12B185C)getActualOffset(0x12B185C))();
	typedef void (*sub12B23D0)(int);
	((sub12B23D0)getActualOffset(0x12B23D0))(v3);

	typedef void (*sub5FDCB8)(int);
	((sub5FDCB8)getActualOffset(0x5FDCB8))((int) MessageRouter::GetInstance());
}

typedef void (*fun222)(Board*);
fun222 oFun222 = NULL;

void hkFun222(Board* a1) {
	if (!from) {
		oFun222(a1);
		return;
	}
	LOGI("disable 222 call");
	return;
}

typedef void (*fun223)(int);
fun223 oFun223 = NULL;

void hkFun223(int messageRouter) {
	LOGI("from = %d vftable = %u", from, getOriginalOffset(*(uint*)messageRouter));
	if (!from) {
		oFun223(messageRouter);
		return;
	}
	LOGI("Disable 223 call");
}

typedef void (*fun224)(Board*, int);
fun224 oFun224 = NULL;

void hkFun224(Board* board, int state) {
	if (state == 5) {
		float curTime = TimeMgr::GetInstance()->m_curTime;
		WaveManager* waveManager = GetWaveManager();
		waveManager->m_paused = false;
		waveManager->m_nextWaveTime = curTime;
		waveManager->m_lastWaveTime = curTime - 4.0;

		return;
		// typedef void (*doLevelStart)(LevelModuleManager*);
		// ((doLevelStart)getActualOffset(0x708D74))(board->levelModuleManager);
		// return;
	}
	oFun224(board, state);
}

typedef void (*fun225)(int, int);
fun225 oFun225 = NULL;

void hkFun225(int a1, int a2) {
	LOGI("seed packet reset nahh"); // nullsub_104 calls this
	// nope this just enables them for interaction...
	oFun225(a1, a2);
	return;
}

typedef void (*fun226)(int);
fun226 oFun226 = NULL;
void hkFun226(int a1) {
	LOGI("callback called 226");
	return;
}

typedef void (*fun227)(int);
fun227 oFun227 = NULL;

void hkFun227(int a1) {
	LOGI("fun 227 called");
	oFun227(a1);
	Board* board = GetBoard();
	RtWeakPtr<uint> tmp;
	tmp.FromOther((RtWeakPtr<uint>*)((uint)board + 0x300));
	uint* haha = tmp.get();
	LOGI("vftable of haha is at %x", getOriginalOffset(*haha));
	LOG_RETURN_ADDRESS(0);
	LOGI("fun 227 finished");
}

typedef void (*fun228)(SeedChooser*, int, void*);
fun228 oFun228 = NULL;

void hkFun228(SeedChooser* a1, int a2, void* a3) {
	/*LOGI("Fun 228 begin");
	if (!SurvivalModule::brah) {
		oFun228(a1, a2, a3);
	}
	else {
		typedef void (*slideFlow)(SlidingWidget*, int, void*);
		((slideFlow)getActualOffset(0xFECC70))(a1, a2, a3);
	}
	LOGI("fun 228 end");*/
	// LOG_RETURN_ADDRESS(0);
}

struct UnkStructSeed {
	char pad00[0x28];
	RtWeakPtr<int> seedPacket;
	int unkNum;
	char pad30[0xC];
};

void custom604F94(SeedChooser* a1, int a2) {
	LOGI("custom 604F94");

	*(int8_t*)(a2 + 0x48) = true;
	typedef int (*getCachedImage)(CachedResource*);
	int emptyPacket = ((getCachedImage)getActualOffset(0x286C2C))((CachedResource*)getActualOffset(0x1CE25A0));

	RtWeakPtr<int> unkPtr;
	typedef void (*sub10B5844)(RtWeakPtr<int>*, RtWeakPtr<int>*);
	((sub10B5844)getActualOffset(0x10B5844))(&unkPtr, (RtWeakPtr<int>*)(emptyPacket + 8));

	int v45[2];
	typedef void (*sub392D24)(int*, RtWeakPtr<int>*);
	((sub392D24)getActualOffset(0x392D24))(v45, &unkPtr);
	++v45[1];

	typedef void (*sub272218)(int*, int*, float);
	((sub272218)getActualOffset(0x272218))(&v45[0], &v45[1], 1536.0);

	std::vector<UnkStructSeed>* unkVec2 = (std::vector<UnkStructSeed>*)((uint)a1 + 0x164);
	LOGI("got unkVec2 at %u", (uint)unkVec2);
	int idx = 0;
	for (UnkStructSeed unkElement : *unkVec2) {
		RtWeakPtr<int> packet;
		packet.FromOther(&unkElement.seedPacket);

		int seedSlotsCount = *(int*)((uint)a1 + 0x174);
		// LOGI("iterate on unKVec2 idx = %d seed slots count = %d", idx, seedSlotsCount);
		int v9 = seedSlotsCount;
		if (idx < seedSlotsCount) {
			typedef void (*sub60650C)(float*, int, int);
			float coords[2];
			((sub60650C)getActualOffset(0x60650C))(coords, v9, idx);
			// LOGI("got coords");

			typedef int (*getScaledOffset)(int, uint);
			int scaledOffsetX = ((getScaledOffset)getActualOffset(0x377854))(getSexyApp(), -7u);
			int scaledOffsetY = ((getScaledOffset)getActualOffset(0x377854))(getSexyApp(), -5u);

			// LOGI("got offsets");

			float v14 = (float) v45[0];
			float v16 = (float) v45[1];
			int v17 = unkElement.unkNum;
			int theX = (coords[0] + v14) + scaledOffsetX;
			int theY = (coords[1] + v16) + scaledOffsetY;

			Color col = Color();
			if (v17 > 2) {
				col = Color(0x96, 0x96, 0x96, 0x8C);
			}
			else {
				col = Color(0x80, 0x80, 0x80, 0xB4);
			}

			*(Color*)(a2 + 0x30) = col;
			// LOGI("got color");

			int emptyPacket = ((getCachedImage)getActualOffset(0x286C2C))((CachedResource*)getActualOffset(0x1CE25A0));
			typedef void (*drawImage)(int, int, int, int);
			// LOGI("trynna draw");
			((drawImage)getActualOffset(0x11255D4))(a2, emptyPacket, theX, theY);
			// LOGI("Drew");

			if (v17 <= 2) {
				Color v22;
				typedef Color* (*getDmgFlashColor)(Color*, int);
				Color* res = ((getDmgFlashColor)getActualOffset(0x11143B0))(&v22, 1);

				*(Color*)(a2 + 0x30) = col;

				typedef void (*sub606648)(SeedChooser*, int, int, int);
				((sub606648)getActualOffset(0x606648))(a1, a2, theX, theY);
				
				int packetReal = (int) packet.get();
				int packetFlags = *(int*)(packetReal + 0x124);
				if (packetFlags & 0x80 != 0) {
					int packetsSelect = ((getCachedImage)getActualOffset(0x286C2C))((CachedResource*)getActualOffset(0x1CE2678));
					((drawImage)getActualOffset(0x11255D4))(a2, packetsSelect, theX, theY);
				}

			}
		}

		++idx;
	}
	
	LOGI("custom 604F94 finished");
}

typedef void (*fun229)(SlidingWidget*, int);
fun229 oFun229 = NULL;

void hkFun229(SeedChooser* a1, int a2) { // a2 is probably the graphics state?
	LOGI("CUSTOM fun 229 called");
	// LOGI("a2 vftable = %x", getOriginalOffset(*(uint*)a2));
	// oFun229(a1, a2);
	// LOGI("fun 229 ended");
	// return;

	typedef void (*fun1124614)(int);
	((fun1124614)getActualOffset(0x1124614))(a2);

	typedef void (*widgetFunc13)(UIWidget*);
	((widgetFunc13)getActualOffset(0x26EB64))(a1);

	typedef void (*sub26F460)(SlidingWidget*, int);
	((sub26F460)getActualOffset(0x26F460))(a1, a2);

	typedef Color* (*getDmgFlashColor)(Color*, int);
	Color v22;

	Color* res = ((getDmgFlashColor)getActualOffset(0x11143B0))(&v22, 1);
	*(int*)(a2 + 0x30) = res->red;
	*(int*)(a2 + 0x34) = res->green;
	*(int*)(a2 + 0x38) = res->blue;
	*(int*)(a2 + 0x3C) = res->alpha;
	*(int8_t*)(a2 + 0x48) = 0;

	// LOGI("color thingy finished");

	typedef void (*sub603F50)(SlidingWidget*, int);
	((sub603F50)getActualOffset(0x603F50))(a1, a2);

	typedef int (*getCachedResource)(CachedResource*);
	int cachedImage = ((getCachedResource)getActualOffset(0x296EF0))((CachedResource*)getActualOffset(0x1CE25B8));
	typedef void (*sub1134030)(int, int, int);
	((sub1134030)getActualOffset(0x1134030))(a2, (uint)a1 + 0x204, cachedImage);

	// LOGI("cached image stuff done");

	typedef void (*sub6040C8)(SeedChooser*, int);
	((sub6040C8)getActualOffset(0x6040C8))(a1, a2);

	typedef void (*sub606ACC)(SeedChooser*, int, int);
	sub606ACC fun606ACC = (sub606ACC)getActualOffset(0x606ACC);

	int uibutton1ptr = *(int*)((uint) a1 + 0x194);
	if (uibutton1ptr) {
		fun606ACC(a1, a2, uibutton1ptr);
	}

	// LOGI("uibutton1 stuff done");

	if ((a1->m_slidingState | 1) != 3) { // sliding state
		LOGI("Condition met");

		/*if (!SurvivalModule::brah) {
			typedef void (*sub604F94)(SeedChooser*, int);
			((sub604F94)getActualOffset(0x604F94))(a1, a2);
		}
		else {
			custom604F94(a1, a2);
		}*/

		custom604F94(a1, a2);

		// LOGI("mini sub finished");

		*(int8_t*)(a2 + 0x48) = 0;

		Color* res = ((getDmgFlashColor)getActualOffset(0x11143B0))(&v22, 1);
		*(int*)(a2 + 0x30) = res->red;
		*(int*)(a2 + 0x34) = res->green;
		*(int*)(a2 + 0x38) = res->blue;
		*(int*)(a2 + 0x3C) = res->alpha;

		// LOGI("colors finished");

		typedef int (*sub60539C)(SeedChooser*, int);
		((sub60539C)getActualOffset(0x60539C))(a1, a2);
	}

	// LOGI("sliding state pt 1 done");

	typedef void (*sub6053FC)(SeedChooser*, int);
	((sub6053FC)getActualOffset(0x6053FC))(a1, a2);

	typedef void (*sub6054D4)(SeedChooser*, int);
	((sub6054D4)getActualOffset(0x6054D4))(a1, a2);

	fun606ACC(a1, a2, *(int*)((uint) a1 + 0x188));
	fun606ACC(a1, a2, *(int*)((uint) a1 + 0x18C));
	fun606ACC(a1, a2, *(int*)((uint) a1 + 0x1A0));
	fun606ACC(a1, a2, *(int*)((uint)a1 + 0x1A4));

	int v19 = *(int*)((uint)a1 + 0x1AC);
	if (v19) {
		typedef void (*func)(int, int);
		(*(func*)(*(uint*)v19 + 0x90))(v19, a2);
	}
	int v20 = *(int*)((uint) a1 + 0x190);
	if (v20) {
		fun606ACC(a1, a2, v20);
	}

	// LOGI("idk rly");

	typedef void (*sub6055F4)(SeedChooser*, int);
	((sub6055F4)getActualOffset(0x6055F4))(a1, a2);

	typedef int (*sub6058AC)();
	int v21 = ((sub6058AC)getActualOffset(0x6058AC))();
	if (v21) {
		fun606ACC(a1, a2, *(int*)((uint) a1 + 0x198));
	}
	
	typedef int (*sub605988)();
	int v24 = ((sub605988)getActualOffset(0x605988))();
	if (v24) {
		fun606ACC(a1, a2, *(int*)((uint) a1 + 0x19C));
	}

	typedef void (*fun605A64)(SeedChooser*, int);
	((fun605A64)getActualOffset(0x605A64))(a1, a2);

	typedef void (*fun605BA8)(SeedChooser*, int);
	((fun605BA8)getActualOffset(0x605BA8))(a1, a2);

	// LOGI("haha");

	if ((a1->m_slidingState | 1) != 3) { // not sliding in or sliding out
		typedef void (*sub6061C8)(SeedChooser*, int);
		((sub6061C8)getActualOffset(0x6061C8))(a1, a2);
	}
	
	// LOGI("sliding state pt 2 finito");

	typedef void (*sub1124784)(int);
	((sub1124784)getActualOffset(0x1124784))(a2);

	LOGI("fun 229 ended");
}

typedef StandardLevelIntro* (*levelIntroCtor)();
levelIntroCtor oLevelIntroCtor = NULL;

static void* levelIntroVtable = NULL;

void OnChoosersDismissed(StandardLevelIntro* a1) {
	/*SurvivalModule* module = (SurvivalModule*) GetBoard()->levelModuleManager->GetModule(SurvivalModule::GetRTClass());
	if (module) {
		LOGI("Module exists on choosers dismissed");
		if (SurvivalModule::firstseed) {
			SurvivalModule::firstseed = false;
		}
		else {
			module->ChangeState(seedselectioncompleted);
		}
	}*/
}

//StandardLevelIntro* hkLevelIntroCtor() {
//	LOGI("Custom intro ctor");
//	StandardLevelIntro* v0 = oLevelIntroCtor();
//
//	if (levelIntroVtable == NULL) {
//		levelIntroVtable = copyVFTable(getActualOffset(0x1BDAC58), 30);
//		patchVFTable(levelIntroVtable, (void*)OnChoosersDismissed, 25);
//	}
//	setVFTable(v0, (uintptr_t)levelIntroVtable);
//
//	return v0;
//}

typedef void (*spawnStreetZombies)(int);
spawnStreetZombies oSpawnStreetZombies = NULL;

void hkSpawnStreetZombies(int a1) {
	LOGI("SPAWN STREET");
	LOGI("a1 vftable = %u", getOriginalOffset(*(uint*)a1));
	oSpawnStreetZombies(a1);
}

typedef int (*sub3ED7F8)(std::u32string*, int);
sub3ED7F8 oSub3ED7F8 = NULL;

int hk3ED7F8(std::u32string* a1, int a2) {
	LOGI("3ED7F8 called with type %d", a2);
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
	std::string utf8str = convert.to_bytes(*a1);
	LOGI("string %s", utf8str.c_str());
	return oSub3ED7F8(a1, a2);
}

typedef void (*getZombiesInWaveActions)(WaveManager*, std::vector<int>*);
getZombiesInWaveActions oGetZombies = NULL;

void hkGetZombies(WaveManager* waveManager, std::vector<int>* typenames) {
	SurvivalModule* module = (SurvivalModule*) GetBoard()->levelModuleManager->GetModule(SurvivalModule::GetRTClass());
	if (module == NULL) {
		oGetZombies(waveManager, typenames);
	}
	else {
		for (int i = waveManager->m_currentWave + 1; i <= waveManager->m_currentWave + module->wavesBetweenChoosers; ++i) {
			typedef void (*sub56E224)(WaveManager*, int, std::vector<int>*);
			((sub56E224)getActualOffset(0x56E224))(waveManager, i, typenames);
		}
	}
}

typedef void (*mixColors)(ZombieConditionTracker*);
mixColors oMixColors = NULL;

void hkMixColors(ZombieConditionTracker* a1) {
	oMixColors(a1);

	bool enableColorMix = false;
	bool flatColor = false;

	Color combinedColor(0, 0, 0, 50);
	a1->m_color1 = combinedColor;
	a1->m_color2 = combinedColor;
	enableColorMix = true;

	a1->m_struct->flatColorMode |= flatColor;
	a1->m_struct->colorMixMode |= enableColorMix;
}

typedef void (*handleMusic)(StageModule*);
handleMusic oHandleMusic = NULL;

// str1 = MiniGame_A str2 = WildWest

void hkHandleMusic(StageModule* a1) {
	if (a1->m_shouldStartMusic) {
		typedef StageModuleProperties* (*getProps)(StageModule*);
		auto props = ((getProps)getActualOffset(0x4C1034))(a1);

		SexyString str1, str2;
		typedef void (*sub4BE7DC)(SexyString*, StageModuleProperties*, SexyString*);
		((sub4BE7DC)getActualOffset(0x4BE7DC))(&str1, props, &str2);
		LOGI("str1 = %s str2 = %s", str1.c_str(), str2.c_str());
	}
	oHandleMusic(a1);
}

typedef void (*getMusic)(SexyString*, StageModuleProperties*, SexyString*);
getMusic oGetMusic = NULL;

void hkGetMusic(SexyString* musicType, StageModuleProperties* props, SexyString* suffix) {
	// this works in changing da music, yes
	oGetMusic(musicType, props, suffix);

	SurvivalModule* module = (SurvivalModule*) GetBoard()->levelModuleManager->GetModule(SurvivalModule::GetRTClass());
	if (module) {
		if (module->chooserCount < 3) {
			*musicType = "MainPath";
		}
		else {
			if (module->chooserCount % 5 == 4 || module->chooserCount % 5 == 2) {
				*musicType = "MiniGame_A";
			}
			else if (module->chooserCount % 5 == 1 || module->chooserCount % 5 == 3) {
				*musicType = "MiniGame_B";
			}
			else {
				*musicType = "MainPath";
			}
		}
		// *musicType = module->musicType;
	}
}

typedef void (*load)(Board*, int);
load oLoad = NULL;

void hkLoad(Board* a1, int a2) {
	LOGI("Load started");
	// oLoad(a1, a2);

	PVZDB* instance = PVZDB::GetInstance();
	typedef void (*subDDD04C)(PVZDB*);
	((subDDD04C)getActualOffset(0xDDD04C))(instance);

	LOGI("call part 1 finished");
	((subDDD04C)getActualOffset(0xDDD218))(instance);

	LOGI("call part 3 started");
	((subDDD04C)getActualOffset(0xDDD2D0))(instance);
	LOGI("call part 3 finished");
	
	typedef void (*subDDDCEC)(PVZDB*, int);
	((subDDDCEC)getActualOffset(0xDDDCEC))(instance, a2);

	LOGI("call part 4 finished");

	int v14[5];
	instance->GetAllInstancesOfType(v14, BoardSaveState);
	int saveStateInstance = (int) PVZDB::GetInstance<int>(v14);

	Board* board = GetBoard();

	typedef void (*resetup)(Board*, int);
	((resetup)getActualOffset(0x71F5AC))(board, saveStateInstance);

	typedef void (*entityDie)(int);
	((entityDie)getActualOffset(0x54BA18))(saveStateInstance);

	*(int*)((uint)board + 0x2C4) = board->m_boardState;
	board->ChangeState(2);

	typedef void (*clearInstancesArray)(int*);
	((clearInstancesArray)getActualOffset(0x10C5E40))(v14);

	LOGI("Load finished");
}

typedef void (*load2)(int, int);
load2 oLoad2 = NULL;

void hkLoad2(int a1, int a2) {
	//int v2 = *(int*)(a1 + 0x18);
	//int v3 = *(int*)(a1 + 0x1C); // there'some kinda vector holding objects of size 64
	//if (v3 != v2) {
	//	int v12 = 4;
	//	int v13 = 0;
	//	do {
	//		if (*(uint8_t*)(v2 + v13 * 64 + 0x19)) {
	//			int v14 = *(int*)(v2 + v13 * 64);

	//			typedef void* (*getV8)();
	//			getV8 pGetRegistry = (getV8)getActualOffset(OFFSET_RTWEAKPTR_GETREGISTRY);
	//			void* registry = pGetRegistry();

	//			typedef int (*sub10CC404)(void*, int);
	//			sub10CC404 pSub10CC404 = (sub10CC404)getActualOffset(OFFSET_RTWEAKPTR_UNKFUNC2);

	//			int v16 = pSub10CC404(registry, v14);
	//			
	//			typedef void (*sub10C69F0)(int, int);
	//			((sub10C69F0)getActualOffset(0x10C69F0))(v16, 0);
	//			
	//			typedef void (*sub35A424)(int, int, int);
	//			((sub35A424)getActualOffset(0x35A424))()
	//		}
	//	}
	//}
}

typedef int (*load3)(int, int, int);
load3 oLoad3 = NULL;

int hkLoad3(int* a1, int* a2, int *a3) {
	LOGI("load3 started");
	// int res = oLoad3(a1, a2, a3);

	// a2 is a vector with elements  of size 16

	int v37[3];
	memset(v37, 0, sizeof(v37));

	typedef void (*sub10CD7D8)(int*, int);
	((sub10CD7D8)getActualOffset(0x10CD7D8))(v37, (a2[1] - *a2) >> 4);
	((sub10CD7D8)getActualOffset(0x10CD88C))(v37, (a2[1] - *a2) >> 4);

	int begin = *a2;
	int end = a2[1];
	int v6, v7;
	if (a2[1] == *a2) {
		v6 = 0;
		v7 = 1;
	}
	else {
		int v9 = 4;
		int v10 = 0;
		int idx = 0;
		int v33 = 0;
		int v34 = 1;
		do {
			int v12 = 0;
			int* v13 = a1;
			int v14 = *(int*)(begin + 0x10 * idx) & 0xFFFFDFFF;
			if ((*(int*)(begin + 0x10 * idx) & 2000) == 0) {
				v13 = a1 + 3;
			}
			int v15 = v37[0];
			if (v14 < ((v13[1] - *v13) >> 2)) { // so v13 has a vector
				v12 = *(int*)(*v13 + 4 * v14);
			}
			int* buffer = (int*) operator new(0x1C);
			typedef void (*setupBuffer)(int*);
			((setupBuffer)getActualOffset(0x1114908))(buffer);
			*(int*)(v15 + v10) = (uint) buffer;

			typedef int (*sub10C0270)(int*, int, int*, int);
			sub10C0270 pFunc = (sub10C0270)getActualOffset(0x10C0270);

			int* sexyAppBase = *(int**)getActualOffset(0x1CF6CC8);

			if (pFunc(sexyAppBase, begin + v9, buffer, 1)) {
				int* v17 = (int*) operator new(0x18);
				typedef int (*sub11164A0)(int);
				int v18 = ((sub11164A0)getActualOffset(0x11164A0))(*(int*)(v15 + v10));
				int v19 = ((sub11164A0)getActualOffset(0x1114D0C))(*(int*)(v15 + v10));

				typedef void (*setup17)(int*, int, int);
				((setup17)getActualOffset(0x10D41C4))(v17, v18, v19);
				
				*(int*)(v15 + v10 + 4) = (uint)v17;

				int* v20 = (int*) operator new(0xC);
				*v20 = *(int*)(v15 + v10 + 4);
				v20[1] = 0;
				v20[2] = 1;

				int v21 = *(int*)(v15 + v10);
				*(int*)(v15 + v10 + 0xC) = (uint) v20;

				// TODO
				typedef int (*sub1114D0C)(int);
				int v22 = v33 + 4 * ((sub1114D0C)getActualOffset(0x1114D0C))(v21);
				v33 = v22;
				if (v34 & 1) {
					typedef int (*sub10C9AA4)(int, int, int);
					v34 = ((sub10C9AA4)getActualOffset(0x10C9AA4))(v12, *(int*)(v15 + v10 + 0xC), v15 + v10 + 0x10);
				}
				else {
					v34 = 0;
				}
			}
			else {
				typedef void (*sub10C69F0)(int, int);
				((sub10C69F0)getActualOffset(0x10C69F0))(v12, 0);
			}

			v9 += 0x10;
			v10 += 0x1C;
			++idx;
		} while (idx < ((end - begin) >> 4));

		if (end == begin) {
			v6 = v33;
			v7 = v34;
		}
		else {
			int v24 = 0;
			v7 = v34;
			int v25 = 0;
		}
	}

	if (a3) {
		*a3 = v6;
	}
	typedef void (*sub10CDA90)(int*);
	((sub10CDA90)getActualOffset(0x10CDA90))(v37);

	LOGI("load3 ended");
	return v7 & 1;
}

typedef void (*spawnSpore)(int, Plant*);
spawnSpore oSpawnSpore = NULL;

void hkSpawnSpore(int a1, Plant* a2) {
	int coords[2];
	typedef void (*getGridCoords)(int*, Plant*);
	((getGridCoords)getActualOffset(0xCDFF24))(coords, a2);

	SexyString plantName = "peashooter";
	typedef int (*getInstance)();
	int instance = ((getInstance)getActualOffset(0x26A600))();

	RtWeakPtr<int> plantType;
	typedef void (*getPlantType)(RtWeakPtr<int>*, int, SexyString*);
	((getPlantType)getActualOffset(0x26A66C))(&plantType, instance, &plantName);

	typedef int (*getBoardPlanting)();
	int boardPlanting = ((getBoardPlanting)getActualOffset(0x2AD5A0))();

	RtWeakPtr<int> plantTypeCpy;
	plantTypeCpy.FromOther(&plantType);

	typedef bool (*canPlantTypeGoHere)(int, int*, RtWeakPtr<int>*, int);
	bool canSpawn = ((canPlantTypeGoHere)getActualOffset(0x6490B8))(boardPlanting, coords, &plantTypeCpy, -1);

	if (canSpawn) {
		int v11[3] = { 0x100, 0, -1 };

		typedef int (*sub_6499C8)(int, int, int, SexyString*, int*);
		sub_6499C8 addPlantAt = (sub_6499C8)getActualOffset(0x6499C8);

		bool added = addPlantAt(boardPlanting, coords[0], coords[1], &plantName, v11);
	}
}

typedef void (*setupBankAndChooser)(SeedBankModule*);
setupBankAndChooser oSetupBankAndChooser = NULL;

void hkSetupBankAndChooser(SeedBankModule* a1) {
	oSetupBankAndChooser(a1);
	LOGI("original setup called");

	if (a1->mUsesSeedChooser) {
		LOGI("Running custom one");
		typedef int (*unkFunc1)();
		int v18 = ((unkFunc1)getActualOffset(0x10B5364))();
		LOGI("v18 got");
		typedef int (*unkFunc2)(int, int, int, int);
		int v19 = 0, v20 = 0;
		int v31 = ((unkFunc2)getActualOffset(0x10B5564))(v18, getActualOffset(0x01AFDDF8), v19, v20);
		LOGI("v31 done");
		typedef int (*unkFunc3)(int, int);
		((unkFunc3)getActualOffset(0x10B55F0))(v18, v31);
		LOGI("ran 10B55F0");
		int v32 = ((unkFunc3)getActualOffset(0x10B55A0))(v18, v31);
		LOGI("got v32");
		int v23 = ((unkFunc3)getActualOffset(0x2704E4))((int)&v31, 0);
		LOGI("seed chooser made");
		RtWeakPtr<int> a2;
		a2.FromOther((RtWeakPtr<int>*)(v23 + 8));
		typedef int (*moveWeakPtr)(RtWeakPtr<int>*, RtWeakPtr<int>*);
		((moveWeakPtr)getActualOffset(OFFSET_RTWEAKPTR_MOVE))((RtWeakPtr<int>*) & a1->m_seedChooser, &a2);
		LOGI("Moved to my seed chooser");

		((unkFunc3)getActualOffset(0x10B5618))(v18, v31);
		LOGI("do something");

		RtWeakPtr<SeedChooser> seedChooserPtr;
		seedChooserPtr.FromOther(& a1->m_seedChooser);

		SeedChooser* chooser = seedChooserPtr.get();
		typedef void (*setProps)(SeedChooser*, SeedBankProperties*);
		LOGI("set props func");

		typedef SeedBankProperties* (*getProps)(SeedBankModule*);
		auto props = ((getProps)getActualOffset(0xD59920))(a1);

		(*(setProps*)(*(uint*)chooser + 0xC0))(chooser, props);
		LOGI("Finished custom one");
	}
}

typedef SeedChooser* (*seedChooserDtor)(SeedChooser*);
seedChooserDtor oDtor = NULL;

SeedChooser* hkSeedChooserDtor(SeedChooser* a1) {

	return a1;
}

typedef void (*chooserSlideIntoIdle)(SeedChooser*);
chooserSlideIntoIdle oChooserSlideIntoIdle = NULL;

void hkChooserSlideIntoIdle(SeedChooser* a1) {
	oChooserSlideIntoIdle(a1);

	auto module = SurvivalModule::GetInstance();
	if (module) {
		auto chooserCount = module->chooserCount;
		if (chooserCount > 0) {
			// we don't want to enable zen garden or almanac
			UIWidget* zenGarden = GetUIWidget("UIZenGarden");
			zenGarden->SetEnabled(false);

			UIWidget* almanac = GetUIWidget("UIAlmanac");
			almanac->SetEnabled(false);
		}
	}
	// LOGI("Chooser 46");
	// LOG_RETURN_ADDRESS(0);
}

typedef void (*slideFlow)(SlidingWidget*, int, int);
slideFlow oSlideFlow = NULL;

void hkSlideFlow(SlidingWidget* a1, int a2, int a3) {
	if (*(uint*)a1 == getActualOffset(0x01BD8018)) {
		LOGI("curtime = %f", TimeMgr::GetInstance()->m_curTime);
		// LOGI("slide duration = %f start = %f", a1->m_slideTimeDuration, a1->m_slideTimeStart);
		LOGI("slideEndTime = %f", a1->m_slideTimeDuration + a1->m_slideTimeStart);
		LOGI("sliding state = %d", a1->m_slidingState);
	}
	oSlideFlow(a1, a2, a3);
}

typedef void (*getZombiePool)(int, int);
getZombiePool oGetZombiePool = NULL;

void hkGetZombiePool(int a1, int a2) {
	LOGI("Get zombie pool called");
	LOG_RETURN_ADDRESS(0);
	oGetZombiePool(a1, a2);
}

typedef void (*fun240)(int);
fun240 oFun240 = NULL;

void hkFun240(int a1) {
	LOG_RETURN_ADDRESS(0);
	LOGI("fun 240 called");
	oFun240(a1);
}

typedef void (*WaveMangerUpdateProgressMeter)(WaveManager*);
WaveMangerUpdateProgressMeter oWaveMangerUpdateProgressMeter = NULL;

void hkWaveMangerUpdateProgressMeter(WaveManager* a1) {
	if (a1->m_currentWave == -1 || a1->m_waves.empty()) {
		return;
	}
	typedef int (*GetWaveHitpoints)(int);
	int waveHitpoints = ((GetWaveHitpoints)getActualOffset(0xFF7FB0))(a1->m_currentWave);
	float totalHealth = a1->m_currentWaveTotalHealth;
	if (totalHealth <= 1) {
		totalHealth = 1;
	}
	int totalWaves = a1->m_waves.size() - 1;

	auto module = GetBoard()->levelModuleManager->GetModule(SurvivalModule::GetRTClass());
	if (module) {  // TOOD: This is only for endless. It'll break for normal Survival rn
		auto survivalModule = (SurvivalModule*)module;
		// subtract by 1 so that final wave is always at the end
		totalWaves = survivalModule->wavesBetweenChoosers - 1;
	}

	totalWaves = std::max(totalWaves, 1);
	// LOGI("Total waves = %d currentWave = %d", totalWaves, a1->m_currentWave);

	float healthPercentage = std::max(0.0f, (float)waveHitpoints / totalHealth);
	float eachWaveFill = 100.0 / totalWaves;

	float progressPercentage = eachWaveFill * a1->m_currentWave + eachWaveFill * (1 - std::min(healthPercentage, 1.0f));
	MessageRouter::GetInstance()->ExecuteMessage(Messages::Get(WaveManagerUpdateToPercent), progressPercentage);
}

typedef void (*fun270)(int, float);
fun270 oFun270 = NULL;

void hkFun270(int a1, float a2) {
	// LOGI("A1 = %d a2 = %f", a1, a2);
	oFun270(a1, a2);
}

void DisableProgressMeterFunc(UIWidget* a1) {
	a1->SetEnabled(false); // It should auto re-enable once waves arrive

	*(uint8_t*)(a1 + 0x111) = 0; // set flagsTriggered back to 0
}

typedef void (*WaveProgressMeterRegisterCallbacks)(UIWidget*);
WaveProgressMeterRegisterCallbacks oWaveProgressMeterRegisterCallbacks = NULL;

void hkWaveProgressMeterRegisterCallbacks(UIWidget* a1) {
	oWaveProgressMeterRegisterCallbacks(a1);

	// MessageRouter::GetInstance()->RegisterCallback(Messages::Get(DisableProgressMeter), *a1, DisableProgressMeterFunc);
	typedef void (*registerDelegate)(MessageRouter*, uint, Sexy::DelegateBase*);
	auto delegate = CreateDelegate(
		(uint)DisableProgressMeterFunc,
		false,
		(uint)a1,
		true,
		getActualOffset(0xCC1660)
	);
	((registerDelegate)getActualOffset(0x5FDA50))(MessageRouter::GetInstance(), Messages::Get(DisableProgressMeter), &delegate);
}

typedef void (*managerModuleProgress)(WaveManagerModule*);
managerModuleProgress oManagerModuleProgress = NULL;

void hkManagerModuleProgress(WaveManagerModule* a1) {
	// LOGI("Progress called for OG %u %u", (uint) a1, (uint) &a1->waveManager);
	LOGI("progress stuff");
	LOG_RETURN_ADDRESS(0);
	oManagerModuleProgress(a1);
}

typedef void (*getZombiePool2)(std::vector<RtWeakPtr<void>>*, LevelEscalationModule*);
getZombiePool2 oGetZombiePool2 = NULL;

void hkGetZombiePool2(std::vector<RtWeakPtr<void>>* res, LevelEscalationModule* a1) {
	// oGetZombiePool2(res, a1);
	// return;

	RtWeakPtr<LevelEscalationModuleProperties> propsPtr;
	propsPtr.FromOther((RtWeakPtr<LevelEscalationModuleProperties>*) & a1->propSheetPtr);
	auto props = propsPtr.get();

	res->clear();
	res->shrink_to_fit(); // ...why did this stop the crash...

	res->resize(props->ZombiePool.size());
	for (int i = 0; i < props->ZombiePool.size(); ++i) {
		(*res)[i].FromOther(&props->ZombiePool[i]);
	}
}

typedef void (*fun273)(LevelModuleManager*);
fun273 oFun273 = NULL;

void hkFun273(LevelModuleManager* a1) {
	LOGI("fun 273 called");

	LOGI("loaded resources = %d", a1->m_loadedResourceGroups.size());
	oFun273(a1);
	LOGI("loaded resources after = %d", a1->m_loadedResourceGroups.size());
}

typedef void (*fun274)(int, std::vector<SexyString>*);
fun274 oFun274 = NULL;

void hkFun274(int a1, std::vector<SexyString>* a2) {
	for (auto& a3 : *a2) {
		LOGI("A3 = %s", a3.c_str());
	}
	oFun274(a1, a2);
}

typedef void (*fun275)(int, WaveManagerProperties*);
fun275 oFun275 = NULL;

void hkFun275(int a1, WaveManagerProperties* a2) {
	WaveManagerGenerationData* a3 = (WaveManagerGenerationData*)a1;
	LOGI("%d %d %d %d %d %d %d %d %ld %d", a3->unk, a3->TotalWaves, a3->WavesPerFlag, a3->unk4, a3->unk5, a3->unk6,
		a3->StartingPoints, a3->PointIncrementPerWave, a3->RandSeed, a3->MaxPoints);
	for (auto& a4 : a3->GenerationData) {
		LOGI("%d %d %d", a4.id, a4.plantfoodCount, a4.suppressDynamicZombies);
		LOGI("Zombie types size = %d", a4.zombieTypes.size());
		for (auto& zombieType : a4.zombieTypes) {
			LOGI("%s %s", zombieType->TypeName.c_str(), zombieType->HomeWorld.c_str());
		}
		LOGI("Unk vec2 size = %d", a4.unkVec2.size());
		int* tmp = (int*) & a4.unkVec2;
		LOGI("%d %d %d", tmp[0], tmp[1], tmp[2]);
	}

	/*
	auto endlessModule = GetBoard()->levelModuleManager->GetModule(WaveManagerEndlessModule::GetRTClass());
	if (endlessModule) {
		auto moduleCasted = (WaveManagerEndlessModule*)endlessModule;
		*(int*)(a1 + 0x18) = moduleCasted->m_currentPoints;
		LOGI("Set starting points to %d", moduleCasted->m_currentPoints);
	}
	*/
	oFun275(a1, a2);
	LOGI("fun 275 finito");
}

typedef void (*runEvent5)(LevelModuleManager*);
runEvent5 oRunEvent5 = NULL;

void hkRunEvent5(LevelModuleManager* a1) {
	oRunEvent5(a1);
}

typedef void (*levelName)(int, Board*);
levelName oLevelName = NULL;

void hkLevelName(std::u32string* a1, Board* a2) {
	const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<> dist(0, characters.size() - 1);

	SexyString levelName;
	for (int i = 0; i < 10; ++i) {
		levelName += characters[dist(mt)];
	}

	// std::u32string advice = U"bruh moment";

	/*std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
	std::u32string advice = convert.from_bytes(levelName);

	*(std::u32string*)((uint)GetBoard() + 0x32C) = advice;
	*(uint*)((uint)GetBoard() + 0x358) = NULL;*/

	/*int v11 = *(uint*)((uint)GetBoard() + 0x358);
	if (v11) {
		LOGI("vtable at 0x358 of board is %x", getOriginalOffset(*(uint*)v11));
	}*/

	GetBoard()->ChangeLevelName(levelName);

	oLevelName((int) a1, a2);
	/*std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
	std::string utf8str = convert.to_bytes(*a1);
	LOGI("string %s", utf8str.c_str());

	int addr = ((int*)a1)[2];
	int* board = (int*) GetBoard();
	LOGI("val here = %d", *(int*)((uint) board + 0x32C));
	LOGI("Val more = %d", *(int*)((uint) board + 0x330));
	for (int i = 0; i < sizeof(Board) / 4; ++i) {
		if (board[i] == 25 || board[i] == 23) {
			LOGI("i is good %d %d", i, board[i]);
		}
	}*/

	// LOG_RETURN_ADDRESS(0);
}

typedef void (*getRandomPortals)(int, int, int, int);
getRandomPortals oGetRandomPortals = NULL;

void hkGetRandomPortals(std::vector<StringWeightedOption*>* a1, std::vector<StringWeightedOption*>* a2, int a3, int* a4) {
	// LOGI("portal types count = %d %d %d %d", a1, a2, a3, a4);
	oGetRandomPortals((int) a1, (int) a2, a3, (int) a4);
	for (StringWeightedOption* portalType : *a1) {
		LOGI("portalType name = %s %f", portalType->Value.c_str(), portalType->Weight);
	}
}

void initSurvivalHooks() {
	PVZ2HookFunction(0x720C0C, (void*)hkFun213, (void**)&oFun213, "seed chooser kill");

	PVZ2HookFunction(0x56E1D4, (void*)hkGetZombies, (void**)&oGetZombies, "get zombies for next screen");
	// PVZ2HookFunction(0x323524, (void*)hkMixColors, (void**)&oMixColors, "mix colors");

	PVZ2HookFunction(0x618470, (void*)hkChooserSlideIntoIdle, (void**)&oChooserSlideIntoIdle, "On slide into idle state");

	// PVZ2HookFunction(0xFECC70, (void*)hkSlideFlow, (void**)&oSlideFlow, "slide flow");

	// PVZ2HookFunction(0x6BC1F8, (void*)hkGetZombiePool, (void**)&oGetZombiePool, "get zombie pool");
	// PVZ2HookFunction(0x6BBA64, (void*)hkFun240, (void**)&oFun240, "fun 240");

	PVZ2HookFunction(0x56ECE4, (void*)hkWaveMangerUpdateProgressMeter, (void**)&oWaveMangerUpdateProgressMeter, "Wave manager update progress meter");

	// PVZ2HookFunction(0xCC05C4, (void*)hkFun270, (void**)&oFun270, "fun 270");
	
	PVZ2HookFunction(0xCC0228, (void*)hkWaveProgressMeterRegisterCallbacks, (void**)&oWaveProgressMeterRegisterCallbacks, "Wave meter register callbacks");
	
	// PVZ2HookFunction(0x56AF4C, (void*)hkManagerModuleProgress, (void**)&oManagerModuleProgress, "manager module progress");
	PVZ2HookFunction(0x6BC1F8, (void*)hkGetZombiePool2, (void**)&oGetZombiePool2, "Get zombie pool");

	PVZ2HookFunction(0x90BEE4, (void*)hkGetRandomPortals, (void**)&oGetRandomPortals, "get random portals");

	SurvivalModule::ModInit();
	SurvivalModuleProps::ModInit();

	DummyModule::ModInit();
	DummyModuleProps::ModInit();

	WaveManagerEndlessModule::ModInit();
	WaveManagerEndlessModuleProps::ModInit();
}

/*
class WidgetContainer : public RtObject {
public:
	std::vector<int>				mWidgets;
	int* mWidgetManager;
	WidgetContainer* mParent;

	bool					mUgpdateIteratorModified;
	int	mUpdateIterator;
	ulong					mLastWMUpdateCount;
	int						mUpdateCnt;
	bool					mDirty;
	int						mX;
	int						mY;
	int						mWidth;
	int						mHeight;
	bool					mHasAlpha;
	bool					mClip;
	int				mWidgetFlagsMod;
	int						mPriority;
	int						mZOrder;
};

static_assert(offsetof(WidgetContainer, mWidgetFlagsMod) == 0x40); // it's supposed to be at 40
static_assert(offsetof(WidgetContainer, mX) == 44); // it's supposed to be at 36

class Widget : public WidgetContainer {
public:
	bool					mVisible;
	bool					mMouseVisible;
	bool					mDisabled;
	bool					mHasFocus;
	bool					mIsDown;
	bool					mIsOver;
	bool					mHasTransparencies;
	std::vector<int>				mColors;
	Rect					mMouseInsets;
	bool					mDoFinger;
	bool					mWantsFocus;

	Widget* mTabPrev;
	Widget* mTabNext;
};

enum LayoutFlags
{
	LAY_SameWidth = 0x0001,
	LAY_SameHeight = 0x0002,

	LAY_SetLeft = 0x0010,
	LAY_SetTop = 0x0020,
	LAY_SetWidth = 0x0040,
	LAY_SetHeight = 0x0080,

	LAY_Above = 0x0100,
	LAY_Below = 0x0200,
	LAY_Right = 0x0400,
	LAY_Left = 0x0800,

	LAY_SameLeft = 0x1000,
	LAY_SameRight = 0x2000,
	LAY_SameTop = 0x4000,
	LAY_SameBottom = 0x8000,

	LAY_GrowToRight = 0x10000,
	LAY_GrowToLeft = 0x20000,
	LAY_GrowToTop = 0x40000,
	LAY_GrowToBottom = 0x80000,

	LAY_HCenter = 0x100000,
	LAY_VCenter = 0x200000,
	LAY_Max = 0x400000,

	LAY_SameSize = LAY_SameWidth | LAY_SameHeight,
	LAY_SameCorner = LAY_SameLeft | LAY_SameTop,
	LAY_SetPos = LAY_SetLeft | LAY_SetTop,
	LAY_SetSize = LAY_SetWidth | LAY_SetHeight
};

static_assert(offsetof(Widget, mMouseInsets) == 0x60); // it's supposed to be at 0x64
*/