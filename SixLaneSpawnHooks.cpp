#include "pch.h"
#include "SixLaneSpawnHooks.h"
#include "memUtils.h"
#include "PvZ2/Board.h"
#include "PvZ2/RtWeakPtr.h"
#include <assert.h>
#include <vector>
#include "TimeMgr.h"

// we know for a fact bytes 110 only overrides the number 0 at 0x124 first 4 bytes. Idt this is used anywhere else, so we can keep the state like that
// massive assumption is that 0x124 is NOT used anywhere else other than choosing rows
uint8_t row6State110[4]; // stores some kind of time
uint8_t row6State124[16];

void boardInitSixLaneSpawns(Board* board) {
    memset(row6State124, 0, sizeof(row6State124));
    *(int*)row6State124 = 5; // for lane 5 stuff
    *(int*)row6State110 = 0x7F7FFFFF; // max float
    assert(*(int*)((uint)board + 0x124) == 0);
    memcpy((void*)((uint)board + 0x124), (const void*)row6State110, 4); // override the number 0 originally here
}

typedef uint (*getRowToSpawn)(int, uint, int, int, int);
getRowToSpawn oGetRowToSpawn = NULL;

uint hkGetRowToSpawn(int a1, uint row, int a3, int a4, int a5) {
    Board* board = getBoard();
    // how rows are internally: 0 - 4 is from 1 - 5. 5 is for no rows set. 6 and above is for those numbers
    // LOGI("Read row %u", row);
    if (row < 5) {
        return row;
    }
    else if (row == 5) {
        RtWeakPtr<int> v9 = RtWeakPtr<int>();
        v9.FromOther((RtWeakPtr<int>*) a3);

        typedef int (*chooseRowForZombie)(Board*, RtWeakPtr<int>*, int);
        int res = ((chooseRowForZombie)getActualOffset(0x72BB20))(board, &v9, a4);
        return res;
    }
    else {
        if (row <= board->m_rowCount) {
            return row - 1;
        }
        if (row == board->m_rowCount + 1) {
            return a5;
        }
        return 0;
    }
}

// this code currently dros support for ZombieSpawnerPattern on 6 rows
typedef void (*setWaveZombiesPositions)(int, std::vector<int>*, int);
setWaveZombiesPositions oSetWaveZombiesPositions = NULL;

void hkSetWaveZombiesPositions(int r0_0, std::vector<int>* zombiesVec, int r2_0) {
    if (getBoard()->m_rowCount < 6) {
        oSetWaveZombiesPositions(r0_0, zombiesVec, r2_0);
        return;
    }

    RtWeakPtr<int> propSheetPtr = RtWeakPtr<int>();
    propSheetPtr.FromOther((RtWeakPtr<int>*)(r0_0 + 0x10));
    int propSheet = (int)propSheetPtr.get();

    RtWeakPtr<int> jitterOffsetsPtr = RtWeakPtr<int>();
    jitterOffsetsPtr.FromOther((RtWeakPtr<int>*)(propSheet + 0x58));

    int zombieSpawningOffsetIncrement = 0x50;
    int zombieSpawningRandomJitter = 0x1E;

    if (!jitterOffsetsPtr.is_invalid()) {
        int jitterOffsets = (int)jitterOffsetsPtr.get();
        if (jitterOffsets) {
            zombieSpawningOffsetIncrement = *(int*)(jitterOffsets + 4);
            zombieSpawningRandomJitter = *(int*)(jitterOffsets + 8);
        }
    }

    // LOGI("jitter offsets got %d %d", zombieSpawningOffsetIncrement, zombieSpawningRandomJitter);

    // This section gets the ZombieSpawnerPattern with the name set at 0x60 of props. Existing is ZombieSpawnerPatternStagged or Delta.
    bool v56 = false; // if pattern exists

    // back to understood code
    int spawnOffset[10];
    memset(spawnOffset, 0, sizeof(spawnOffset));

    for (int zombie : *zombiesVec) {
        float mX = *(float*)(zombie + 0x14);
        float mY = *(float*)(zombie + 0x18);
        float mZ = *(float*)(zombie + 0x1C);
        int gridY = floorf((mY - GRID_TILE_TOP) / GRID_TILE_HEIGHT);

        typedef int (*vfunc1)(int);
        vfunc1 v1 = *(vfunc1*)(*(uint*)r0_0 + 0x68);
        if (v1(r0_0)) {
            // zombie_function_57
            typedef void (*zombieFunc57)(int, SexyVector3*, int);
            zombieFunc57 v2 = *(zombieFunc57*)(*(uint*)zombie + 0xE4);

            SexyVector3 spawnCoords = SexyVector3(mX, mY, mZ);
            v2(zombie, &spawnCoords, 1);
        }
        else {
            typedef int (*sub112D0BC)(int, int);
            int jitter = ((sub112D0BC)getActualOffset(0x112D0BC))(r2_0, zombieSpawningRandomJitter);
            float offset = spawnOffset[gridY] + jitter;

            mX += offset;
            spawnOffset[gridY] += zombieSpawningOffsetIncrement;

            if (v56) {
                // some code to add further offsets based on the pattern's rowBetweenOffset
            }
            typedef int (*boardEntitySetPos)(int, SexyVector3*);
            boardEntitySetPos setPosFunc = (boardEntitySetPos)getActualOffset(0x2D850C);
            SexyVector3 newPos = SexyVector3(mX, mY, mZ);
            setPosFunc(zombie, &newPos);
        }
    }
}

