#pragma once

#include "GameObject.h"

class GameSubsystem : public GameObject
{
};

class Chickening {
public:
	int row;
	int unk;
	float nextChickenTime;
	float endTime;
	int team;
	SexyString chickenTypeName;
	float duration;
	float durationVariance;
	float interval;
	float intervalVariance;
	int condition;

	Chickening() : chickenTypeName("brah") { };
};

class ZombieChickenSubsystem : public GameSubsystem {
public:
	std::vector<Chickening> m_chickenings;
	int unk;
};

