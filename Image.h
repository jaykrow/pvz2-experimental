#pragma once

#include "RtObject.h"

class BaseResource : public Sexy::RtObject {
public:
	int unk;
	RtWeakPtr<void> unkPtr;
	RtWeakPtr<void> unkPtr2;
};

class Image : public BaseResource
{
public:
	char pad018[12];
	SexyString mFilePath;
	int mWidth;
	int mHeight;
	int mNumRows;
	int mNumCols;
	RtWeakPtr<void> m_atlastImage;
	int m_atlastStartX;
	int m_atlastStartY;
	int m_atlastEndX;
	int m_atlastEndY;
};

static_assert(sizeof(Image) == 0x58);
static_assert(offsetof(Image, mFilePath) == 0x24);
static_assert(offsetof(Image, m_atlastStartY) == 0x4C);

