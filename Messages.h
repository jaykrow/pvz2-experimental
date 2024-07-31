#pragma once
#include "memUtils.h"

void DisableProgressMeterMsg();

void SurvivalLevelEndedMsg();

enum MessageId {
	PlantBoosted = 0xCF89B0,
	PressViewBoardOrZombies = 0xF65AC4,
	WaveManagerUpdateToPercent = 0x2ACD08,
	SetFlagCount = 0x56730C,
	DisableProgressMeter = -1,
	SurvivalLevelEnded = -2,
	NextWaveProgressMsg = 0x5672FC,
	LevelStartMsg = 0x2ACC78
};

class Messages {
public:
	static uint Get(MessageId id) {
		if ((uint)id < 0) {
			switch (id) {
			case DisableProgressMeter:
				return (uint)DisableProgressMeterMsg;
			case SurvivalLevelEnded:
				return (uint)SurvivalLevelEndedMsg;
			}
		}
		return GetActualOffset((uint) id);
	}
};
