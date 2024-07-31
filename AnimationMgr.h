#pragma once

#include "GameObject.h"
#include "ZombieEvent.h"

class AnimationController : public GameObject {
public:
	float m_startTime;
	float m_endTime;
	float m_duration;
	bool deleteOnEnd;
	SexyString m_description;
};

static_assert(sizeof(AnimationController) == 0x2C);

class MoveBoard : public AnimationController {
public:
	int m_startXPos;
	int m_endXPos;
	int m_startYPos;
	int m_endYPos;
	int m_curveType;
};

class TimeEvent : public AnimationController {
public:
	int unk;
	ZombieEvent delegate;

	DEFINE_RT_CLASS_GET_CLASS_WRAPPER(0x1031C98);
};

static_assert(sizeof(MoveBoard) == 0x40);

class AnimationMgr : public GameObject
{
public:
	float m_time;
	bool m_paused;
	std::vector<RtWeakPtr<AnimationController>> m_animControllers;
	std::vector<RtWeakPtr<AnimationController>> m_animControllersToAdd;

	DEFINE_RT_CLASS_GET_CLASS_WRAPPER(0x10320D8);

	void AddAnimController(AnimationController* animController, float startTime, float& endTime) {
		typedef void (*addAnimController)(AnimationMgr*, AnimationController*, float, float*);
		addAnimController pAddAnimController = (addAnimController)getActualOffset(0x10328C0);
		
		return pAddAnimController(this, animController, startTime, &endTime);
	}

	virtual void Func13(int a2) { };
};

TimeEvent* MakeTimeEvent(RtWeakPtr<GameObject>* owner, SexyString* eventName);

MoveBoard* MakeMoveBoard(float duration, int startX, int endX, int startY, int endY, int curveType);

void GetStartXAndEndX(int type, int& startX, int& endX);

