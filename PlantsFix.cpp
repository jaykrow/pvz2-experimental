#include "PlantsFix.h"
#include <cmath>
#include "memUtils.h"
#include "PvZ2/RtWeakPtr.h"
#include "PvZ2/Board.h"
#include <random>

std::mt19937 mt_rand; // Optionally, you can seed the generator here or in a function
std::uniform_real_distribution<float> myDist(0.0, 1.0);

typedef int (*fun_91BC3C)(int, int);
fun_91BC3C oFun_91BC3C = NULL;

int hkFun_91BC3C(int r0_0, int a2) {
    int v4; // r7
    int v5; // r4
    int v6; // r6
    int TypeId; // r0
    int* v8; // r0
    int v10; // r0
    int v11; // r4
    int row; // r0
    float v13; // r4
    float v14; // r7
    float v15; // s0
    int v16; // r0
    float v17; // s16
    int* v18; // r0
    float v19; // s6
    float v20; // s0
    float v21; // s2
    float v22; // r0
    float v24[3]; // [sp+4h] [bp-6Ch] BYREF
    float v25; // [sp+10h] [bp-60h] BYREF
    float v26; // [sp+14h] [bp-5Ch]
    float v27; // [sp+18h] [bp-58h]
    int gridCoords[2]; // [sp+20h] [bp-50h] BYREF
    RtWeakPtr<int> a1; // [sp+28h] [bp-48h] BYREF

    v4 = *(int*)(r0_0 + 0x3C);
    if (!v4) {
        typedef int (*getRtWeakPtrRegistry)();
        getRtWeakPtrRegistry funGetRtWeakPtrRegistry = (getRtWeakPtrRegistry)getActualOffset(0x10C6764);
        v5 = funGetRtWeakPtrRegistry();

        typedef void (*sub_10CC81C)(RtWeakPtr<int>*, int, RtWeakPtr<int>*);
        ((sub_10CC81C)getActualOffset(0x10CC81C))(&a1, (int)v5, (RtWeakPtr<int>*)(r0_0 + 0x40));
        v6 = 0;
        v4 = 0;
        if (!a1.is_invalid())
        {
            TypeId = a1.type_id();

            typedef int (*sub_10CC404)(int, int);
            v8 = (int*)((sub_10CC404)getActualOffset(0x10CC404))(v5, TypeId);
            v4 = 0;
            if (v8) {
                typedef int (*resolveWeakPtr)(int*, RtWeakPtr<int>*);
                resolveWeakPtr funResolveWeakPtr = (resolveWeakPtr)getActualOffset(0x10C8780);
                v4 = funResolveWeakPtr(v8, &a1);
            }
        }

        typedef void (*weakPtrDtor)(RtWeakPtr<int>*);
        weakPtrDtor funWeakPtrDtor = (weakPtrDtor)getActualOffset(0x10B58F8);
        funWeakPtrDtor(&a1);
        *(int*)(r0_0 + 0x3C) = v4;
    }

    if (v4)
    {
        typedef int (*temp)(int, int);
        temp funTemp = *(temp*)(*(int*)v4 + 0x10);

        typedef int (*sub_91B488)();
        v10 = ((sub_91B488)getActualOffset(0x91B488))();
        v6 = funTemp(v4, v10);
        if (v6)
            v6 = v4;
    }

    typedef float (*sub_CDFF24)(int*, int);
    ((sub_CDFF24)getActualOffset(0xCDFF24))(gridCoords, a2);
    v11 = 0;
    row = gridCoords[1];
    if (gridCoords[1] != *(int*)(r0_0 + 0x13C))
    {
        *(int*)(r0_0 + 0x13C) = gridCoords[1];
        v13 = *(float*)(r0_0 + 0x74);
        v14 = *(float*)(r0_0 + 0x78);
        v25 = *(float*)(r0_0 + 0x70);
        v26 = v13;
        v27 = v14;
        v15 = v13;
        if (v13 == 0.0)
        {
            float v16[8];

            typedef int (*sub_10AB3B8)(float*);
            ((sub_10AB3B8)getActualOffset(0x10AB3B8))(v16); // setup v16's list

            typedef int (*sub_4A05BC)(float*, float);
            int tmp = *(int*)(v6 + 0x188) ^ 0x80000000;
            ((sub_4A05BC)getActualOffset(0x4A05BC))(v16, *(float*)&tmp);

            typedef float (*sub_10AB42C)(float*, float*, float*);
            ((sub_10AB42C)getActualOffset(0x10AB42C))(v24, v16, &v25);
            v25 = v24[0];
            v26 = v24[1];
            v27 = v24[2];
            // I want to change the probabilities of bouncing as well for 6 rows
            int rowCount = getBoard()->m_rowCount;
            // if it's on the middle bottom row, make it more likely to bounce down. if middle top, more likely to bounce up
            if (gridCoords[1] == rowCount - 2)
            {
                v17 = 1.0 - *(float*)(v6 + 0x18C);
            }
            else if (gridCoords[1] == 1)
            {
                v17 = *(float*)(v6 + 0x18C);
            }
            else
            {
                v17 = 0.5;
            }

            /*typedef int* (*MTRandInstanceGetter)();
            MTRandInstanceGetter getMTRandInstance = (MTRandInstanceGetter)getActualOffset(0x10ACB34);
            v18 = getMTRandInstance();*/
            v19 = 1.0;

            //typedef float (*RandomFloat)(int);
            //RandomFloat getRandomFloat = (RandomFloat)getActualOffset(0x27E044);

            // v20 = (float)((uint)(getRandomFloat((int)v18)));
            v20 = myDist(mt_rand);

            //LOGI("chance to bounce down is %f row = %d v20 = %f", v17, gridCoords[1], v20);

            row = gridCoords[1];
            if (v20 > v17)
                v19 = -1.0;
            v15 = v26 * v19;
            v26 = v26 * v19;
        }
        if (row)
        {
            Board* board = getBoard();
            if (row == board->m_rowCount - 1)
            {
                v21 = -1.0;
            }
            else
            {
                v21 = 1.0;
                if (v15 > 0.0)
                    v21 = -1.0;
            }
        }
        else
        {
            v21 = 1.0;
        }
        v22 = v25;
        v26 = v21 * fabsf(v15);
        *(float*)(r0_0 + 0x74) = v26;
        *(float*)(r0_0 + 0x70) = v22;
        *(float*)(r0_0 + 0x78) = v27;

        typedef int (*ProjectileCollideWithObject)(int, int);
        v11 = ((ProjectileCollideWithObject)getActualOffset(0x913424))(r0_0, a2);

        typedef void (*temp2)(int, int);
        (*(temp2*)(*(uint*)r0_0 + 0xD4))(r0_0, a2);
    }
    return v11;
}

