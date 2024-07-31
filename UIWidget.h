#pragma once

#include "GameObject.h"

class UIWidget : public GameObject
{
public:
	char unk[20];
	SexyString m_propertySheetName;
	RtWeakPtr<void> m_proprtySheetPtr;
	Rect m_boundingRect;
	int m_overrideSelfAnchor;
	int m_overrideParentAnchor;
	RtWeakPtr<UIWidget> parent;
	char childList[12]; // unk type
	int m_flags;
	float m_stateEnterTime;
	SexyVector2 m_overridePositionOffset;
	SexyVector2 m_scaledPositionOffset;
	bool m_positionOffsetOverriden;
	bool m_overrideInputPriority;
	bool m_inputPriorityOverridden;
	std::vector<SexyString> loadingResourcesList;
	int unk2;
	RtWeakPtr<void> m_atlastImage;
	Rect m_atlastSrcRect;
	int unkInt;
	Sexy::DelegateBase delegateWidgetState;
	Sexy::DelegateBase delegate;
	Sexy::DelegateBase delegatWidgetState2;
	char pad0F4[0x18];

	void SetEnabled(bool enabled);
};

static_assert(sizeof(UIWidget) == 0x10C);
static_assert(offsetof(UIWidget, m_boundingRect) == 0x38);
static_assert(offsetof(UIWidget, m_flags) == 0x64);
static_assert(offsetof(UIWidget, loadingResourcesList) == 0x80);

class SlidingWidget : public UIWidget {
public:
	float m_slideTimeStart;
	float m_slideTimeDuration;
	int m_slidingState;
	int m_slideInCurve;
	int m_slideOutCurve;
	SexyVector2 m_originalPosition;
	SexyVector2 m_offscreenPosition;
	SexyString m_audioOnSlideIn;
	SexyString m_audioOnSLideOut;
	bool m_slideEnabled;
	bool m_slideSetup;
	float m_normalizedProgress;
};

static_assert(sizeof(SlidingWidget) == 0x150);
static_assert(offsetof(SlidingWidget, m_normalizedProgress) == 0x14C);

UIWidget* GetUIWidget(SexyString name);
