#pragma once

#include "GameObject.h"
#include "SexyTypes.h"
#include "ZombieEvent.h"

class StandaloneEffect : public GameObject
{
public:
	int standaloneEffectFlags;
	SexyVector2 screenSpaceOrigin;
	int renderLayerOverride;
	int renderLayer;
	int unk;
	RtWeakPtr<int> attachmentTarget;
	SexyVector2 attachmentScreenSpaceOffset;
	int attachmentRenderLayerOffset;
	int unk2;
	ZombieEvent onCompletion;
	bool shouldDraw;
	char unk3[23];
};