typedef int (*canAddZombieToRow)(Board*, int, RtWeakPtr<int>*);
canAddZombieToRow oCanAddZombieToRow = NULL;

int hkCanAddZombieToRow(Board* board, int row, RtWeakPtr<int>* a3) {
    // need to set 0x110 to a non-zero value so that the FC + 4 * 5 check passes for lane 6
    uint boardAddr = (uint)board;
    int oldFC;

    if (board->m_rowCount == 6) {
        oldFC = *(int*)(boardAddr + 0x110);
        *(int*)(boardAddr + 0x110) = 1;
    }
    int res = oCanAddZombieToRow(board, row, a3);
    if (board->m_rowCount == 6) {
        *(int*)(boardAddr + 0x110) = oldFC;
    }
    return res;
}

typedef int (*decideRandomRow)(float*, int, int);
decideRandomRow oDecideRandomRow = NULL;

int hkDecideRandomRow(float* rowWeights, int rowCount, int a3) { // patch up whatever that 0x124 spot is even using
    /*
    prep job:
    - override 0x124 again with its original data (thank god this function only accesses from 0x128 and above)
    - reserve and override the 0x174 data by 16 bytes (this needs to be overridden already from chooseRow)
    */
    // uint8_t reservedBytes[16];
    if (rowCount == 6) {
        uint boardAddr = (uint)getBoard();
        memcpy((void*)row6State110, (const void*)(boardAddr + 0x124), 4);
        *(int*)(boardAddr + 0x124) = 0; // I've checked with ADB, and there are no unexpected memory accesses for this
    }
    int res = oDecideRandomRow(rowWeights, rowCount, a3);
    //LOGI("res to spawn in = %d", res);
    assert(0 <= res && res < rowCount);
    if (rowCount == 6) {
        uint boardAddr = (uint)getBoard();
        memcpy((void*)(boardAddr + 0x124), (const void*)row6State110, 4);
    }
    return res;

    //typedef float (*sub_1087C58)(float, float, float);
    //typedef float (*sub_112D148)(int, float);
    //sub_1087C58 fun1087C58 = (sub_1087C58)getActualOffset(0x1087C58);
    //sub_112D148 fun112D148 = (sub_112D148)getActualOffset(0x112D148);

    //int v3 = rowCount;
    //float v9, v10;
    //if (rowCount < 1) {
    //    v9 = 0.0;
    //    v10 = FLT_MAX;
    //}
    //else {
    //    float totalWeight = 0.0;
    //    for (int i = 1; i <= rowCount; ++i) { // rowWeights actually start at index 1
    //        float weight = rowWeights[i];
    //        LOGI("weight = %f", weight);
    //        totalWeight += weight;
    //    }
    //    v9 = 0.0;
    //    v10 = 1.0 / totalWeight;
    //    
    //    for (int i = 2; i < rowCount; ++i) {
    //        float v13 = fun1087C58(v10 * rowWeights[i - 1], rowWeights[i], rowWeights[i + 1]);
    //        v9 += v13;
    //    }
    //}
    //float v14;
    //if (a3) {
    //    v14 = fun112D148(a3, v9);
    //}
    //else {
    //    float v15 = 0.0;
    //    float v16 = v9;
    //    if (v9 < 0.0) {
    //        v16 = 0.0;
    //        v15 = v9;
    //    }
    //    v14 = v15 + (v16 - v15) * (uint)(getRandomFloat() * 2.3283e-10);
    //}
    //LOGI("v14 = %f", v14);
    //int v18 = -1;
    //if (rowCount >= 2) {
    //    float v19 = 0.0;
    //    for (int i = 2; i < rowCount; ++i) {
    //        v19 += fun1087C58(v10 * rowWeights[i - 1], rowWeights[i], rowWeights[i + 1]);
    //        v18 = i - 2;
    //        if (v14 <= v19) {
    //            break;
    //        }
    //    }
    //}
    //typedef float* (*sub1087E5C)(float*, int, int);
    //((sub1087E5C)getActualOffset(0x1087E5C))(rowWeights, v3, v18);
    //int rowWeightsAddr = (int)rowWeights;
    //return *(int*)(rowWeightsAddr + 0x10 * v18);
}

typedef int (*chooseRowForZombie)(Board*, RtWeakPtr<int>*, int);
chooseRowForZombie oChooseRowForZombie = NULL;

int chooseRowForZombieOriginal(Board* board, RtWeakPtr<int>* a2, int a3);

