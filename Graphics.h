#pragma once
#include "Color.h"

#define UNK_GRAPHICSFUNC 0x1124614
#define UNK_GRAPHICSFUNC2 0x1124784

class Graphics
{
public:
	int unk1;
	int unk2;
	float mTransX;
	float mTransY;
	float mScaleX;
	float mScaleY;
	float mScaleOrigX;
	float mScaleOrigY;
	Rect mClipRect;
	Color mColor;
	int mFont; // Sexy::Font type
	int mDrawMode;
	bool mColorizeImages;
	bool mFastStretch;
	bool mWriteColoredString;
	bool mLinearBlend;
	bool mIs3D;
	int mRenderDevice; // AndroidRenderDevice*
	int mRenderContext;
	int mGraphics3D;
	// there are still at least 4 more unknown fields

	void UnkFunc() {
		typedef void (*unkFunc)(Graphics*);
		((unkFunc)getActualOffset(UNK_GRAPHICSFUNC))(this);
	}

	void UnkFunc2() {
		typedef void (*unkFunc)(Graphics*);
		((unkFunc)getActualOffset(UNK_GRAPHICSFUNC2))(this);
	}
};


static_assert(offsetof(Graphics, mRenderDevice) == 0x50);
static_assert(offsetof(Graphics, mIs3D) == 0x4C);
