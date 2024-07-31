#pragma once

#include "LevelModule.h"
#include "StandaloneEffect.h"

class IntroModule : public LevelModule
{
public:
	Sexy::DelegateBase onIntroComplete;

	virtual void Function13() { }
	virtual void Function14() { }
	virtual void Function15() { }
	virtual void Function16() { }
	virtual void Function17() { }
	virtual void Function18() { }
	virtual void ShowReadySetPlantText() { }
	virtual void OnShowPlantText() { }
};

#define STANDARDLEVELINTRO_VTBL 0x1BDAC58
#define STANDARDLEVELINTRO_GETCLASS 0x66FAB8

class StandardLevelIntro : public IntroModule {
public:
	int unkNum;
	int unk;
	RtWeakPtr<StandaloneEffect> unkEffectPtr;
	int unk2;
	int unk3; // this is leaked over for child classes

	virtual void Function21() { }
	virtual bool IsNotStandard() { return false; }
	virtual void BringUpSeedChooser() { }
	virtual void Function24() { }; // send message nullsub_614
	virtual void OnChoosersDismissed() { }
	virtual void OnStandardIntroComplete() { }
	virtual void Function27() { }
	virtual void Function28() { }
	virtual void OnTransitionFromLoading() { }
};