typedef int (*fun_91BEB8)(float*, float);
fun_91BEB8 oFun_91BEB8 = NULL;

int hkFun_91BEB8(float* a1, float a2) {
    float v4; // s0

    typedef int (*updateVelocity)(int, float);
    ((updateVelocity)getActualOffset(0x912FA0))((int)a1, a2);
    int row = (int)floorf((float)(a1[6] + -160.0) / 76.0);
    if (row < 0)
    {
        v4 = fabsf(a1[0x1D]);
    }
    else
    {
        Board* board = getBoard();
        if (row <= board->m_rowCount - 1)
            return row;
        v4 = -fabsf(a1[0x1D]);
    }
    int result = (int)(a1[0x1D]);
    a1[0x1D] = v4;
    return result;
}

typedef void (*moonflowerSpawnTiles)(int, int);
moonflowerSpawnTiles oMoonflowerSpawnTiles = NULL;

void hkMoonflowerSpawnTiles(int a1, int a2) {
    //LOGI("spawn tile called");
    //oMoonflowerSpawnTiles(a1, a2);
    //return;
    typedef int (*subAE1310)(int, int);
    typedef bool (*subE81B70)(int);

    int v19[3] = { 0, 0, 0 };

    int rowCount = getBoard()->m_rowCount;
    int colCount = getBoard()->m_columnCount;

    if (((subAE1310)getActualOffset(0xAE1310))(0, a2) && !((subE81B70)getActualOffset(0xE81B70))(a2)) {
        typedef int (*subAE15CC)(int, int, int*, int);
        ((subAE15CC)getActualOffset(0xAE15CC))(a1, a2, v19, 0);
        
        typedef float (*v4Fun)(int);
        v4Fun fun = *(v4Fun*)(**(uint**)(a2 + 0x64) + 0x118);
        float v4 = fun(*(int*)(a2 + 0x64));
        float row = (float)(*(int*)(a2 + 0x98)); // row
        float v6 = v4 - 0.5;
        float v7 = v6 + row;
        float v8 = row - v6; // this looks right to me?
        // LOGI("v4 = %f v5 = %f v6 = %f v7 = %f v8 = %f", v4, row, v6, v7, v8);
        if (v8 <= v7) {
            float col = (float)(*(int*)(a2 + 0x9C)); // col
            int v10 = 0;
            float v11 = v6 + col;
            int v12 = (int)(2 * v4);
            // LOGI("v9 = %f v12 = %d", col, v12);
            do {
                if (v8 < 0.0 || v8 >= rowCount) {
                    v10 += v12;
                }
                else if (col - v6 <= v11) {
                    float v13 = col - v6;
                    do
                    {
                        // LOGI("loop here %f %f v19 = %d %d %d", v8, v13, v19[0], v19[1], v19[2]);
                        if (v13 >= 0.0 && v13 < colCount && !*(int*)(v19[0] + 4 * v10))
                        {
                            // LOGI("got in");
                            Board* board = getBoard();
                            int v18[2] = { (int)v13, (int)v8 };
                            typedef bool (*canBeShown)(Board*, int*);
                            canBeShown checkShowable = (canBeShown)getActualOffset(0x72A620);
                            if (!checkShowable(board, v18))
                            {
                                typedef int (*moonflowerGetClass)();
                                int moonflowerClass = ((moonflowerGetClass)getActualOffset(0xD29C48))();
                                
                                typedef int (*getDBType)(int, int, int);
                                int v16 = ((getDBType)getActualOffset(0x54B580))(moonflowerClass, 0x42, 1);
                                
                                typedef void (*subD2A1A0)(int, int*);
                                ((subD2A1A0)getActualOffset(0xD2A1A0))(v16, v18);
                            }
                        }
                        v13 += 1.0;
                        ++v10;
                    } while (v13 <= v11);
                }
                v8 += 1.0;
            } while (v8 <= v7);
        }
    }
    return;
}

