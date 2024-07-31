#pragma once
#include <cstdint>
#include <string>
#include "RtObject.h"

typedef std::string SexyString;

typedef uint8_t byte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

struct SexyVector3
{
	float mX, mY, mZ;

	SexyVector3() : mX(0), mY(0), mZ(0) {};

	SexyVector3(float x, float y, float z)
	{
		mX = x;
		mY = y;
		mZ = z;
	}
};

struct SexyVector2 {
	float mX, mY;
	SexyVector2() : mX(0), mY(0) {};
	SexyVector2(float x, float y) : mX(x), mY(y) {};
};

struct Rect
{
	int mX, mY, mWidth, mHeight;

	Rect() {
		this->mX = 0;
		this->mY = 0;
		this->mWidth = 0;
		this->mHeight = 0;
	}

	Rect(int mX, int mY, int mWidth, int mHeight) {
		this->mX = mX;
		this->mY = mY;
		this->mWidth = mWidth;
		this->mHeight = mHeight;
	}
};

struct FRect {
	float mX, mY, mWidth, mHeight;

	FRect() {
		this->mX = 0;
		this->mY = 0;
		this->mWidth = 0;
		this->mHeight = 0;
	}

	FRect(float mX, float mY, float mWidth, float mHeight) {
		this->mX = mX;
		this->mY = mY;
		this->mWidth = mWidth;
		this->mHeight = mHeight;
	}
};

struct FPoint {
	float x;
	float y;

	FPoint() {};

	FPoint(float x, float y) : x(x), y(y) {};
};

struct CornerRect {
	FPoint topLeft;
	FPoint topRight;
	FPoint bottomLeft;
	FPoint bottomRight;

	CornerRect() {}

	CornerRect(FPoint &&topLeft, FPoint &&topRight, FPoint &&bottomLeft, FPoint &&bottomRight) : 
		topLeft(topLeft), topRight(topRight), bottomLeft(bottomLeft), bottomRight(bottomRight) {};
};

struct ValueRange
{
	float Min;
	float Max;

	ValueRange() : Min(0), Max(0) {};

	ValueRange(float Min, float Max) : Min(Min), Max(Max) {};
};

struct Point {
	int x;
	int y;

	Point(int x, int y) : x(x), y(y) {};
};

struct WeightedOptionStubForRt : public Sexy::RtObject {
};

struct StringWeightedOption {
	SexyString Value;
	float Weight;

	StringWeightedOption(const SexyString &Value, float Weight) : Value(Value), Weight(Weight) {};

	// off_1C0BDE0 is vftbl
	virtual void* Destroy() {
		Value.~SexyString();
	}

	virtual void DestroyAndDealloc() {
		void* destroyed = Destroy();
		operator delete(destroyed);
	}

	virtual float GetWeight() {
		return Weight;
	}
};

static_assert(sizeof(StringWeightedOption) == 0x14);

struct WeightedVector {
	std::vector<std::pair<int, int>> WeightedVec;
	int TotalWeight = 0;
	int Seed = 0; // Not Seed, it's a pointer. Probably points to a MTRand instance?
	uint8_t unk = 0;

	void Add(int val, int weight) { // refer to sub_3AEFA0 for implementation. That one checks for val duplicates and replaces if necessary
		for (auto& pair : WeightedVec) {
			if (pair.first == val) {
				TotalWeight -= pair.second;
				pair.second = weight;
				TotalWeight += pair.second;
				return;
			}
		}

		WeightedVec.push_back(std::make_pair(val, weight));
		TotalWeight += weight;
	}
};

//struct Color {
//	int mRed;
//	int mGreen;
//	int mBlue;
//	int mAlpha;
//};