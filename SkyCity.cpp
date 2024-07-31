#include "pch.h"
#include "SkyCity.h"
#include "Zombie.h"
#include "BoardRegionSky.h"
#include "memUtils.h"
#include "SkyModule.h"
#include "SkyModuleProps.h"
#include "PvZ2/Board.h"
#include "ZombieParticle.h"

typedef bool (*zombieHasShadow)(Zombie*);
zombieHasShadow oZombieHasShadow = NULL;

bool hkZombieHasShadow(Zombie* a1) {
	return oZombieHasShadow(a1) && !SkyExistsAtPosition(&a1->m_position);
}

typedef bool (*plantHasShadow)(Plant*);
plantHasShadow oPlantHasShadow = NULL;

bool hkPlantHasShadow(Plant* a1) {
	return oPlantHasShadow(a1) && !SkyExistsAtPosition(&a1->m_position);
}

typedef bool (*projHasShadow)(Projectile*);
projHasShadow oProjHasShadow = NULL;

bool hkProjHasShadow(Projectile* a1) {
	return oProjHasShadow(a1) && !SkyExistsAtPosition(&a1->m_position);
}

typedef bool (*boardEntityHasShadow)(BoardEntity*);
boardEntityHasShadow oBoardEntityHasShadow = NULL;

bool hkBoardEntityHasShadow(BoardEntity* a1) {
	return oBoardEntityHasShadow(a1) && !SkyExistsAtPosition(&a1->m_position);
}

typedef void (*zombieParticleRun)(ZombieParticle*);
zombieParticleRun oZombieParticleRun = NULL;

void hkZombieParticleRun(ZombieParticle* a1) {
	if (SkyExistsAtPosition(&a1->m_position) && a1->m_settled) {
		// Sky region will set settled for us on particle landing
		a1->m_scale *= 0.98;
		a1->m_currentRotation += 0.08;
		a1->m_position.mX += 1;
		a1->m_position.mZ -= 0.5;
	}
	// LOGI("Rotation = %f currentRot = %f", a1->m_rotation, a1->m_currentRotation);
	oZombieParticleRun(a1);
	// LOGI("Rotation after = %f currentRot = %f", a1->m_rotation, a1->m_currentRotation);
}

void initSkyCityHooks() {
	PVZ2HookFunction(0x8BFDC8, (void*)hkZombieHasShadow, (void**)&oZombieHasShadow, "zombie has shadow");
	PVZ2HookFunction(0xE84E30, (void*)hkPlantHasShadow, (void**)&oPlantHasShadow, "plant has shadow");
	PVZ2HookFunction(0x915354, (void*)hkProjHasShadow, (void**)&oProjHasShadow, "projectile has shadow");
	PVZ2HookFunction(0x2D9474, (void*)hkBoardEntityHasShadow, (void**)&oBoardEntityHasShadow, "board entity has shadow");
	PVZ2HookFunction(0x537154, (void*)hkZombieParticleRun, (void**)&oZombieParticleRun, "zombie particle run");

	BoardRegionSky::ModInit();
	SkyModule::ModInit();
	SkyModuleProps::ModInit();
}