typedef void (*getTargetsOnScreen)(int, char);
getTargetsOnScreen oGetTargetsOnScreen = NULL;

void hkGetTargetsOnScreen(int a1, char a2) {
    int v5[4];
    int v7[4]; // [sp+14h] [bp-3Ch] BYREF
    int v8[6];
    int v9; // [sp+3Ch] [bp-14h]

    v7[1] = 0;
    v7[0] = 0;
    v7[2] = getBoard()->m_columnCount;
    v7[3] = getBoard()->m_rowCount;

    memset(v5, 0, sizeof(v5));
    v5[0] = getActualOffset(0x1BCF1B0);
    v8[2] = 0;
    v8[1] = getActualOffset(0x50F2CC);
    v8[4] = 1;
    v8[3] = (int)v7;
    v8[0] = getActualOffset(0x1BCF1D8);
    v8[5] = getActualOffset(0x512A74);

    typedef int (*sub512A90)(int, int*, int);
    ((sub512A90)getActualOffset(0x512A90))((int)&v5, v8, 0);

    typedef void (*sub50E7D8)(int, int, int*);
    ((sub50E7D8)getActualOffset(0x50E7D8))(a1, a2, v5);

    v5[0] = getActualOffset(0x1BCF1B0);

    typedef void** (*sub512468)(void**);
    ((sub512468)getActualOffset(0x512468))((void**)&v5[1]);
}

typedef bool (*checkCanSwapToRow)(int, uint);
checkCanSwapToRow oCheckCanSwapToRow = NULL;

bool hkCheckCanSwapToRow(int zombie, uint targetRow) {
    typedef float (*subCDFF24)(uint*, int);
    uint gridCoords[2];
    ((subCDFF24)getActualOffset(0xCDFF24))(gridCoords, zombie);
    uint newCoords[2] = { gridCoords[0], targetRow };
    if (targetRow < getBoard()->m_rowCount)
    {
        typedef bool (*sub72A620)(Board*, uint*);
        return !((sub72A620)getActualOffset(0x72A620))(getBoard(), newCoords);
    }
    return false;
}

typedef void (*plantDusklobberCanCreateProj)(RtWeakPtr<int>*, int, int);
plantDusklobberCanCreateProj oPlantDusklobberCanCreateProj = NULL;

