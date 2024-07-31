#include "pch.h"
#include "SurvivalModule.h"
#include "LevelModuleManager.h"
#include "PvZ2/Board.h"
#include "WaveManager.h"
#include "Zombie.h"
#include "TimeMgr.h"
#include "MessageRouter.h"
#include "ZombieStateHelper.h"
#include "SeedBankModule.h"
#include "DummyModule.h"
#include "DummyModuleProps.h"
#include "SeedPacket.h"
#include "Messages.h"

DEFINE_STATIC_RT_CLASS_MEMBERS(SurvivalModule);

DECLARE_DELEGATES_SETUP(SurvivalModule);
DECLARE_CALLBACK(SurvivalModule, onViewZombiesComplete);
DECLARE_CALLBACK(SurvivalModule, onResume);
DECLARE_CALLBACK(SurvivalModule, onResumeGame);
DECLARE_CALLBACK(SurvivalModule, onShowMoreApproachingComplete);
DECLARE_CALLBACK(SurvivalModule, onViewBoardComplete);

//AnimationMgr* getAnimationMgr(SurvivalModule* self) {
//	RtWeakPtr<AnimationMgr> animMgrPtr;
//	animMgrPtr.FromOther(&self->m_animationMgr);
//	if (!animMgrPtr.is_valid()) {
//		return NULL;
//	}
//	return animMgrPtr.get();
//}
//

bool CheckInvulnerableZombieEndable(SurvivalModule* self) {
	// TODO: we should find a way to kill out the invulnerable zombies, else it'd get inefficient fast...
	typedef bool (*findValidZombies)();
	bool existsZombies = ((findValidZombies)getActualOffset(0xDBA038))();
	if (existsZombies) {
		if (self->invulnZombieFound) {
			return TimeMgr::GetInstance()->m_curTime >= self->invulnZombieTargetTime;
		}
		else {
			self->invulnZombieFound = true;
			self->invulnZombieTargetTime = TimeMgr::GetInstance()->m_curTime + 16.0;
		}
	}
	else {
		self->invulnZombieFound = false;
		self->invulnZombieTargetTime = MAXFLOAT;
	}
	return false;
}

void CheckCurrentWave(SurvivalModule* self) {
	WaveManager* waveManager = GetWaveManager();
	if (waveManager == NULL) {
		return;
	}

	// LOGI("Current wave is %d pause = %d waveCount = %d", waveManager->m_currentWave, waveManager->IsPaused(), waveManager->m_waves.size());
	if (waveManager->m_currentWave != self->lastPausedAt &&
		waveManager->m_currentWave + 1 != waveManager->m_waves.size() && 
		(waveManager->m_currentWave + 1) % self->wavesBetweenChoosers == 0 && 
		!waveManager->IsPaused()) {
		LOGI("Suppress wave manager");
		// SetSuppressedState(waveManager, true); // pause would be more appropriate here. Also set Board->plantsCanAttack to false
		self->lastPausedAt = waveManager->m_currentWave;
		waveManager->SetPaused(true);
	}

	if (self->m_state == playing && GetWaveManager()->IsPaused()) {
		typedef bool (*winCon1)();
		bool res = ((winCon1)getActualOffset(0xDB9DD0))() || CheckInvulnerableZombieEndable(self);
		if (res) {
			LOGI("added zombies size = %d destroyed zombies size = %d", GetBoard()->addedZombies.size(), GetBoard()->destroyedZombiesVec.size());
			self->ChangeState(playingfinished);
		}
	}

	// I'm no longer maintaining my own animation manager
	//typedef void (*moveStagedControllers)(AnimationMgr*); // from toAdd into the list
	//((moveStagedControllers)getActualOffset(0x103298C))(getAnimationMgr(self));

	/*typedef int (*getSeedChooser)(Board*);
	int seedChooserWidget = ((getSeedChooser)getActualOffset(0x720AFC))(GetBoard());
	int seedBankWidget = ((getSeedChooser)getActualOffset(0x7209CC))(GetBoard());*/
	/*if (seedChooserWidget) {
		LOGI("seed chooser exists!");
	}
	else {
		LOGI("ded seed chooser");
	}*/
	/*if (seedBankWidget) {
		LOGI("seed bank exists!");
	}
	else {
		LOGI("seed bank is kil");
	}*/

	// LOGI("Check current wave code finished");
}

