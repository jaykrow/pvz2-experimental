#pragma once
#include "GameObject.h"
#include "Color.h"
#include "ZombieEvent.h"
#include "PopAnim.h"
#include "Graphics.h"

#define POPANIMRIG_DTOR_ADDR 0x665FE0

#define TRANSANDRENDER_ADDR 0x668EE8
#define RENDER_ADDR 0x6686B0

class PopAnimRig : public GameObject
{
public:
	int unk;
	float m_saturation;
	Sexy::PopAnim* popAnim;
	int unk6;
	RtWeakPtr<PopAnimRig> selfPtr;
	int activeAnim;
	SexyString activeAnimBaseLabel;
	int activeAnimPlayStyle;
	int activeAnimSeqEndCount;
	std::vector<int> m_animRandomDistribution;
	int activeAnimSelectMethod;
	int activeAnimLastPlayedVariation;
	int unk7;
	RtWeakPtr<void> audioObject;
	bool manageDeletion;
	bool mirrorX;
	bool disabled;
	bool useShaderOverrideColor;
	int unk3;
	Color shaderOverrideColor;
	Color unk4;
	Sexy::DelegateBase unkDelegateOnStop;
	Sexy::DelegateBase unkDelegateOnLoop;
	Sexy::DelegateBase unkDelegateOnCommand;
	ZombieEvent serialOnAnimStopped;
	ZombieEvent serialOnLoopingAnimContinued;
	ZombieEvent serialOnPopAnimCommand;
	bool registeredInAudioMgr;
	char unk5[11];
	int m_state;

	void TransformAndRender(Graphics* graphics, Sexy::Transform2D& trans) {
		typedef void (*func)(PopAnimRig*, Graphics*, Sexy::Transform2D*);
		((func)getActualOffset(TRANSANDRENDER_ADDR))(this, graphics, & trans);
	}

	void Render(Graphics* graphics) {
		typedef void (*func)(PopAnimRig*, Graphics*);
		((func)getActualOffset(RENDER_ADDR))(this, graphics);
	}

	Color GetColor() {
		return this->popAnim->mColor;
	}

	void SetColor(Color& color) {
		this->popAnim->mColor = color;
	}

	virtual int SetActiveAnim(SexyString* animName, int a3, uint unk_stateNum, int a5) {}
	virtual int SetAnimModifier(float multiplier) {}
	virtual int Function16() {}
	virtual int Function17() {}
	virtual int Function18() {}
	virtual int Function19() {}
	virtual int Function20() {}
	virtual int Function21() {}
	virtual int Function22() {}
	virtual int Function23() {}
	virtual int Function24() {}
	virtual int Function25() {}
	virtual int Function26() {}
	virtual int Function27() {}
	virtual int Function28() {}
	virtual int DamageFlash() {}
	virtual int GetUserShaderOverrideColor() {}
	virtual int Function31() {}

};

/*
static_assert(offsetof(PopAnimRig, unkInt) == 0x18);
static_assert(offsetof(PopAnimRig, selfPtr) == 0x20);
static_assert(offsetof(PopAnimRig, audioObject) == 0x58);
static_assert(offsetof(PopAnimRig, registeredInAudioMgr) == 0x160);
static_assert(offsetof(PopAnimRig, unk5) == 0x161);
*/
