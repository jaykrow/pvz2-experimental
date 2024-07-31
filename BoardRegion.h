#pragma once

#include "GameObject.h"
#include "BoardEntity.h"

#define BOARDREGION_CONSTRUCT_ADDR 0x104FCE0
#define BOARDREGION_SETFLAG_ADDR 0x104FD24
#define BOARDREGION_VFTABLE_ADDR 0x1C64648
#define BOARDREGION_GETCLASS_ADDR 0x104F7F0

class BoardRegionLandedEntity {
public:
	SexyVector3 coords;
	BoardEntity* entity;
};

enum BoardRegionFlags {
	region_deepwater = 1,
	region_tidewater = 2,
	region_sky = 4
};

class BoardRegion : public GameObject
{
public:
	int m_flags;
	FRect m_region;
	int unk;

	void SetRegionFlag(int flag) {
		typedef int (*sub104FD24)(BoardRegion*, int);
		((sub104FD24)getActualOffset(BOARDREGION_SETFLAG_ADDR))(this, flag);
	}

	virtual void Function13() { };
	virtual void OnParticleImpact() { };
	virtual void CanPlantGoHere(int* gridCoords, int plantType, std::vector<int>* reasonsVec) { };
	virtual float GetSinkDistance(float*) { };
	virtual void RunsEveryFrame() { };
};

