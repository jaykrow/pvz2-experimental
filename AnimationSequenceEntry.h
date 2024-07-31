#pragma once
class AnimationSequenceEntry
{
public:
	SexyString animLabel;
	int selectionMethod;
	float loopingDuration;
	
	AnimationSequenceEntry() : animLabel(""), selectionMethod(0), loopingDuration(0.0) {};
	AnimationSequenceEntry(SexyString animLabel, int selectionMethod, float loopingDuration) : animLabel(animLabel), selectionMethod(selectionMethod), loopingDuration(loopingDuration) {};
};

