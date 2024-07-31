#include "pch.h"
#include "ZombiesFix.h"
#include "memUtils.h"
#include "PvZ2/Board.h"
#include "PvZ2/RtWeakPtr.h"
#include <vector>
#include "DamageInfo.h"

typedef int (*getDbInstanceType)();
getDbInstanceType getDbInstance = NULL;

typedef void (*zombieEnterStateType)(int, int, int);
zombieEnterStateType zombieEnterState = NULL;

typedef int* (*getTypeFromIDType)(int*, int, int);
getTypeFromIDType getTypeFromID = NULL;

typedef int (*zombieSetSpawnPositionType)(int, SexyVector3*);
zombieSetSpawnPositionType zombieSetSpawnPosition = NULL;

typedef void (*zombieKingOnSpawn)(int);
zombieKingOnSpawn oZombieKingOnSpawn = NULL;

// Get rows that no zombie with zombieClass is currently occupying
std::vector<int> getPotentialRows(int a1, int zombieClass) {
	Board* board = getBoard();
	int rowCount = board->m_rowCount;

	std::vector<int> potentialRows;
	for (int i = 0; i < rowCount; ++i) {
		potentialRows.push_back(i);
	}
	std::vector<int>* zombiesOnBoard = (std::vector<int>*)((uint)board + 0x3A8);
	for (int zombie : *zombiesOnBoard) {
		if (zombie == a1) {
			continue;
		}
		typedef int (*isType)(int, int);
		isType checkType = *(isType*)(*(int*)zombie + 0x10);
		if (checkType(zombie, zombieClass)) {
			float mY = *(float*)(zombie + 0x18);
			int gridY = (mY - GRID_TILE_TOP) / GRID_TILE_HEIGHT;

			auto it = std::find(potentialRows.begin(), potentialRows.end(), gridY);
			if (it != potentialRows.end()) {
				potentialRows.erase(it);
			}
		}
	}
	return potentialRows;
}

int getRandomRowInPotentialRows(std::vector<int>& potentialRows) {
	typedef void (*sub2ACF6C)(int*, int*);
	int* beginAddr = &(*potentialRows.begin());
	((sub2ACF6C)getActualOffset(0x2ACF6C))(beginAddr, beginAddr + (potentialRows.size() - 1));
	return *potentialRows.begin();
}

void hkZombieKingOnSpawn(int a1) {
	typedef void (*zombieFun49)(int);
	((zombieFun49)getActualOffset(0x8AEAF8))(a1);
	zombieEnterState(a1, 0x10, 0);

	typedef int (*getZombieKingClass)();
	int zombieKingClass = ((getZombieKingClass)getActualOffset(0x775038))();
	std::vector<int> potentialRows = getPotentialRows(a1, zombieKingClass);

	float currentY = *(float*)(a1 + 0x18);
	int currentRow = (currentY - GRID_TILE_TOP) / GRID_TILE_HEIGHT;
	// if it can be spawned on this row, then do so. Else, we spawn it on a random row
	if (potentialRows.empty()) {
		// LOGI("cannot spawn this zombie king, instantly killing it %d", a1);
		// this king cannot be spawned
		typedef void (*zombieFun36)(int, DamageInfo*);
		DamageInfo damageInfo = DamageInfo();
		damageInfo.flags = damage_instantly_fatal;
		((zombieFun36)getActualOffset(0x8B6EEC))(a1, &damageInfo);
		return;
	}

	if (std::find(potentialRows.begin(), potentialRows.end(), currentRow) == potentialRows.end()) {
		// can't spawn it here. Get a random row
		currentRow = getRandomRowInPotentialRows(potentialRows);
	}
	SexyVector3 newLoc = SexyVector3(744.0, 0x4C * currentRow + 0xDE, *(int*)(a1 + 0xC)); // current Z is taken
	zombieSetSpawnPosition(a1, &newLoc);
	typedef int (*sub8BBE50)(int, int);
	((sub8BBE50)getActualOffset(0x8BBE50))(a1, 1);
}

typedef uint(*zombieBeachFishermanSpawn)(int);
zombieBeachFishermanSpawn oZombieBeachFishermanSpawn = NULL;

uint hkZombieBeachFishermanSpawn(int a1) {
	typedef int (*getZombieFishermanClass)();
	int zombieFishermanClass = ((getZombieFishermanClass)getActualOffset(0x741DDC))();
	std::vector<int> potentialRows = getPotentialRows(a1, zombieFishermanClass);

	float currentY = *(float*)(a1 + 0x18);
	int currentRow = (currentY - GRID_TILE_TOP) / GRID_TILE_HEIGHT;

	if (potentialRows.empty()) {
		return -1u;
	}

	if (std::find(potentialRows.begin(), potentialRows.end(), currentRow) == potentialRows.end()) {
		// can't spawn it here. Get a random row
		currentRow = getRandomRowInPotentialRows(potentialRows);
	}
	return currentRow;
}

typedef void (*zombiePirateCaptainLaunchParrot)(int);
zombiePirateCaptainLaunchParrot oZombiePirateCaptainLaunchParrot = NULL;

