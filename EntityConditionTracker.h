#pragma once

#include <vector>
#include "Color.h"

struct EntityCondition
{
	float m_startTime;
	float m_endTime;
	float m_eventTime;
	float m_additionalDataValue;
};

//template <typename T, typename CT>
class EntityConditionTracker
{
public:
	std::vector<uint8_t> m_conditionFlags;
	std::vector<EntityCondition> m_conditions;

public:
	virtual void Function0() {};
	virtual void Function1() {};
	virtual void Function2() {};
	virtual void Function3() {};
};

struct UnkConditionStruct
{
    bool b1;
    bool b2;
    bool b3;
    bool flatColorMode;
    bool colorMixMode;
    bool b6;
};

class ZombieConditionTracker : public EntityConditionTracker
{
public:
    int m_pad[5];
    UnkConditionStruct* m_struct;
    int m_pad2[2];
    Color m_color1;
    Color m_color2;
};