void SurvivalOnZombieDeath(SurvivalModule* self, Zombie* a2) {
	// LOGI("Zombie death self state = %d wave manager paused = %d", self->m_state, GetWaveManager()->IsPaused());
	if (self->m_state == playing && GetWaveManager()->IsPaused()) {
		typedef int (*pvzDBGet)();
		int instance = ((pvzDBGet)getActualOffset(0x274CE4))();
		
		int v20[5];
		typedef void (*getAllInstances)(int*, int, int);
		((getAllInstances)getActualOffset(0xDDCEB4))(v20, instance, 0x43); // zombie type

		int aliveZombieCount = 0;

		while (v20[2] != v20[4]) {
			RtWeakPtr<Zombie> zombiePtrCopy;
			typedef void (*getZombiePtr)(RtWeakPtr<Zombie>*, int*);
			((getZombiePtr)getActualOffset(0x10C5EF8))(&zombiePtrCopy, v20);

			RtWeakPtr<Zombie> zombiePtr;
			zombiePtr.FromOther(&zombiePtrCopy);

			Zombie* zombie = zombiePtr.get();
			aliveZombieCount += zombie->IsDeadOrDying() ^ 1;

			if (!zombie->IsDeadOrDying()) {
				RtWeakPtr<ZombieType> type;
				type.FromOther(&zombie->m_type);
				auto val = type.get();
				// LOGI("Val name = %s", val->ZombieClass.c_str());
				// LOGI("zombie position = %f %f %f\n", zombie->m_position.mX, zombie->m_position.mY, zombie->m_position.mZ);
			}

			typedef void (*sub10C5F58)(int*);
			((sub10C5F58)getActualOffset(0x10C5F58))(v20);
		}
		typedef void (*sub10C5E40)(int*);
		((sub10C5E40)getActualOffset(0x10C5E40))(v20);

		LOGI("Alive zombie count = %d", aliveZombieCount);

		/*
		LOGI("GetBoard()->addedZombies size = %d", GetBoard()->addedZombies.size());
		for (Zombie* zombie : GetBoard()->addedZombies) {
			RtWeakPtr<ZombieType> type;
			type.FromOther(&zombie->m_type);
			auto val = type.get();
			LOGI("Val name = %s", val->ZombieClass.c_str());
		}
		LOGI("destroyed zombies size = %d", GetBoard()->destroyedZombiesVec.size());
		*/

		if (aliveZombieCount == 0) {
			self->ChangeState(playingfinished);
		}
	}
}
//
//void AnimatePanning(SurvivalModule* self, int panDirection) {
//	// 1 pans to zombies, 2 pans backwards
//	// 
//	// TODO: Enable UIViewBoardOrZombies widget
//
//	RtWeakPtr<AnimationMgr> animMgrPtr;
//	animMgrPtr.FromOther(&self->m_animationMgr);
//	if (!animMgrPtr.is_valid()) {
//		return;
//	}
//
//	typedef void (*addAnimController)(AnimationMgr*, AnimationController*, float, float*);
//	addAnimController pAddAnimController = (addAnimController)getActualOffset(0x10328C0);
//
// 	AnimationMgr* animMgr = animMgrPtr.get();
//	int startX = 0, endX = 0;
//	MoveBoard* moveboard;
//
//	float endTime;
//
//	if (panDirection == 1) {
//		typedef void (*getStartAndEnd)(int, int*, int*);
//		((getStartAndEnd)getActualOffset(0x671824))(5, &startX, &endX);
//		
//		typedef MoveBoard* (*getMovement)(float, int, int, int, int, int);
//		LOGI("Move board startX = %d endX = %d", startX, endX);
//		moveboard = ((getMovement)getActualOffset(0x334ED8))(2.5, startX, endX, 0, 0, 4);
//
//		float m_time = animMgr->m_time;
//
//		pAddAnimController(animMgr, moveboard, m_time, &endTime);
//
//		LOGI("m_time = %f endTime = %f", m_time, endTime);
//
//		// add a callback to set the state and then we can bring up the seed chooser
//
//		SexyString eventName = "onViewZombiesComplete";
//		typedef TimeEvent* (*makeTimeEvent)(RtWeakPtr<SurvivalModule>*, SexyString*);
//		RtWeakPtr<SurvivalModule> thisPtr;
//		thisPtr.FromOther((RtWeakPtr<SurvivalModule>*) & self->m_thisPtr);
//		TimeEvent* timeEvent = ((makeTimeEvent)GetActualOffset(0x334F34))(&thisPtr, &eventName);
//
//		float tmp;
//		pAddAnimController(animMgr, timeEvent, endTime, &tmp);
//	}
//	else {
//		GetStartXAndEndX(4, startX, endX);
//		moveboard = MakeMoveBoard(2.5, startX, endX, 0, 0, 4);
//		float m_time = animMgr->m_time;
//		pAddAnimController(animMgr, moveboard, m_time, &endTime);
//
//		SexyString eventName = "onSeedSelectionComplete";
//
//		RtWeakPtr<SurvivalModule> thisPtr;
//		thisPtr.FromOther((RtWeakPtr<SurvivalModule>*) & self->m_thisPtr);
//
//		TimeEvent* timeEvent = MakeTimeEvent((RtWeakPtr<GameObject>*) &thisPtr, &eventName);
//
//		float tmp;
//		pAddAnimController(animMgr, timeEvent, endTime, &tmp);
//	}
//}
//

