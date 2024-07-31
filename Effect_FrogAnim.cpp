#include "pch.h"
#include "Effect_FrogAnim.h"
#include "TimeMgr.h"
#include "ZombieStateHelper.h"

#define ZOMBIE_EVENT_CONSTRUCT_ADDR 0x3A56A0

static bool frogDelegatesSetup;
static Sexy::DelegateBase frogLoopDelegate;

DEFINE_STATIC_RT_CLASS_MEMBERS(Effect_FrogAnim);

void FrogAnimOnLoop(Effect_FrogAnim* effect) {
	// LOGI("Frog anim looped %u", (uint)effect);
	effect->animLooped = true;
}

void FrogAnimOnStopOrLoop(Effect_FrogAnim* r0_0) {
	r0_0->animFinished = true;

	int animSeqSize = r0_0->animSequence.size();
	if (animSeqSize != 4) {
		typedef void (*popAnimOnStopOrLoop)(Effect_PopAnim*);
		((popAnimOnStopOrLoop)getActualOffset(0xE47970))(r0_0);
		return;
	}

	// handle special frog logic
	int currentIndex = r0_0->animSequenceCurrentIndex;
	++currentIndex;
	if (currentIndex >= animSeqSize) {
		currentIndex = 1; // reset to idle state
		// r0_0->resetToIdle = true;
	}
	if (currentIndex == 3) {
		r0_0->relocatedPlant = true;
	}
	r0_0->animSequenceCurrentIndex = currentIndex;
	r0_0->m_unPausedInState = false; // changed to new state

	auto animEntry = &r0_0->animSequence[currentIndex];

	RtWeakPtr<PopAnimRig> animRigPtr = RtWeakPtr<PopAnimRig>();
	animRigPtr.FromOther(&r0_0->animRig);
	PopAnimRig* animRig = animRigPtr.get();
	RtWeakPtr<Effect_FrogAnim> thisPtr;
	thisPtr.FromOther((RtWeakPtr<Effect_FrogAnim>*) & r0_0->m_thisPtr);

	if (animEntry->loopingDuration <= 0.0) {
		SexyString eventName = "onAnimStopped";
		typedef ZombieEvent* (*zombieEventConstruct)(ZombieEvent*, RtWeakPtr<Effect_FrogAnim>*, SexyString*);

		ZombieEvent onAnimStopped;
		((zombieEventConstruct)getActualOffset(ZOMBIE_EVENT_CONSTRUCT_ADDR))(&onAnimStopped, &thisPtr, &eventName);

		typedef int (*playAnimWithCallback)(PopAnimRig*, SexyString&, int, ZombieEvent&);
		((playAnimWithCallback)getActualOffset(0x667AA8))(animRig, animEntry->animLabel, animEntry->selectionMethod, onAnimStopped);
	}
	else {
		typedef int (*loopAnimWithCallback)(PopAnimRig*, SexyString&, int, ZombieEvent&);

		SexyString eventName = "onIdleLooped";
		typedef ZombieEvent* (*zombieEventConstruct)(ZombieEvent*, RtWeakPtr<Effect_FrogAnim>*, SexyString*);

		ZombieEvent onIdleLooped;
		((zombieEventConstruct)getActualOffset(ZOMBIE_EVENT_CONSTRUCT_ADDR))(&onIdleLooped, &thisPtr, &eventName);

		((loopAnimWithCallback)getActualOffset(0x667C88))(animRig, animEntry->animLabel, animEntry->selectionMethod, onIdleLooped);
	}

	r0_0->animSequenceCurrentIndexStartTime = TimeMgr::GetInstance()->m_curTime;
}

void Effect_FrogAnim::buildEventCallbacks(Reflection::CRefManualSymbolBuilder* builder, void* rClass) {
	if (!frogDelegatesSetup) {
		SetupLiteralDelegate(&frogLoopDelegate, (uint)FrogAnimOnLoop);
		frogDelegatesSetup = true;
	}

	RegisterEventCallback(builder, rClass, "onIdleLooped", frogLoopDelegate);

	LOGI("Event callbacks built");
}

void Effect_FrogAnim::ModInit() {
	LOGI("Initializing Effect_FrogAnim");
	frogDelegatesSetup = false;

	vftable = copyVFTable(getActualOffset(0x1C52168), 24); // make space for our last function
	patchVFTable(vftable, (void*)Effect_FrogAnim::GetRTClass, 0);
	patchVFTable(vftable, (void*)FrogAnimOnStopOrLoop, 0x17);

	GetRTClass();
	LOGI("Finished initializing Effect_FrogAnim");
}

bool Effect_FrogAnim::isJumping() {
	return animSequenceCurrentIndex == 2 || animSequenceCurrentIndex == 3;
}
