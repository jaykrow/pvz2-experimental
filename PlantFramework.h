#pragma once
#include "RtObject.h"
#include "Plant.h"
#include "PopAnimRig.h"
#include "Zombie.h"

class PlantSpikeweed : public PlantFramework {
public:
	std::vector<RtWeakPtr<PopAnimRig>> m_plantfoodSpikes;
	bool m_plantfoodSpikesActive;
	std::vector<RtWeakPtr<Zombie>> m_zombiesBeingPulled;
};

class PlantSpikerock : public PlantSpikeweed {
public:
	int m_remainingSmashes;
};

static_assert(offsetof(PlantSpikerock, m_remainingSmashes) == 0x24);