void hkPlantDusklobberCanCreateProj(RtWeakPtr<int>* projectile, int plantFramework, int offset) {
    // we combine byte patching and checks here. Hardcode the function inside to check for 6 rows, but filter the call here
    *projectile = RtWeakPtr<int>();
    int plant = *(int*)(plantFramework + 4);
    int plantRow = *(int*)(plant + 0x98);
    if (plantRow + offset < getBoard()->m_rowCount) {
        oPlantDusklobberCanCreateProj(projectile, plantFramework, offset);
    }
}

typedef void (*plantAppleMortarCanFireAt)(RtWeakPtr<int>*, int, int, int);
plantAppleMortarCanFireAt oPlantAppleMortarCanFireAt = NULL;

void hkPlantAppleMortarCanFireAt(RtWeakPtr<int>* projectile, int plantFramework, int offset, int a4) {
    *projectile = RtWeakPtr<int>();
    int plant = *(int*)(plantFramework + 4);
    int plantRow = *(int*)(plant + 0x98);
    if (plantRow + offset < getBoard()->m_rowCount) {
        oPlantAppleMortarCanFireAt(projectile, plantFramework, offset, a4);
    }
}

typedef void (*plantSlingPeaCanFireAt)(int, std::vector<int>*, int, uint, uint, int);
plantSlingPeaCanFireAt oPlantSlingPeaCanFireAt = NULL;

void hkPlantSlingPeaCanFireAt(int r0_0, std::vector<int>* targetsVec, int a3, uint gridX, uint gridY, int a6) {
    if (gridY >= getBoard()->m_rowCount) {
        return;
    }
    oPlantSlingPeaCanFireAt(r0_0, targetsVec, a3, gridX, gridY, a6);
}

typedef bool (*checkIntensiveCarrotShowPlant)(int, int);
checkIntensiveCarrotShowPlant oCheckIntensiveCarrotShowPlant = NULL;

bool hkCheckIntensiveCarrotShowPlant(int r0_0, int plant) {
    return oCheckIntensiveCarrotShowPlant(r0_0, plant);
}

void initPlantsFix() {
    // init my own random. I don't understand popcap's implementation in 91BC3C
    mt_rand.seed(time(0));

	// bouncing projectiles fix: bowling bulb, maypop and tumbleweed. maypop is not fixed in my obb idk
	PVZ2HookFunction(0x91BC3C, (void*)hkFun_91BC3C, (void**)&oFun_91BC3C, "bouncing projectiles collide with enemy fix");
    PVZ2HookFunction(0x91BEB8, (void*)hkFun_91BEB8, (void**)&oFun_91BEB8, "bouncing projectiles go out of board straight path");

    // moonflower allow tiles to spawn on row 6
    PVZ2HookFunction(0xAE13BC, (void*)hkMoonflowerSpawnTiles, (void**)&oMoonflowerSpawnTiles, "fix moonflower not having tiles on row 6");
    // bug still: moonflower moved by a minecart does not despawn the tiles properly on lane 6. I'll fix this later

    // this applies to everything screen-wide: teleportato mine, hocus, shadow shroom PF
    PVZ2HookFunction(0x50F18C, (void*)hkGetTargetsOnScreen, (void**)&oGetTargetsOnScreen, "get targets on screen");

    // Garlic allow lane 6 redirection
    PVZ2HookFunction(0xA03C60, (void*)hkCheckCanSwapToRow, (void**)&oCheckCanSwapToRow, "allow garlic to divert to lane 6");

    // fix apple mortar and dusk lobber
    PVZ2HookFunction(0xAEC768, (void*)hkPlantDusklobberCanCreateProj, (void**)&oPlantDusklobberCanCreateProj, "allow dusk lobber to fire on lane 6");
    PVZ2HookFunction(0xB823F4, (void*)hkPlantAppleMortarCanFireAt, (void**)&oPlantAppleMortarCanFireAt, "allow apple mortar to fire on lane 6");

    // fix sling pea same way: byte patching + custom check
    PVZ2HookFunction(0xBC2548, (void*)hkPlantSlingPeaCanFireAt, (void**)&oPlantSlingPeaCanFireAt, "allow sling pea to fire on lane 6");

    PVZ2HookFunction(0xA9E150, (void*)hkCheckIntensiveCarrotShowPlant, (void**)&oCheckIntensiveCarrotShowPlant, "allow intensive carrot to show dead plants lane 6");
}