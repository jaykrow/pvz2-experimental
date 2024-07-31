#pragma once

#include "LevelModule.h"
#include "Image.h"
#include "PvZ2/Board.h"
#include "StageModuleProperties.h"

#define STAGEMODULE_CONSTRUCT_ADDR 0x4BFF5C
#define STAGEMODULE_GETCLASS_ADDR 0x4BF560
#define STAGEMODULE_VTBL 0x1BCD938
#define STAGEMODULE_VFUNCS 28

enum MusicState {
	musicunk = 0,
	musicinitial = 1,
	musicgamestart = 2,
	musichugewave = 3,
	musichugewave2 = 4,
	musicfinalwave = 5,
	musiclosegame = 6,
	musicwingame = 7
};

class StageModule : public LevelModule
{
public:
	bool m_sendAlternateHugeWaveEvent;
	SexyString m_musicTriggerOverride;
	char pad028[0xC];
	bool m_shouldStopMusic;
	bool m_shouldStartMusic;
	bool m_startMusicOnLastStandStart;
	MusicState m_musicState;
	bool m_suppressMusicStateChangeEvents;
	int m_lastWaveIntroMusic;
	int unk;
	RtWeakPtr<Image> m_backImage;
	RtWeakPtr<Image> m_backImageRight;
	RtWeakPtr<Image> m_backImageLeft;
	char pad0060[0x30];

	static StageModule* GetInstance(Board*);

	StageModuleProperties* GetProps();
};

static_assert(sizeof(StageModule) == 0x90);
static_assert(offsetof(StageModule, m_musicTriggerOverride) == 0x1C);
static_assert(offsetof(StageModule, m_suppressMusicStateChangeEvents) == 0x3C);
static_assert(offsetof(StageModule, m_backImage) == 0x48);
static_assert(offsetof(StageModule, pad0060) == 0x60);