void CreateFreshSeedChooser(SurvivalModule* self) {
	typedef SeedBankModule* (*getSeedBankModule)(Board*);
	auto module = ((getSeedBankModule)getActualOffset(0x720ADC))(GetBoard());

	// construct the seedChooser
	typedef int (*unkFunc1)();
	int v18 = ((unkFunc1)getActualOffset(0x10B5364))();
	typedef int (*unkFunc2)(int, int, int, int);
	int v19 = 0, v20 = 0;
	int v31 = ((unkFunc2)getActualOffset(0x10B5564))(v18, getActualOffset(0x01AFDDF8), v19, v20);
	typedef int (*unkFunc3)(int, int);
	((unkFunc3)getActualOffset(0x10B55F0))(v18, v31);
	int v32 = ((unkFunc3)getActualOffset(0x10B55A0))(v18, v31);
	int v23 = ((unkFunc3)getActualOffset(0x2704E4))((int)&v31, 0);

	RtWeakPtr<int> a2;
	a2.FromOther((RtWeakPtr<int>*)(v23 + 8));
	typedef int (*moveWeakPtr)(RtWeakPtr<int>*, RtWeakPtr<int>*);
	((moveWeakPtr)getActualOffset(OFFSET_RTWEAKPTR_MOVE))((RtWeakPtr<int>*) & module->m_seedChooser, &a2);
	// LOGI("Moved to my seed chooser");

	((unkFunc3)getActualOffset(0x10B5618))(v18, v31);
	// LOGI("do something");

	RtWeakPtr<SeedChooser> seedChooserPtr;
	seedChooserPtr.FromOther(&module->m_seedChooser);

	SeedChooser* chooser = seedChooserPtr.get();
	typedef void (*setProps)(SeedChooser*, SeedBankProperties*);
	// LOGI("set props func");

	typedef SeedBankProperties* (*getProps)(SeedBankModule*);
	auto props = ((getProps)getActualOffset(0xD59920))(module);
	
	// dummy props
	/*auto dummyModule = GetBoard()->levelModuleManager->GetModule(DummyModule::GetRTClass());
	LOGI("dummy module = %u", (uint)dummyModule);
	RtWeakPtr<DummyModuleProps> dummyProps;
	dummyProps.FromOther((RtWeakPtr<DummyModuleProps>*) & dummyModule->propSheetPtr);
	LOGI("props from other");
	auto props = dummyProps.get();*/

	// LOGI("props obtained...");

	(*(setProps*)(*(uint*)chooser + 0xC0))(chooser, props);
	++self->chooserCount;
	// LOGI("set props ran");
}

