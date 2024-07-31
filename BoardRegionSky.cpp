#include "pch.h"
#include "BoardRegionSky.h"
#include "PvZ2/Board.h"
#include "ZombieParticle.h"

DEFINE_STATIC_RT_CLASS_MEMBERS(BoardRegionSky);

void SkyEntityLandedCallback(BoardRegionSky* self, BoardRegionLandedEntity *entity) {
	LOGI("Landed callback");
	BoardEntity* landedEntity = entity->entity;

	LOGI("Vftable = %u", getOriginalOffset(*(uint*)landedEntity));
	LOGI("Real = %u", *(uint*)landedEntity); 

	if (*(uint*)landedEntity == getActualOffset(ZOMBIEPARTICLE_VTABLE_ADDR)) { // TODO: Fix this to use a isType call
		LOGI("It's a particle");
		ZombieParticle* particle = (ZombieParticle*)landedEntity;
		particle->m_settled = true;
		particle->m_position.mY -= GRID_TILE_HEIGHT; // pushes it to a lower render layer
		particle->m_position.mZ -= GRID_TILE_HEIGHT;

		// TODO: START FADE TIMER TO MAKE SURE THE GUYS ARE KILLED
	}
}

void BoardRegionSky::ModInit() {
	LOGI("Initializing BoardRegionSky");

	vftable = copyVFTable(getActualOffset(BOARDREGION_VFTABLE_ADDR), 18);

	patchVFTable(vftable, (void*)BoardRegionSky::GetRTClass, 0); // We aren't currently using this class
	patchVFTable(vftable, (void*)SkyEntityLandedCallback, 14);
	patchVFTable(vftable, (void*)getActualOffset(0x4AD038), 15); // Tide's CanPlantGoHere logic

	GetRTClass();
	LOGI("Finished initializing BoardRegionSky");
}

bool SkyExistsAtPosition(SexyVector3* position) {
	typedef BoardRegion* (*findRegionWithFlags)(Board*, SexyVector3*, int);
	return ((findRegionWithFlags)getActualOffset(0x72A6D0))(getBoard(), position, region_sky) != NULL;
}
