#include "pch.h"
#include "SixLaneAmbushFix.h"
#include "memUtils.h"
#include "PvZ2/Board.h"
#include "PvZ2/RtWeakPtr.h"

typedef int (*addZombieByTypeType)(Board*, int, int, int, int);
addZombieByTypeType addZombieByType = NULL;

typedef void (*spawnAmbushBeach)(int, int, int, int*);
spawnAmbushBeach oSpawnAmbushBeach = NULL;

void hkSpawnAmbushBeach(int beachEventSpawner, int amount, int a3, int a4) {
    int propSheet = getWeakPtr(beachEventSpawner + 0x10);
    typedef int (*getZombieTypeDirectoryInstance)();
    int typeDir = ((getZombieTypeDirectoryInstance)getActualOffset(0x28FE30))();

    SexyString a1;
    typedef int* (*subEBFC8C)(SexyString*, int);
    ((subEBFC8C)getActualOffset(0xEBFC8C))(&a1, propSheet);

    typedef void (*getZombieType)(RtWeakPtr<int>*, int, std::string*);
    RtWeakPtr<int> zType;
    ((getZombieType)getActualOffset(0x28FE9C))(&zType, typeDir, &a1);

    int totalSpawns = *(int*)(propSheet + 0x38);
    int amountSpawned = *(int*)(beachEventSpawner + 0x2C);
    amount = std::min(amount, totalSpawns - amountSpawned);
    if (amount == 0) {
        return;
    }
    // LOGI("we want to spawn %d more total = %d spawned = %d", amount, totalSpawns, amountSpawned);
    for (int i = 0; i < amount; ++i) {
        Board* board = getBoard();
        int rowCount = board->m_rowCount;
        int zombie = addZombieByType(board, (int)&zType, a3, 6, -1);

        typedef void (*sub8BC738)(int, int);
        ((sub8BC738)getActualOffset(0x8BC738))(zombie, 0);
        typedef int (*zombieSetLoot)(int, int);
        ((zombieSetLoot)getActualOffset(0x8BC728))(zombie, *(int*)(*(int*)(beachEventSpawner + 0x18) + 4 * i)); // this is a vector of loots

        //LOGI("loot set");

        int animRig = getWeakPtr(zombie + 0x80);
        typedef int(*setLayerVisible)(int, SexyString*, bool);
        SexyString seaweed = "zombie_seaweed1";
        ((setLayerVisible)getActualOffset(0x667590))(animRig, &seaweed, true);
        //LOGI("got past set layer visible");

        int colStart = *(int*)(propSheet + 0x44);
        int colEnd = *(int*)(propSheet + 0x48);
        if (colEnd > colStart) {
            typedef int (*sub112D0BC)(int, int);
            colStart += ((sub112D0BC)getActualOffset(0x112D0BC))(a4, colEnd - colStart);
        }

        int nextRowToSpawn = *(int*)(beachEventSpawner + 0x30);
        *(int*)(beachEventSpawner + 0x30) = (nextRowToSpawn + 0x11) % rowCount; // I REMADE THIS JUST TO FIX THIS
        //LOGI("got next row to spawn");
        SexyVector3 loc = SexyVector3(colStart * 64 + 0xE8, nextRowToSpawn * 76 + 0xDE, 600.0);

        typedef void (*zombieFun57)(int, SexyVector3*, int);
        (*(zombieFun57*)(*(uint*)zombie + 0xE4))(zombie, &loc, 1);

        ++amountSpawned;
        *(int*)(beachEventSpawner + 0x2C) = amountSpawned;
    }
}

typedef void (*zombieRainSpawnerSpawnAmbush)(int, int, int, int*);
zombieRainSpawnerSpawnAmbush oZombieRainSpawnerSpawnAmbush = NULL;

void hkZombieRainSpawnerSpawnAmbush(int zombieRainSpawner, int amount, int a3, int a4) {
    int propSheet = getWeakPtr(zombieRainSpawner + 0x10);

    typedef int (*getZombieTypeDirectoryInstance)();
    int typeDir = ((getZombieTypeDirectoryInstance)getActualOffset(0x28FE30))();

    SexyString a1;
    typedef int* (*sub291E5C)(SexyString*, int);
    ((sub291E5C)getActualOffset(0x291E5C))(&a1, propSheet);

    typedef void (*getZombieType)(RtWeakPtr<int>*, int, std::string*);
    RtWeakPtr<int> zType;
    ((getZombieType)getActualOffset(0x28FE9C))(&zType, typeDir, &a1);

    int totalSpawns = *(int*)(propSheet + 0x38);
    int amountSpawned = *(int*)(zombieRainSpawner + 0x2C);
    amount = std::min(amount, totalSpawns - amountSpawned);
    if (amount == 0) {
        return;
    }
    int rowCount = getBoard()->m_rowCount;
    for (int i = 0; i < amount; ++i) {
        typedef int (*spawnZombieFunc)(int, RtWeakPtr<int>*, int, int);
        int zombie = ((spawnZombieFunc)getActualOffset(0x28EDD0))(0, &zType, a3, 6);
        // LOGI("spawned zombie func");
        typedef void (*sub8BC738)(int, int);
        ((sub8BC738)getActualOffset(0x8BC738))(zombie, 0);
        typedef int (*zombieSetLoot)(int, int);
        ((zombieSetLoot)getActualOffset(0x8BC728))(zombie, *(int*)(*(int*)(zombieRainSpawner + 0x18) + 4 * i)); // this is a vector of loots

        // LOGI("Spawn zombie so far and set loot");

        int colStart = *(int*)(propSheet + 0x48);
        int colEnd = *(int*)(propSheet + 0x4C);
        typedef int (*sub112D0BC)(int, int);
        colStart += ((sub112D0BC)getActualOffset(0x112D0BC))(a4, colEnd - colStart);

        int nextRowToSpawn = *(int*)(zombieRainSpawner + 0x30);
        *(int*)(zombieRainSpawner + 0x30) = (nextRowToSpawn + 1) % rowCount;
        SexyVector3 loc = SexyVector3(colStart * 64 + 0xE8 - 10.0, nextRowToSpawn * 76 + 0xDE - 10.0, 600.0);

        // LOGI("location decided");

        typedef void (*setPosition)(int, SexyVector3*);
        ((setPosition)getActualOffset(0x2D850C))(zombie, &loc);
        // LOGI("set position");

        typedef void (*someFunc)(int, int, float, float, int, int, int);
        someFunc fun = *(someFunc*)(*(uint*)zombieRainSpawner + 0x58);
        int zombieFallTime = *(int*)(propSheet + 0x44);
        fun(zombieRainSpawner, zombie, loc.mX, loc.mY, 0, zombieFallTime, 0x44160000);
        // LOGI("set dropdown");

        ++amountSpawned;
        *(int*)(zombieRainSpawner + 0x2C) = amountSpawned;
    }
}

void initSixLaneAmbushFix() {
    addZombieByType = (addZombieByTypeType)getActualOffset(0x72B4D8);

	PVZ2HookFunction(0xEBF84C, (void*)hkSpawnAmbushBeach, (void**)&oSpawnAmbushBeach, "SpawnAmbushBeach");
	PVZ2HookFunction(0x29184C, (void*)hkZombieRainSpawnerSpawnAmbush, (void**)&oZombieRainSpawnerSpawnAmbush, "ZombieRainSpawner::SpawnAmbush");
}