void BringUpSeedChooser(SurvivalModule* self) {
	typedef void (*func)();
	func pFunc = (func)getActualOffset(0x335364);
	pFunc();
	return;
	
	typedef SeedBankModule* (*getSeedBankModule)(Board*);
	auto module = ((getSeedBankModule)getActualOffset(0x720ADC))(GetBoard());
	LOGI("got module %d", module);
	typedef SeedBankProperties* (*getProps)(SeedBankModule*);
	auto props = ((getProps)getActualOffset(0xD59920))(module);

	// LOGI("Got props %d", props);

	// CreateFreshSeedChooser(self);

	typedef void (*setEnabled)(SeedChooser*, bool);
	// ((setEnabled)getActualOffset(0x270E98))(chooser, 1);
	// pFunc();

	LOGI("set enabled");

	return;

	/*
	typedef int (*getSeedChooser)(Board*);
	int seedChooserWidget = ((getSeedChooser)getActualOffset(0x720AFC))(GetBoard());

	LOGI("Get seed chooser widget addr = %u", (uint)seedChooserWidget);
	LOGI("Get seed chooser widget addr = %u", (uint)seedChooserWidget);

	RtWeakPtr<AnimationMgr> boardAnimMgrPtr;
	boardAnimMgrPtr.FromOther(&GetBoard()->m_animationMgr);
	AnimationMgr* animMgr = boardAnimMgrPtr.get();

	LOGI("Get animMgr pointer");

	typedef int (*getSeedBank)(Board*);
	int a = ((getSeedBank)getActualOffset(0x7209CC))(GetBoard());
	LOGI("Seed bank vtable = %u", getOriginalOffset(*(uint*)a));

	animMgr->m_paused = true;
	LOGI("Set paused");
	typedef void (*setEnabled)(int, bool);

	// ((setEnabled)GetActualOffset(0x270E98))(a, 0);
	// LOGI("Disabled seed bank");

	((setEnabled)getActualOffset(0x270E98))(seedChooserWidget, 1);
	LOGI("Enable seed chooser");

	return;

	typedef int (*getWidget)(SexyString*);
	SexyString widgetName = "UIStartGameButton";
	int widget = ((getWidget)getActualOffset(0x270228))(&widgetName);
	LOGI("start game widget address = %d", widget);

	widgetName = "UIViewBoardOrZombies";
	widget = ((getWidget)getActualOffset(0x270228))(&widgetName);
	((setEnabled)getActualOffset(0x270E98))(widget, 1);
	LOGI("view widget address = %d", widget);

	widgetName = "UIReloadLayoutButton";
	widget = ((getWidget)getActualOffset(0x270228))(&widgetName);
	LOGI("reload layout widget address = %d", widget);
	((setEnabled)GetActualOffset(0x270E98))(widget, 1);
	*/
}
//
//bool SurvivalModule::brah = false;
//bool SurvivalModule::firstseed = false;
//
//void SurvivalModule::ChangeState(SurvivalModuleState state) {
//	LOGI("Changed state to %d", state);
//	if (state == panningtozombies) {
//		AnimatePanning(this, 1);
//	}
//	if (state == pannedtozombies) {
//		// bring up seed chooser
//		typedef void (*changeState)(Board*, int);
//		((changeState)getActualOffset(0x71B1AC))(GetBoard(), 3);
//		BringUpSeedChooser(this);
//	}
//	else if (state == seedselectioncompleted) {
//		typedef void (*changeState)(Board*, int);
//		((changeState)getActualOffset(0x71B1AC))(GetBoard(), 5);
//		AnimatePanning(this, 2);
//	}
//
//	this->m_state = state;
//}
//
void SurvivalSetup(SurvivalModule* self) {
	/*
	self->ChangeState(levelintro);

	typedef AnimationMgr* (*createAnimationMgr)(Sexy::RtClass*, int, int);
	AnimationMgr* animMgr = ((createAnimationMgr)getActualOffset(0x54B580))(AnimationMgr::GetRTClass(), 0x41, 1);
	
	self->m_animationMgr.FromOther((RtWeakPtr<AnimationMgr>*) & animMgr->m_thisPtr);
	*/
	self->wavesBetweenChoosers = self->GetProps()->WavesBetweenChoosers;
	self->flagsBetweenChoosers = self->GetProps()->FlagsBetweenChoosers;
}
//
//void SurvivalModuleRenderAnimMgr(SurvivalModule* self, int a2) {
//	RtWeakPtr<AnimationMgr> animMgrPtr;
//	animMgrPtr.FromOther(&self->m_animationMgr);
//
//	AnimationMgr* animMgr = animMgrPtr.get();
//	// LOGI("Controllers size = %d To add = %d", animMgr->m_animControllers.size(), animMgr->m_animControllersToAdd.size());
//	animMgr->Func13(a2);
//}
//
//void ClearAnimMgr(SurvivalModule* self) {
//	RtWeakPtr<AnimationMgr> animMgrPtr;
//	animMgrPtr.FromOther(&self->m_animationMgr);
//
//	AnimationMgr* animMgr = animMgrPtr.get();
//	typedef void (*clearAnimMgr)(AnimationMgr*);
//	((clearAnimMgr)getActualOffset(0x103331C))(animMgr);
//}
//
//void SurvivalTransitionFromLoading(SurvivalModule* self) {
//	LOGI("Transition from loading called");
//
//	SurvivalModule::brah = false;
//	SurvivalModule::firstseed = true;
//	return;
//
//	Board* board = GetBoard();
//	typedef void (*func)(Board*, int, int);
//	func pFunc = *(func*)(*(uint*) board + 0xD4); // renders the board with the offset (x, y)
//	pFunc(board, -*(int*)((uint) board + 0x368), 0);
//}
//
//void SurvivalIntro(SurvivalModule* self) {
//	return;
//	LOGI("Intro");
//	typedef void (*setTutorial)(Board*);
//	((setTutorial)getActualOffset(0x728550))(GetBoard());
//	auto waveManager = GetWaveManager();
//	if (waveManager) {
//		SetSuppressedState(waveManager, true);
//	}
//
//	self->m_state = levelintro;
//	// typedef void (*BoardStart)(Board*);
//	// ((BoardStart)getActualOffset(0x7280C4))(GetBoard());
//}
//

void SurvivalIntro(SurvivalModule* self) {
	if (self->m_state == initial) {
		self->ChangeState(levelintro);
	}
}

void SurvivalStart(SurvivalModule* self) {
	LOGI("Start event called");

	if (self->m_state == levelintro) {
		self->chooserCount++; // initial chooser accounted for
		self->ChangeState(playing);
	}
}

void SurvivalViewBoardOrZombies(SurvivalModule* self) {
	/*LOGI("view board or zombies pressed");
	auto state = self->m_state;
	if (state == pannedtozombies) {
		self->ChangeState(panningtoboard);
	}

	if (state == pannedtoboard) {
		self->ChangeState(panningtozombies);
	}*/

	// This makes the SeedChooser not kill itself when pulled down
	typedef SeedBankNew* (*GetSeedBank)(Board*);
	auto seedBank = ((GetSeedBank)getActualOffset(0x7209CC))(GetBoard());
	seedBank->SetEnabled(false);
}

