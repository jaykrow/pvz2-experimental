#pragma once

#include "StandaloneEffect.h"
#include "AnimationSequenceEntry.h"
#include "PopAnimRig.h"

class Effect_PopAnim : public StandaloneEffect
{
public:
	RtWeakPtr<PopAnimRig> animRig;
	std::vector<AnimationSequenceEntry> animSequence;
	int animSequenceCurrentIndex;
	float animSequenceCurrentIndexStartTime;
	float xFocusFraction;
	float yFocusFraction;
	float xScale;
	float yScale;
	float orientation;
	SexyVector2 translation;
	char unk4[40];

	DEFINE_RT_CLASS_GET_CLASS_WRAPPER(0xE45E40)

	int SetPositionAndLayer(SexyVector3 *a2, int layer) {
		typedef int (*setPositionAndLayer)(Effect_PopAnim*, SexyVector3*, int);
		return ((setPositionAndLayer)getActualOffset(0x7401A8))(this, a2, layer);
	}

	int SetScale(float xScale, float yScale) {
		typedef int (*setScale)(Effect_PopAnim*, float, float);
		return ((setScale)getActualOffset(0xE47644))(this, xScale, yScale);
	}

	void SetFromResource(int resource, int a2) {
		typedef void (*setFromResource)(Effect_PopAnim*, int, int);
		((setFromResource)getActualOffset(0xE47544))(this, resource, a2);
	}

	bool IsPaused();
	void SetPaused(bool paused);
};

/*
void GetPopAnimResource(RtWeakPtr<int>* a1, SexyString* resourceName) {
	typedef void (*getResource)(RtWeakPtr<int>*, SexyString*);
	((getResource)getActualOffset(0xC90A14))(a1, resourceName);
}
*/

void PlaySuccessiveAnims(Effect_PopAnim* popAnim, std::vector<SexyString> &anims);