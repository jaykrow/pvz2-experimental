#include "pch.h"
#include "Effect_PopAnim.h"

void PlaySuccessiveAnims(Effect_PopAnim* popAnim, std::vector<SexyString> &anims) {
	popAnim->animSequence.clear();
	popAnim->animSequenceCurrentIndex = -1;
	popAnim->animSequenceCurrentIndexStartTime = MAXFLOAT;
	typedef void (*addAnimEntry)(std::vector<AnimationSequenceEntry>*, SexyString*, float);
	addAnimEntry addAnimEntryFun = (addAnimEntry)getActualOffset(0xE45AF4);

	for (SexyString& anim : anims) {
		addAnimEntryFun(&popAnim->animSequence, &anim, 0.0);
	}
	
	typedef void (*onCurrentAnimStop)(Effect_PopAnim*);
	((onCurrentAnimStop)getActualOffset(0xE47970))(popAnim);
}

bool Effect_PopAnim::IsPaused() {
	RtWeakPtr<PopAnimRig> rig = RtWeakPtr<PopAnimRig>();
	rig.FromOther(&animRig);
	PopAnimRig* localRig = rig.get();

	if (localRig->popAnim->mPaused) {
		return true;
	}
	else {
		return false;
	}
}

void Effect_PopAnim::SetPaused(bool paused) {
	RtWeakPtr<PopAnimRig> rig = RtWeakPtr<PopAnimRig>();
	rig.FromOther(&animRig);
	PopAnimRig* localRig = rig.get();

	localRig->popAnim->mPaused = paused;
}