void SurvivalModuleNextWave(SurvivalModule* self, int nextWaveIndex, WaveType waveType, bool isFinalWave) {
	LOGI("Next wave called %d %d %d", nextWaveIndex, waveType, isFinalWave);
	//WaveManager* waveManager = GetWaveManager();

	//if (nextWaveIndex != self->lastPausedAt &&
	//	!isFinalWave &&
	//	(nextWaveIndex + 1) % self->wavesBetweenChoosers == 0) {
	//	LOGI("Suppress wave manager");
	//	// SetSuppressedState(waveManager, true); // pause would be more appropriate here. Also set Board->plantsCanAttack to false
	//	self->lastPausedAt = nextWaveIndex;

	//	waveManager->SetPaused(true);
	//}

	if (waveType == wavetypehugewave) {
		++self->flagsCount;
		LOGI("flagsCount = %d", self->flagsCount);
		SexyString newName = "Survival: Endless - " + std::to_string(self->flagsCount) + " flag" + (self->flagsCount > 1 ? "s" : "") + " completed";
		LOGI("newName = %s", newName.c_str());
		GetBoard()->ChangeLevelName(newName);
	}
}

void SurvivalModule::RegisterCallbacks(SurvivalModule* self) {
	auto levelModuleManager = GetBoard()->levelModuleManager;
	levelModuleManager->unk0x40 = true; // I'm not sure what this does

	typedef void (*addDelegate)(Sexy::Event*, Sexy::DelegateBase*, int);
	addDelegate pAddDelegate = (addDelegate)getActualOffset(0x2A417C);

	// typedef void (*standardIntroRegister)(StandardLevelIntro*);
	// ((standardIntroRegister)getActualOffset(0x670AE0))(self);

	/*auto initialDelegate = CreateDelegate(
		0x1BB8A90,
		(uint)SurvivalTransitionFromLoading,
		false,
		(uint)self,
		true,
		getActualOffset(0xD527E0)
	);
	((addDelegate)getActualOffset(0x2A417C))(&levelModuleManager->transitionFromLoadingEvent, &initialDelegate, 0);*/

	auto introDelegate = CreateDelegate(
		0x1BB8A90,
		(uint)SurvivalIntro,
		false,
		(uint)self,
		true,
		getActualOffset(0xD527E0)
	);
	pAddDelegate(&levelModuleManager->introEvent, &introDelegate, 0);

	auto checkWaveDelegate = CreateEmptyDelegate(0x1BB8A90);
	checkWaveDelegate.callbackFunc = (uint)CheckCurrentWave;
	checkWaveDelegate.useOwnerVtable = false;
	checkWaveDelegate.callbackOwner = (uint) self;
	checkWaveDelegate.unk0x10 = true;
	checkWaveDelegate.executeCallbackFunction = getActualOffset(0xD527E0);

	pAddDelegate(&levelModuleManager->unkEvent12, &checkWaveDelegate, 0);

	auto startDelegate = CreateDelegate(
		0x1BB8A90,
		(uint)SurvivalStart,
		false,
		(uint)self,
		true,
		getActualOffset(0xD527E0)
	);
	pAddDelegate(&levelModuleManager->levelStartEvent, &startDelegate, 0);

	auto onViewBoardOrZombiesPressed = CreateDelegate(
		0x1BB8A90,
		(uint)SurvivalViewBoardOrZombies,
		false,
		(uint)self,
		true,
		getActualOffset(0xD527E0)
	);
	
	MessageRouter* router = MessageRouter::GetInstance();
	typedef void (*registerMsgDelegate)(MessageRouter*, uint, Sexy::DelegateBase*);
	((registerMsgDelegate)getActualOffset(0x5FDA50))(router, getActualOffset(0xF65AC4), &onViewBoardOrZombiesPressed);

	auto nextWaveDelegate = CreateDelegate(
		(uint)SurvivalModuleNextWave,
		false,
		(uint)self,
		true,
		getActualOffset(0xD527E0)
	);
	typedef void (*registerNextWaveMsg)(MessageRouter*, uint, uint, uint, int);
	((registerNextWaveMsg)getActualOffset(0x4C0AB4))(
		router, 
		Messages::Get(NextWaveProgressMsg), 
		(uint)self, 
		(uint)SurvivalModuleNextWave, 
		 0
	);

	// TODO: Add a TransitionFromLoading callback to change the level name if flagsCount > 0

	/*auto renderQueueDelegate = CreateEmptyDelegate(0x1BBF490);
	renderQueueDelegate.callbackFunc = (uint)SurvivalModuleRenderAnimMgr;
	renderQueueDelegate.useOwnerVtable = false;
	renderQueueDelegate.callbackOwner = (uint)self;
	renderQueueDelegate.unk0x10 = true;
	renderQueueDelegate.executeCallbackFunction = getActualOffset(0xD527FC);
	((addDelegate)getActualOffset(0x32C5B0))(&levelModuleManager->unkEvent17, &renderQueueDelegate, 0);*/
	
	// unkEvent11 should be hooked up to clearing the animationMgr
	/*auto clearAnimDelegate = CreateEmptyDelegate(0x1BB8A90);
	clearAnimDelegate.callbackFunc = (uint)ClearAnimMgr;
	clearAnimDelegate.useOwnerVtable = false;
	clearAnimDelegate.callbackOwner = (uint)self;
	clearAnimDelegate.unk0x10 = true;
	clearAnimDelegate.executeCallbackFunction = getActualOffset(0xD527E0);
	((addDelegate)getActualOffset(0x32C5B0))(&levelModuleManager->unkEvent11, &clearAnimDelegate, 0);*/

	typedef void (*addMsgHandler)(MessageRouter*, uint, SurvivalModule*, uint, int); // last is useOwnerVtable
	// zombie death msg
	// ((addMsgHandler)getActualOffset(0xD504E4))(router, getActualOffset(0x8A6FD8), self, (uint)SurvivalOnZombieDeath, 0);
	// zombie destroyed msg
	// ((addMsgHandler)getActualOffset(0xD507D0))(router, getActualOffset(0x8A6FD4), self, (uint)SurvivalOnZombieDeath, 0);
}

