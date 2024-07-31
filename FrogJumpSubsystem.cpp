#include "pch.h"
#include "FrogJumpSubsystem.h"
#include "Effect_FrogAnim.h"
#include "TimeMgr.h"
#include "PvZ2/Board.h"
#include "assert.h"
#include "PlantConditions.h"

DEFINE_STATIC_RT_CLASS_MEMBERS(FrogJumpSubsystem)
bool FrogJumpSubsystem::checkIgnoreFrogs = false;

void RunFrogSubsystem(FrogJumpSubsystem* subsystem) {
	// LOGI("Running frog jump subsystem frogs size = %d", subsystem->frogs.size());
	for (Frog &frog : subsystem->m_frogs) {
		if (!frog.effect.is_valid()) {
			continue;
		}

		RtWeakPtr<Effect_FrogAnim> effectPtr = RtWeakPtr<Effect_FrogAnim>();
		effectPtr.FromOther(&frog.effect);
		Effect_FrogAnim* effect = effectPtr.get();

		if (!frog.removed) {
			if (effect->animSequenceCurrentIndex == 1) {
				if (effect->animFinished) {
					if (!frog.firstIdle) {
						frog.coords.mX += GRID_TILE_WIDTH;
					}
					frog.firstIdle = false;
					effect->animFinished = false;
				}

				float curTime = TimeMgr::GetInstance()->m_curTime;

				if (curTime - effect->animSequenceCurrentIndexStartTime > 3.0 && effect->animLooped) {
					FrogAnimOnStopOrLoop(effect); // progresses it into jump_up_right
				}

				effect->animLooped = false;
			}

			if (effect->animSequenceCurrentIndex == 2) { // jump_up_right
				float curTime = TimeMgr::GetInstance()->m_curTime;

				if (effect->IsPaused()) {
					if (effect->m_lastPauseTime == 0.0) {
						effect->m_lastPauseTime = curTime;
					}
					else {
						if (curTime - effect->m_lastPauseTime > 0.5) {
							effect->SetPaused(false);
							effect->m_lastPauseTime = 0.0;
							effect->m_lastUnpausedTime = curTime;
							effect->m_unPausedInState = true;
						}
					}
				}
				else {
					if (effect->m_unPausedInState) {
						// I have 17/30 seconds to move
						SexyVector3 newPos = frog.coords;
						newPos.mX += std::min(64.0, (64.0 * 30.0 / 10.0) * (curTime - effect->m_lastUnpausedTime));
						newPos.mZ += std::min(76.0, (76.0 * 30.0 / 10.0) * (curTime - effect->m_lastUnpausedTime));
						effect->SetPositionAndLayer(&newPos, effect->renderLayer);
					}
				}
			}

			if (effect->animSequenceCurrentIndex == 3) { // jump_down_right
				float curTime = TimeMgr::GetInstance()->m_curTime;

				if (effect->IsPaused()) {
					if (effect->m_lastPauseTime == 0.0) {
						effect->m_lastPauseTime = curTime;
					}
					else {
						if (curTime - effect->m_lastPauseTime > 0.0) {
							effect->SetPaused(false);
							effect->m_lastPauseTime = 0.0;
							effect->m_lastUnpausedTime = curTime;
							effect->m_unPausedInState = true;
						}
					}
				}
				else {
					if (effect->m_unPausedInState) {
						SexyVector3 newPos = frog.coords;
						newPos.mX += 64.0;
						newPos.mZ += std::max(0.0, 76.0 - (76.0 * 30.0 / 10.0) * (curTime - effect->m_lastUnpausedTime));
						effect->SetPositionAndLayer(&newPos, effect->renderLayer);
					}
				}
			}

			if (effect->relocatedPlant) {
				for (auto& plant2 : frog.plants) {
					RtWeakPtr<Plant> plantPtr = RtWeakPtr<Plant>();
					plantPtr.FromOther(&plant2);

					if (!plantPtr.is_valid()) {
						continue;
					}

					Plant* plant = plantPtr.get();
					assert(plant != NULL);

					if (plant->m_column < 9) {
						typedef void (*sub64AD88)(int, uint, uint, Plant*, Point*);
						int messageRouter = *(int*)getActualOffset(0x1C85050);
						typedef uint(*getListeners)(int, uint);
						getListeners func = *(getListeners*)(*(uint*)messageRouter + 0xC);
						int v22 = func(messageRouter, getActualOffset(0xE7BE80));
						Point newPoint = Point(plant->m_column + 1, plant->m_row);
						((sub64AD88)getActualOffset(0x64AD88))(messageRouter, getActualOffset(0xE7BE80), v22, plant, &newPoint);
						//LOGI("Doing listeners call to nullsub 145");

						if (plant->m_column + 1 == 9) {
							frog.markedForRemoval = true;
							frog.leftLawn = true;
						}

						typedef void (*relocate)(Plant*, int, int);
						((relocate)getActualOffset(0xE81A3C))(plant, plant->m_column + 1, plant->m_row);
					}
				}

				effect->relocatedPlant = false;
			}
		}

		if (!effect->isJumping() && frog.markedForRemoval) {
			if (!frog.removed) {
				LOGI("Frog marked for removal found");
				
				SexyString dieAnim = "transform_back";
				if (frog.leftLawn) {
					dieAnim = "disappear";
				}

				std::vector<SexyString> dieAnims = { dieAnim }; // disappear is used for outside of lawn
				PlaySuccessiveAnims(effect, dieAnims);
				frog.removed = true;
			}

			if (effect->animFinished) {
				// LOGI("Death anim finished, restoring plants");
				typedef void (*changeFlags)(Effect_FrogAnim*, int);
				((changeFlags)getActualOffset(0x740DC8))(effect, 0);

				for (auto& plant2 : frog.plants) {
					RtWeakPtr<Plant> plantPtr = RtWeakPtr<Plant>();
					plantPtr.FromOther(&plant2);

					if (!plantPtr.is_valid()) {
						continue;
					}

					Plant* plant = plantPtr.get();
					assert(plant != NULL);

					typedef bool (*canPlantTypeGoHere)(uint, Point*, RtWeakPtr<int>*, int);
					typedef int (*getBoardPlanting)();
					int boardPlanting = ((getBoardPlanting)getActualOffset(0x2AD5A0))();
					Point plantPoint = Point(plant->m_column, plant->m_row);
					RtWeakPtr<int> plantType = RtWeakPtr<int>();
					plantType.FromOther(&plant->m_type);
					int plantTypeReal = (int) plantType.get();

					FrogJumpSubsystem::checkIgnoreFrogs = true;
					bool canPlantGoHere = ((canPlantTypeGoHere)getActualOffset(0x6490B8))(boardPlanting, &plantPoint, &plantType, -1);
					FrogJumpSubsystem::checkIgnoreFrogs = false;

					// LOGI("Can plant go here = %d", canPlantGoHere);

					if (!canPlantGoHere) {
						typedef void (*plantDie)(Plant*, uint64_t);
						((plantDie)getActualOffset(0xE83498))(plant, 0x2000000000LL);
					}
					else {
						typedef void (*plantRemoveCond)(Plant*, PlantConditions);

						if (plant->m_conditionTracker.HasCondition(plant_sheeped)) {
							((plantRemoveCond)getActualOffset(0xE89C24))(plant, plant_sheeped);
						}
						else if (plant->m_conditionTracker.HasCondition(plantgroup_sheeped)) {
							((plantRemoveCond)getActualOffset(0xE89C24))(plant, plantgroup_sheeped);
						}
					}
				}

				effect->animFinished = false;
				frog.DestroyEffect();
			}
		}
	}
	// LOGI("Finished frog iteration");
}