void hkZombiePirateCaptainLaunchParrot(RtWeakPtr<int>* a1) {
	int rowCount = getBoard()->m_rowCount;

	RtWeakPtr<int> rows[rowCount];
	for (int i = 0; i < rowCount; ++i) {
		rows[i] = RtWeakPtr<int>();
	}

	int v60[6];
	getTypeFromID(v60, getDbInstance(), 0x47);

	while (v60[2] != v60[4]) {
		RtWeakPtr<int> plantGroupWeakPtr;
		typedef RtWeakPtr<int>* (*sub_10C5EF8)(RtWeakPtr<int>*, int*);
		((sub_10C5EF8)getActualOffset(0x10C5EF8))(&plantGroupWeakPtr, v60);
		int plantGroup = (int)plantGroupWeakPtr.get();

		int gridX = *(int*)(plantGroup + 0x64);
		if (gridX >= 0) {
			int gridY = *(int*)(plantGroup + 0x68);
			if (gridY < getBoard()->m_rowCount) {
				std::vector<RtWeakPtr<int>>* m_plants;
				m_plants = (std::vector<RtWeakPtr<int>>*)(plantGroup + 0x70);

				if (!m_plants->empty()) {
					RtWeakPtr<int> backmostPlant = RtWeakPtr<int>();
					backmostPlant.FromOther(&(*m_plants)[0]);
					int plant = (int) backmostPlant.get();

					typedef bool (*plantStealable)(int);
					bool canSteal = ((plantStealable)getActualOffset(0xE84C00))(plant);
					if (canSteal) {
						RtWeakPtr<int> plantToSteal = RtWeakPtr<int>();
						typedef void (*getPlantToStealFromGroup)(RtWeakPtr<int>*, RtWeakPtr<int>*);
						((getPlantToStealFromGroup)getActualOffset(0x840B2C))(&plantToSteal, &plantGroupWeakPtr);
						if (plantToSteal.is_valid()) {
							int plant = (int) plantToSteal.get();
							int row = *(int*)(plant + 0x98);
							if (rows[row].is_valid()) {
								RtWeakPtr<int> tmp = RtWeakPtr<int>();
								tmp.FromOther(&rows[row]);
								int v35 = (int) tmp.get();

								float currX = *(float*)(v35 + 0x14);
								if (currX < *(float*)(plant + 0x14)) { // plant->mX
									rows[row].FromOther(&plantToSteal);
								}
							}
							else {
								rows[row].FromOther(&plantToSteal);
							}
						}
					}
				}
			}
		}
		typedef int (*sub10C5F58)(int*);
		((sub10C5F58)getActualOffset(0x10C5F58))(v60);
	}

	typedef int (*sub10C5E40)(int*);
	((sub10C5E40)getActualOffset(0x10C5E40))(v60);
	memset(v60, 0, sizeof(v60));

	if (rowCount >= 1) {
		for (int i = 0; i < rowCount; ++i) {
			if (rows[i].is_valid()) {
				RtWeakPtr<int> v54;
				v54.FromOther(&rows[i]);
				typedef void (*sub841410)(int*, RtWeakPtr<int>*, int);
				((sub841410)getActualOffset(0x841410))(v60, &v54, 0x64);
			}
		}
	}
	if (v60[1] == v60[0]) {
		*a1 = RtWeakPtr<int>();
	}
	else {
		typedef void (*sub84156C)(RtWeakPtr<int>*, int*);
		((sub84156C)getActualOffset(0x84156C))(a1, v60);
	}
	typedef int (*destroyv60)(int*);
	((destroyv60)getActualOffset(0x8415FC))(v60);

	// the final dtor needn't be called, should auto destroy on stack destruction
}

typedef void (*impKillOnBounds)(int);
impKillOnBounds oImpKillOnBounds = NULL;

void hkImpKillOnBounds(int a1) {
	float mY = *(float*)(a1 + 0x18);
	typedef bool (*zombieIsDead)(int);
	zombieIsDead checkZombieDeath = (zombieIsDead)getActualOffset(0x8AF73C);
	if (!checkZombieDeath(a1)) {
		int gridY = (mY - GRID_TILE_TOP) / GRID_TILE_HEIGHT;
		if (gridY < getBoard()->m_rowCount) {
			bool getsUpFromLanding = *(uint8_t*)(a1 + 0x30D);
			int state;
			if (getsUpFromLanding) {
				state = 0x14;
				*(uint8_t*)(a1 + 0x30D) = false;
			}
			else {
				typedef bool (*zombieHasCondition)(int, int);
				zombieHasCondition checkZombieHasCondition = (zombieHasCondition)getActualOffset(0x8AF9E4);

				if (checkZombieHasCondition(a1, 0x17)) { // hypnotized cond
					typedef void (*sub8B25E8)(int, int);
					((sub8B25E8)getActualOffset(0x8B25E8))(a1, 1);
				}
				state = 1;
			}

			zombieEnterState(a1, state, 0);
		}
		else {
			typedef void (*zombieFatal)(int, int);
			((zombieFatal)getActualOffset(0x8AFC84))(a1, 0);
		}
	}
}

void initZombiesFix() {
	zombieEnterState = (zombieEnterStateType)getActualOffset(0x8AEC44);
	zombieSetSpawnPosition = (zombieSetSpawnPositionType)getActualOffset(0x2D874C);
	getDbInstance = (getDbInstanceType)getActualOffset(0x274CE4);
	getTypeFromID = (getTypeFromIDType)getActualOffset(0xDDCEB4);

	PVZ2HookFunction(0x83FC88, (void*)hkZombiePirateCaptainLaunchParrot, (void**)&oZombiePirateCaptainLaunchParrot, "fix parrots crash");
	// patches imps dying instantly on landing
	PVZ2HookFunction(0x7D276C, (void*)hkImpKillOnBounds, (void**)&oImpKillOnBounds, "ImpKillOnBounds");
	// fixes kings' spawning
	PVZ2HookFunction(0x77628C, (void*)hkZombieKingOnSpawn, (void**)&oZombieKingOnSpawn, "ZombieKing spawn");
	PVZ2HookFunction(0x743EEC, (void*)hkZombieBeachFishermanSpawn, (void**)&oZombieBeachFishermanSpawn, "ZombieFisherman spawn");
}