DEFINE_CALLBACK_FUNC(SurvivalModule, onViewZombiesComplete) {
	a1->ChangeState(pannedtozombies);
}

DEFINE_CALLBACK_FUNC(SurvivalModule, onResume) {
	a1->ChangeState(resumed);
}

void KillStreetZombies();

void EnableSeedPackets();

DEFINE_CALLBACK_FUNC(SurvivalModule, onResumeGame) {
	LOGI("Resume game");
	Board* board = GetBoard();

	KillStreetZombies();

	board->ChangeState(5);
	board->plantsCanAttack = true;
	board->SetPaused(false);

	auto waveManager = GetWaveManager();
	waveManager->SetPaused(false);

	EnableSeedPackets();

	a1->ChangeState(playing);
}

DEFINE_CALLBACK_FUNC(SurvivalModule, onViewBoardComplete) {
	LOGI("On view board complete");
	a1->ChangeState(pannedtoboard);
}

void SpawnStreetZombies();

AnimationMgr* GetAnimationMgr() {
	Board* board = GetBoard();
	RtWeakPtr<AnimationMgr> animationMgrPtr;
	animationMgrPtr.FromOther(&board->m_animationMgr);
	return animationMgrPtr.get();
}

DEFINE_CALLBACK_FUNC(SurvivalModule, onShowMoreApproachingComplete) {
	// let's set the music here
	/*a1->overrideMusic = true;
	if (a1->musicType == "MiniGame_A") {
		a1->musicType = "MiniGame_B";
	}
	else {
		a1->musicType = "MiniGame_A";
	}*/

	GetBoard()->ChangeState(3); // levelintro board state. THis will trigger a reset of StageModule's music
	
	typedef SeedBankNew* (*GetSeedBank)(Board*);
	auto seedBank = ((GetSeedBank)getActualOffset(0x7209CC))(GetBoard());
	seedBank->SetEnabled(false);

	// clear up boosts applied from the module so that the next seed chooser reflects boosts properly
	typedef SeedBankModule* (*getSeedBankModule)(Board*);
	auto module = ((getSeedBankModule)getActualOffset(0x720ADC))(GetBoard());

	module->m_boostsApplied.clear();
	module->m_hasAppliedCombatBoosts = false;

	CreateFreshSeedChooser(a1);

	// deselect shovel and plantfoods too
	typedef void (*DeselectOnCursor)(Board*);
	((DeselectOnCursor)getActualOffset(0x7267D8))(GetBoard());

	auto shovel = GetUIWidget("UIShovel");
	shovel->SetEnabled(false);
	auto powerupHolder = GetUIWidget("UIPowerupHolder");
	powerupHolder->SetEnabled(false);
	auto plantfood = GetUIWidget("UIPlantfood");
	plantfood->SetEnabled(false);
	auto fastForward = GetUIWidget("UIFastForwardButton");
	fastForward->SetEnabled(false);

	MessageRouter::GetInstance()->ExecuteMessage(Messages::Get(DisableProgressMeter));

	MessageRouter::GetInstance()->ExecuteMessage(Messages::Get(SurvivalLevelEnded));

	// spawn some street zombies
	SpawnStreetZombies();

	int startX, endX;
	GetStartXAndEndX(5, startX, endX);
	auto moveBoard = MakeMoveBoard(1.5, startX, endX, 0, 0, 4);

	float endTime;
	auto animMgr = GetAnimationMgr();
	animMgr->AddAnimController(moveBoard, animMgr->m_time + 1.0, endTime);

	SexyString event = "onViewZombiesComplete";
	auto timeEvent = MakeTimeEvent((RtWeakPtr<GameObject>*) & a1->m_thisPtr, &event);
	float tmp;
	animMgr->AddAnimController(timeEvent, endTime, tmp);
}

