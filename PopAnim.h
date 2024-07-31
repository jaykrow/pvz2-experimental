#pragma once

#include "PvZ2/RtWeakPtr.h"
#include "Color.h"

namespace Sexy {

	class BaseResource2 : public RtObject {

	};

	class Matrix3 {
	public:
		float mat[3][3];
	};

	class Transform2D : public Matrix3 {
	public:
		Transform2D() {
			memset(mat, 0, sizeof(mat));
			mat[0][0] = 1.0;
			mat[1][1] = 1.0;
			mat[2][2] = 1.0;
		}
	};

	class PopAnim : public BaseResource2 {
	public:
		int unk1;
		RtWeakPtr<void> pamRtId;
		RtWeakPtr<void> unkPtr2;
		int unk3;
		int unk4;
		int mDrawScale;
		int mImgScale;
		int mAnimRate;
		Rect mAnimRect;
		int mLastPlayedFrameLabel;
		char pad0040[0x8];
		int unkStruct;
		int mMainAnimDef;
		int mBlendTicksTotal;
		int mBlendTicksCur;
		int mBlendDelay;
		Transform2D mTransform;
		Color mColor;
		bool mAdditive;
		bool unk5;
		bool mAnimRunning;
		bool mPaused;
		bool mColorizeType;
	};
}

static_assert(sizeof(Sexy::PopAnim) == 0x98);
static_assert(offsetof(Sexy::PopAnim, pad0040) == 0x40);
static_assert(offsetof(Sexy::PopAnim, mTransform) == 0x5C);