void FrogJumpSubsystem::AddFrog(RtWeakPtr<Effect_FrogAnim>* frog, RtWeakPtr<PlantGroup>* plantGroup, SexyVector3* coords) {
	for (Frog& m_frog : m_frogs) {
		if (!m_frog.effect.is_valid()) {
			m_frog.ReInit(frog, plantGroup, coords);
			// LOGI("Replaced existing element, size is still %d", m_frogs.size());
			return;
		}
	}

	m_frogs.emplace_back(frog, plantGroup, coords);
	// LOGI("Frog size = %d", m_frogs.size());
}

void FrogJumpSubsystem::MarkFrogForRemoval(RtWeakPtr<Effect_FrogAnim>* other) {
	// LOGI("Mark frog for removal");
	for (Frog& frog : m_frogs) {
		if (frog.effect.equals(other)) {
			// LOGI("Found matching frog");
			frog.markedForRemoval = true;
			break;
		}
	}
}

void* FrogJumpSubsystem::Destroy(FrogJumpSubsystem* subsystem) {
	LOGI("Destroy FrogJumpSubsystem");
	subsystem->m_frogs.~vector<Frog>();
	
	typedef void* (*ChickenSubsystemDestroy)(ZombieChickenSubsystem*);
	return ((ChickenSubsystemDestroy) getActualOffset(0x341E04))(subsystem);
}

void FrogJumpSubsystem::DestroyAndDealloc(FrogJumpSubsystem* subsystem) {
	auto destroyed = Destroy(subsystem);
	operator delete(destroyed);
}

void FrogJumpSubsystem::ModInit() {
	LOGI("Initializing FrogJumpSubsystem");
	vftable = copyVFTable(getActualOffset(0x1C12F04), 15);
	patchVFTable(vftable, (void*)FrogJumpSubsystem::GetRTClass, 0);
	patchVFTable(vftable, (void*)FrogJumpSubsystem::Destroy, 2); // not testing these atm
	patchVFTable(vftable, (void*)FrogJumpSubsystem::DestroyAndDealloc, 3);
	patchVFTable(vftable, (void*)RunFrogSubsystem, 13);

	GetRTClass();
	LOGI("Finished initializing FrogJumpSubsystem");
}