void SurvivalModule::buildEventCallbacks(Reflection::CRefManualSymbolBuilder* builder, void* rclass) {
	IF_CALLBACK_NOTSETUP(SurvivalModule) {
		DEFINE_CALLBACK(SurvivalModule, onViewZombiesComplete);
		DEFINE_CALLBACK(SurvivalModule, onResume);
		DEFINE_CALLBACK(SurvivalModule, onResumeGame);
		DEFINE_CALLBACK(SurvivalModule, onShowMoreApproachingComplete);
		DEFINE_CALLBACK(SurvivalModule, onViewBoardComplete);
	}

	REGISTER_EVENT_CALLBACK(builder, rclass, SurvivalModule, onViewZombiesComplete);
	REGISTER_EVENT_CALLBACK(builder, rclass, SurvivalModule, onResume);
	REGISTER_EVENT_CALLBACK(builder, rclass, SurvivalModule, onResumeGame);
	REGISTER_EVENT_CALLBACK(builder, rclass, SurvivalModule, onShowMoreApproachingComplete);
	REGISTER_EVENT_CALLBACK(builder, rclass, SurvivalModule, onViewBoardComplete);
}

void SpawnStreetZombies() {
	typedef void (*spawnStreet)(Board*);
	((spawnStreet)getActualOffset(0x37B320))(GetBoard());
}

void KillStreetZombies() {
	typedef void (*killStreetZombies)();
	((killStreetZombies)getActualOffset(0x728188))();
}

std::vector<UIWidget*> GetSeedPackets() {
	typedef SeedBankNew* (*GetSeedBank)(Board*);
	auto seedBank = ((GetSeedBank)getActualOffset(0x7209CC))(GetBoard());

	std::vector<UIWidget*> resVec;

	for (int i = 0; i < seedBank->m_packetCount; ++i) {
		// LOGI("i = %d", i);
		typedef void (*GetChild)(RtWeakPtr<UIWidget>*, UIWidget*, int*);
		RtWeakPtr<UIWidget> res;
		((GetChild)getActualOffset(0xEDAFAC))(&res, seedBank, &i);
		// LOGI("child got");

		UIWidget* childWidget = res.get();
		resVec.push_back(childWidget);
	}

	return resVec;
}

void EnableSeedPackets() {
	// LOGI("Seed bank got at %u", (uint)seedBank);
	auto packets = GetSeedPackets();
	for (auto packet: packets) {
		packet->m_flags |= 4;
	}
}

void DisableSeedPackets() {
	// LOGI("Disable seed packets called");
	auto packets = GetSeedPackets();
	for (auto packet : packets) {
		packet->m_flags &= ~4u;
	}

	typedef void (*DeselectPacketOnCursor)(Board*);
	((DeselectPacketOnCursor)getActualOffset(0x7267D8))(GetBoard());
	// LOGI("deselected cursor");
}