int hkChooseRowForZombie(Board* board, RtWeakPtr<int>* a2, int a3) {
    /*
    preparations needed:
    - reserve 16 bytes at 0x174
    - as long as the following function doesn't access anything at 0x174 we are good. also assuming nothing outside of this function accesses 0x124
    */
    uint boardAddr = (uint)board;
    uint8_t reservedBytes[16];

    if (board->m_rowCount == 6) {
        // do our preparations
        memcpy((void*)reservedBytes, (const void*)(boardAddr + 0x174), 16);
        memcpy((void*)(boardAddr + 0x174), (const void*)row6State124, 16);

        //LOGI("boardAddr + 0x174 = %d", *(int*)(boardAddr + 0x174));
    }
    //int res = oChooseRowForZombie(board, a2, a3);
    int res = chooseRowForZombieOriginal(board, a2, a3);
    if (board->m_rowCount == 6) {
        memcpy((void*)row6State124, (const void*)(boardAddr + 0x174), 16);
        memcpy((void*)(boardAddr + 0x174), (const void*)reservedBytes, 16);
    }
    return res;
}

typedef int (*checkAnyValidRowForZombie)(Board*, RtWeakPtr<int>*);
checkAnyValidRowForZombie oCheckAnyValidRowForZombie = NULL;

int hkCheckAnyValidRowForZombie(Board* board, RtWeakPtr<int>* spawnInstruction) {
    for (int i = 0; i < board->m_rowCount; ++i) {
        RtWeakPtr<int> v5 = RtWeakPtr<int>();
        v5.FromOther(spawnInstruction);
        if (hkCanAddZombieToRow(board, i, &v5)) {
            return true;
        }
    }
    return false;
}

int chooseRowForZombieOriginal(Board* board, RtWeakPtr<int>* spawnInstruction, int r2_0) {
    RtWeakPtr<int> v6 = RtWeakPtr<int>();
    v6.FromOther(spawnInstruction);
    if (!hkCheckAnyValidRowForZombie(board, &v6)) {
        // just print debug message for no valid row to add this zombie. I don't see any asserts though
    }
    int v14 = *(int*)((uint)board + 0x320);
    RtWeakPtr<int> a3 = RtWeakPtr<int>();
    a3.FromOther((RtWeakPtr<int>*)((uint)board + 0x300));
    int v17 = (int) a3.get();
    int v20 = *(uint8_t*)(v17 + 0x108);
    float v21 = 0.0, v22 = 0.0;
    if (v20) {
        v21 = *(float*)(v14 + 0x70);
        v22 = *(float*)(v14 + 0x74);
    }
    else {
        typedef int (*sub55B55C)();
        int v23 = ((sub55B55C)getActualOffset(0x55B55C))();
        int v24 = v23 + 0x1F0;
        int v25 = 4 * *(int16_t*)(v24 + 4) - 4;
        float v26 = *(float*)(*(int*)(v14 + 0x64) + v25);
        float v27 = *(float*)(*(int*)(v14 + 0x58) + v25);

        if (v26 > 0.0) v22 = v26;
        if (v27 > 0.0) v21 = v27;
    }
    float* v28 = (float*)((uint)board + 0x128);
    float* v29 = (float*)((uint)board + 0x110);
    for (int i = 0; i < board->m_rowCount; ++i) {
        float weight = 0.0;
        if (hkCanAddZombieToRow(board, i, &v6)) {
            TimeMgr* timeMgr = TimeMgr::GetInstance();
            assert(timeMgr != NULL);
            float v35 = timeMgr->m_curTime - v29[i];
            weight = 1.0;
            if (v35 > 0.0) {
                weight = 0.01;
                if (v35 >= v21) {
                    weight = 1.0;
                    if (v35 < v22) {
                        float v36 = v35;
                        if (v22 != 0.0) v36 = v22;
                        weight = v35 / v36;
                    }
                }
            }
        }
        v28[4 * i] = weight;
    }
    return hkDecideRandomRow((float*)((uint)board + 0x124), board->m_rowCount, r2_0);
}

void initSixLaneSpawnsHooks() {
    PVZ2HookFunction(0x2904C4, (void*)hkGetRowToSpawn, (void**)&oGetRowToSpawn, "allow read rows from json to read row 6"); // implement get row to spawn logic for higher row numbers
    PVZ2HookFunction(0x28F474, (void*)hkSetWaveZombiesPositions, (void**)&oSetWaveZombiesPositions, "set wave zombie positions without crashing"); // true lane 6 fix for non crashing

    // random rows allow lane 6
    PVZ2HookFunction(0x72CBAC, (void*)hkCanAddZombieToRow, (void**)&oCanAddZombieToRow, "can add zombie to row"); // this still all passes the check. arguably unnecessary ngl
    PVZ2HookFunction(0x1087CFC, (void*)hkDecideRandomRow, (void**)&oDecideRandomRow, "decide random row to spawn");
    PVZ2HookFunction(0x72BB20, (void*)hkChooseRowForZombie, (void**)&oChooseRowForZombie, "choose row fro zombie");
    PVZ2HookFunction(0x72CAF4, (void*)hkCheckAnyValidRowForZombie, (void**)&oCheckAnyValidRowForZombie, "check for any valid rows for a zombie");
}