void SurvivalModule::ChangeState(SurvivalModuleState newState) {
	SurvivalModuleState oldState = this->m_state;
	this->m_state = newState;
	LOGI("Change state to %d", newState);
	switch (newState) {
	case levelintro:
		break;
	case transitionfromloading:
		break;
	case playing:
		break;
	case playingfinished: {
		// gotta disable a bunch of widgets
		if (oldState == newState) {
			break;
		}

		Board* board = GetBoard();
		auto animMgr = GetAnimationMgr();

		board->plantsCanAttack = false;
		board->SetPaused(true);
		// GetBoard()->ChangeState(3); // levelintro board state

		// what if I disable the seed packets here?
		DisableSeedPackets();

		// Show the advice text with time event onShowMoreApproachingComplete
		typedef AnimationController* (*makeShowAdvice)(std::u32string*, int);
		std::u32string advice = U"[ADVICE_MORE_ZOMBIES_APPROACHING]";
		auto adviceAnim = ((makeShowAdvice)getActualOffset(0x3ED7F8))(&advice, 1);

		typedef int (*getAudioMgr)();
		int audioMgr = ((getAudioMgr)getActualOffset(0x279750))();
		typedef int (*playSound)(int, SexyString*);
		SexyString audioName = "Play_ChiliBean_Fart";
		((playSound)getActualOffset(0x6BEA78))(audioMgr, &audioName);
	
		adviceAnim->m_duration = 4;
		adviceAnim->m_endTime = adviceAnim->m_duration + animMgr->m_time;

		float endTime;
		animMgr->AddAnimController(adviceAnim, animMgr->m_time, endTime);
		// LOGI("cur time = %f endTime = %f", animMgr->m_time, endTime);
		
		SexyString eventName = "onShowMoreApproachingComplete";
		auto timedEvent = MakeTimeEvent((RtWeakPtr<GameObject>*) & m_thisPtr, &eventName);
		
		float tmp;
		animMgr->AddAnimController(timedEvent, endTime, tmp);

		break;
	}
	// These timed events don't work because animMgr is paused, so these states are unused
	case panningtozombies: {
		//auto viewWidget = GetUIWidget("UIViewBoardOrZombies");
		//if (viewWidget) {
		//	viewWidget->m_flags &= -4u; // disable it
		//}

		/*int startX, endX;
		GetStartXAndEndX(5, startX, endX);
		auto moveBoard = MakeMoveBoard(1.5, startX, endX, 0, 0, 4);

		float endTime;
		auto animMgr = GetAnimationMgr();
		animMgr->AddAnimController(moveBoard, animMgr->m_time, endTime);*/

		/*SexyString event = "onViewZombiesComplete";
		auto timeEvent = MakeTimeEvent((RtWeakPtr<GameObject>*) & this->m_thisPtr, &event);
		float tmp;
		auto animMgr = GetAnimationMgr();
		animMgr->AddAnimController(timeEvent, animMgr->m_time + 1.2, tmp);*/

		break;
	}
	case pannedtozombies: {
		/*auto viewWidget = GetUIWidget("UIViewBoardOrZombies");
		if (viewWidget) {
			viewWidget->m_flags |= 4u;
		}*/

		// clear out the imitated flag on packets
		auto packets = GetSeedPackets();
		for (auto packet : packets) {
			SeedPacket* packetCasted = (SeedPacket*)packet;
			packetCasted->m_packetFlags &= ~PACKETFLAG_IMITATED;
		}

		BringUpSeedChooser(this);

		break;
	}
	case panningtoboard: {
		//auto viewWidget = GetUIWidget("UIViewBoardOrZombies");
		//if (viewWidget) {
		//	viewWidget->m_flags &= -4u; // disable it
		//}

		//int startX, endX;
		//GetStartXAndEndX(4, startX, endX);
		//auto moveBoard = MakeMoveBoard(2.5, startX, endX, 0, 0, 4);

		/*
		//float endTime;
		auto animMgr = GetAnimationMgr();
		//animMgr->AddAnimController(moveBoard, animMgr->m_time, endTime);

		SexyString event = "onViewBoardComplete";
		auto timeEvent = MakeTimeEvent( & this->m_thisPtr, & event);
		float tmp;
		// piggybacking off seed chooser's flow. It finishes the pan left in 1 second
		animMgr->AddAnimController(timeEvent, animMgr->m_time + 1.2, tmp);
		*/
		break;
	}
	case pannedtoboard: {
		/*auto viewWidget = GetUIWidget("UIViewBoardOrZombies");
		if (viewWidget) {
			viewWidget->m_flags |= 4u;
		}*/

		break;
	}
	case resumepanning: {
		auto animMgr = GetAnimationMgr();

		int startX, endX;
		GetStartXAndEndX(4, startX, endX);
		auto moveBoard = MakeMoveBoard(2.5, startX, endX, 0, 0, 4);

		float endTime;
		animMgr->AddAnimController(moveBoard, animMgr->m_time, endTime);

		SexyString event = "onResume";
		auto timeEvent = MakeTimeEvent((RtWeakPtr<GameObject>*) & this->m_thisPtr, &event);
		float tmp;
		animMgr->AddAnimController(timeEvent, endTime, tmp);

		break;
	}
	case resumed: {
		SexyString event = "onResumeGame";
		auto timeEvent = MakeTimeEvent((RtWeakPtr<GameObject>*) & this->m_thisPtr, &event);
		float tmp;
		auto animMgr = GetAnimationMgr();
		animMgr->AddAnimController(timeEvent, animMgr->m_time + 0.5, tmp);

		break;
	}
	}
}

SurvivalModuleProps* SurvivalModule::GetProps() {
	RtWeakPtr<SurvivalModuleProps> propsPtr;
	propsPtr.FromOther((RtWeakPtr<SurvivalModuleProps>*) & this->propSheetPtr);

	return propsPtr.get();
}

SurvivalModule* SurvivalModule::GetInstance() {
	return (SurvivalModule*) GetBoard()->levelModuleManager->GetModule(SurvivalModule::GetRTClass());
}

void* SurvivalModule::Destroy(SurvivalModule* self) {
	typedef void (*deregisterCallbacks)(MessageRouter*, int);
	((deregisterCallbacks)getActualOffset(0x5FDE64))(MessageRouter::GetInstance(), (uint)self);

	typedef void* (*levelModuleDtor)(LevelModule*);
	return ((levelModuleDtor)getActualOffset(LEVELMODULE_DTOR_ADDR))(self);
}

void SurvivalModule::ModInit() {
	LOGI("SurvivalModule init");

	// vftable = copyVFTable(getActualOffset(STANDARDLEVELINTRO_VTBL), 30);
	vftable = copyVFTable(getActualOffset(0x1C41DE4), 19);
	patchVFTable(vftable, (void*)SurvivalModule::GetRTClass, 0);
	// TODO: Make custom destructors as well to deregister callbacks
	patchVFTable(vftable, (void*)SurvivalModule::Destroy, 2);
	patchVFTable(vftable, (void*)SurvivalModule::DestroyAndDealloc, 3);
	patchVFTable(vftable, (void*)SurvivalModule::RegisterCallbacks, 7);
	patchVFTable(vftable, (void*)SurvivalSetup, 17);

	GetRTClass();
	LOGI("Finished initializing SurvivalModule");
}
