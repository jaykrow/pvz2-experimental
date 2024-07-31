#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <map>
#include <unistd.h>
#include <substrate.h>
#include "memUtils.h"
#include "PVZ2ExampleMod.h"
#include "SexyTypes.h"
#include "PvZ2/Board.h"
#include <unwind.h>
#include <dlfcn.h>
#include "PvZ2/RtWeakPtr.h" // the code here uses 9.6 addresses
#include <stdarg.h>
#include "PlantsFix.h"
#include "ZombotanyHooks.h"
#include "TideChecker.h"
#include "GridItemPuddle.h"
#include "SixLaneModule.h"
#include "SixLaneModuleProps.h"
#include <random>
#include <assert.h>
#include "ZombiesFix.h"
#include "SixLaneSpawnHooks.h"
#include "SixLaneAmbushFix.h"
#include "RailAltHooks.h"
#include "GridItemRailAlt.h"
#include "GridItemRailAltProps.h"
#include "ZombieSteamMechanic.h"
#include "PlantCustomConditions.h"
#include "PlantAction.h"
#include "TimeMgr.h"
#include "PlantConditions.h"
#include "Plant.h"
#include "PlantGroup.h"
#include "SkyCity.h"
#include "SurvivalHooks.h"
#include "RoofHooks.h"
#include "SexyApp.h"
#include "RoofStage.h"
#include "ZombossActions.h"
#include "ZombieZombossMech.h"
#include "GridItemPowerTile.h"

#pragma region Alias to ID

class ZombieAlmanac
{
public:
    void* vftable;
    std::map<SexyString, uint> m_aliasToId;
};

class PlantNameMapper
{
public:
    void* vftable;
    std::map<SexyString, uint> m_aliasToId;
};

#define FIRST_FREE_ZOMBIE_ID 441
#define FIRST_FREE_PLANT_ID 185

std::vector<SexyString> g_modPlantTypenames;
std::vector<SexyString> g_modZombieTypenames;

#define REGISTER_PLANT_TYPENAME(typename) \
    g_modPlantTypenames.push_back(typename); \

#define REGISTER_ZOMBIE_TYPENAME(typename) \
    g_modZombieTypenames.push_back(typename); \

typedef ZombieAlmanac* (*ZombieAlmanacCtor)(ZombieAlmanac*);
ZombieAlmanacCtor oZombieAlmanacCtor = NULL;

void* hkCreateZombieTypenameMap(ZombieAlmanac* a1)
{
    // Let the game create the original alias->id map
    ZombieAlmanac* obj = oZombieAlmanacCtor(a1);
    // Now add our own zombie aliases to it 
    // (mod aliases can be registered with the REGISTER_ZOMBIE_TYPENAME macro)
    g_modZombieTypenames.clear();
    REGISTER_ZOMBIE_TYPENAME("frog_bitch");

    LOGI("Extra zombie typenames = %d", g_modZombieTypenames.size());

    for (int iter = 0; iter < g_modZombieTypenames.size(); iter++)
    {
        LOGI("Registering extra zombie typename %s", g_modZombieTypenames[iter].c_str());
        obj->m_aliasToId[g_modZombieTypenames[iter]] = FIRST_FREE_ZOMBIE_ID + iter;
    }

    return obj;
}

typedef PlantNameMapper* (*PlantNameMapperCtor)(PlantNameMapper*);
PlantNameMapperCtor oPlantNameMapperCtor = NULL;

void* hkCreatePlantNameMapper(PlantNameMapper* self)
{
    // Same deal with the ZombieAlamanc::ctor hook
    PlantNameMapper* obj = oPlantNameMapperCtor(self);

    g_modPlantTypenames.clear();
    REGISTER_PLANT_TYPENAME("mangofier");

    LOGI("Extra typenames size = %d", g_modPlantTypenames.size());

    for (int iter = 0; iter < g_modPlantTypenames.size(); iter++)
    {
        LOGI("Registering plant %s", g_modPlantTypenames[iter].c_str());
        obj->m_aliasToId[g_modPlantTypenames[iter]] = FIRST_FREE_PLANT_ID + iter;
    }

    return obj;
}

#pragma endregion

#pragma region Mummy Memory Fix

// The original function is broken for some reason. We don't need it
uint oCamelZombieFunc = NULL;

typedef void(*camelMinigameModuleFunc)(int, int, int);
camelMinigameModuleFunc cmmFunc = (camelMinigameModuleFunc)getActualOffset(0x797494);

void hkCamelZombieFunc(int a1, int a2, int a3)
{
    // Redirect call to some function in CamelMinigameModule
    // This fixes the crash when camels are rising from the ground
    cmmFunc(a1, a2, a3);
}

#pragma endregion

#pragma region Piano Zombie List

typedef bool (*initZombiePianoList)(int, int);
initZombiePianoList oInitZombiePianoList = NULL;

std::vector<SexyString>* g_pianoList = NULL;
bool g_pianoListInitialized = false;

bool hkInitZombiePianoList(int a1, int a2)
{
    // This function is called every frame when a piano zombie is on screen
    // So this global bool is needed to prevent wasting a massive amount of cpu time
    if (!g_pianoListInitialized)
    {
        bool orig = oInitZombiePianoList(a1, a2);

        uint ptrAddr = getActualOffset(0x1CE922C); // address of piano zombie's list in memory
        g_pianoList = reinterpret_cast<std::vector<SexyString>*>(ptrAddr);

        // @todo: add this to piano zombie's props instead?
        g_pianoList->clear();
        g_pianoList->push_back("cowboy");
        g_pianoList->push_back("cowboy_armor1");
        g_pianoList->push_back("cowboy_armor2");
        g_pianoList->push_back("cowboy_armor4");
        g_pianoList->push_back("pirate_gargantuar");

        g_pianoListInitialized = true;
    }
    return oInitZombiePianoList(a1, a2);
}

#pragma endregion

#pragma region Board Zoom

int gWidth = 0;
int gHeight = 0;

#define LAWNAPP_ADDR 0x01CF299C
inline uint getLawnApp() {
    return *(uint*)getActualOffset(LAWNAPP_ADDR);
}

uint getSexyApp() {
    return *(uint*)getActualOffset(SEXYAPP_ADDR);
}

enum AspectRatio
{
    Letterbox,
    Widescreen,
    Ultrawide,
};

AspectRatio GetAspectRatio()
{
    float ratio = (float)gWidth / (float)gHeight;
    if (ratio <= 1.4f)
    {
        return Letterbox;
    }
    else if (ratio >= 1.41f && ratio <= 1.85f)
    {
        return Widescreen;
    }
    else if (ratio >= 1.86f)
    {
        return Ultrawide;
    }
}

typedef void(*ReinitForSurfaceChange)(int, int, int, int, int);
ReinitForSurfaceChange oRFSC = nullptr;

void HkReinitForSurfaceChange(int thisptr, int a2, int width, int height, int a5)
{
    gWidth = width;
    gHeight = height;
    return oRFSC(thisptr, a2, width, height, a5);
}

typedef void* (*boardCtor)(void*);
boardCtor oBoardCtor = NULL;

void printBoard(int board);

void boardInitSixLanes(Board* board) {
    board->m_rowCount = 6;

    boardInitSixLaneSpawns(board);
}

void* hkBoardCtor(Board* board)
{
    oBoardCtor(board);

    LOGI("Board constructor called");

    board->m_lawnRect.mX = 200; //200
    board->m_lawnRect.mY = 160; //160

    switch (GetAspectRatio())
    {
        case Letterbox:
        {
            LOGI("Letterbox");
            board->m_lawnRect.mWidth = 576; //576
            board->m_lawnRect.mHeight = 500; //380
            break;
        }
        case Widescreen:
        {
            LOGI("widescreen");
            board->m_lawnRect.mWidth = 576; //576
            board->m_lawnRect.mHeight = 450; //380
            // board->m_lawnRect.mHeight = 540; //380
            break;
        }
        case Ultrawide:
        {
            LOGI("Ultrawide");
            board->m_lawnRect.mX = 450;
            board->m_lawnRect.mY = 160;
            board->m_lawnRect.mWidth = 576; //576
            board->m_lawnRect.mHeight = 525; //380
            break;
        }
    }

    return board;
}

#pragma endregion

#pragma region Dumb Hardcoded Immunities (Healer/Magician)

// we don't need to save a pointer to the original functions
uint dispose;

void hkMagicianHealerConditionFunc(int a1, int condition)
{
    // Hardcoded immunity checks would go here
    return;
}

bool hkMagicianHealerImmuneToShrink(int a1)
{
    // true = immune to shrinking
    return false;
}

bool hkMagicianInitializeFamilyImmunities(int a1, int64_t a2)
{
    typedef bool(*zFamilyFunc)(int);
    zFamilyFunc func = (zFamilyFunc)getActualOffset(0x8C70A0); // function 93 in Zombie's vftable 
    return func(a1);
}

#pragma endregion

#pragma region Adding 6th lane support

typedef int (*CanPlantAt)(int, int*, int, int, int);
typedef int (*AddPlantAt)(int, int, int, int, uint8_t*);
typedef int (*CanPlantTypeGoHere)(int, int*, int, int);

CanPlantAt oCanPlantAt = NULL;
AddPlantAt oAddPlantAt = NULL;
CanPlantTypeGoHere oCanPlantTypeGoHere = NULL;

typedef int (*mGetBoard)();
mGetBoard oGetBoard = NULL;

void printBoard(int board) {
    LOGI("Print board");
    LOGI("State at 0x174");
    for (int row = 0; row < 5; ++row) {
        std::string s = "";
        for (int col = 0; col < 9; ++col) {
            s += std::to_string(*(uint*)(board + 0x174 + (col * 5 + row) * 4)) + " ";
        }
        LOGI(s.c_str());
    }

    LOGI("State at 0x228");
    for (int row = 0; row < 5; ++row) {
        std::string s = "";
        for (int col = 0; col < 9; ++col) {
            s += std::to_string((int)(*(char*)(board + 0x228 + col * 5 + row))) + " ";
        }
        LOGI(s.c_str());
    }
}

Board* hookGetBoard() {
    // just making this available for own use
    return (Board*)oGetBoard();
}

int newCanPlantAt(int r0_0, int* a2, int a3, int a4, int a5) {
    // Don't use a3. It holds a pointer, not an integer

    // super hacky fix. On all lawns except for PS and WW these values should be as follows:
    // for 174 - 227: all 1 as integers
    // for 228 - 254: all 0 as chars
    Board* board = hookGetBoard();

    char old228Val;
    char old255Val;
    if (board->m_rowCount == 6) {
        if (*a2 == 0 && a2[1] == 0) {
            old228Val = *(char*)((uint)board + 0x228);
            *(char*)((uint)board + 0x228) = 0;
        }

        if (*a2 == 8 && a2[1] == 5) { // now that m_rowCount init has moved, we have to recheck this part of the code
            old228Val = *(char*)((uint)board + 0x228);
            old255Val = *(char*)((uint)board + 0x255);
            *(char*)((uint)board + 0x228) = 1;
            *(char*)((uint)board + 0x255) = 0;
        }
    }

    int result = oCanPlantAt(r0_0, a2, a3, a4, a5);
    // LOGI("Can plant at args %d %d %d %d %d result %d", r0_0, (a2 == NULL ? -1 : *a2), (a2 == NULL ? -1 : a2[1]), a4, a5, result);

    if (board->m_rowCount == 6) {
        if (*a2 == 0 && a2[1] == 0) {
            *(char*)((uint)board + 0x228) = old228Val;
        }

        if (*a2 == 8 && a2[1] == 5) {
            *(char*)((uint)board + 0x228) = old228Val;
            *(char*)((uint)board + 0x255) = old255Val;
        }
    }

    // Recreation of oCanPlantAt
    int v9;

    //uint board2 = (uint)hookGetBoard();
    //if (*a2 < 0 || *a2 >= *(uint32_t*)(board2 + 0x94) || (v9 = a2[1], v9 < 0) || v9 >= *(uint32_t*)(board2 + 0x98)) {
    //    // LOGI("board + 0x94 and + 0x98 values are %u %u", *(uint32_t*)(board + 0x94), *(uint32_t*)(board + 0x98));
    //    LOGI("Board coordinates outta range, should not happen");
    //}
    //LOGI("We want these checks to fail column %d row %d + 0x228 = %d!", *a2, a2[1], *(char*)(board2 + 0x228));
    //LOGI("Value of next check is %d, expecting 0", *(char*)(board2 + 5 * *a2 + a2[1] + 0x228)); // this check fails for (0, 0) with 6 rows WHAT?
    //LOGI("Value of next check is %d == 0 OR 2", (*(char*)(board2 + 0x14 * *a2 + 4 * a2[1] + 0x174)));


    // some jank:
    // PS 6th lane is treated as planks

    // LOGI("CanPlantAt result = %d", result);
    return result;
}

int newAddPlantAt(int r0_0, int a2, int a3, int a4, uint8_t* a5) {
    // Don't use a4. Same reason as above.
    // LOGI("Add plant at args %d %d %d %u", r0_0, a2, a3, (a5 == NULL ? -1 : *a5));
    // LOGI("address to listen to is %u gLibAddr = %u", (uint)getBoard() + 0x124, g_libAddress); // used for debugging GDB
    // LOG_RETURN_ADDRESS(0);
    return oAddPlantAt(r0_0, a2, a3, a4, a5);
}

int newCanPlantTypeGoHere(int r0_0, int* a2, int a3, int a4) {
    // LOGI("Can plant type go here %d %d %d %d", r0_0, (a2 == NULL ? -1 : *a2), a3, a4);
    return oCanPlantTypeGoHere(r0_0, a2, a3, a4);
}

typedef int (*fun_CF3DA8)(int);
fun_CF3DA8 oFun_CF3DA8 = NULL;

typedef void (*TempCF3DA8)(int*, int); // refers to CF3BE4 below

int hookFun_CF3DA8(int r0_0) {
    // r0_0 is pointer to this

    /*int v18[2];

    (*(TempCF3DA8*)(*(uint32_t*)r0_0 + 0x34))(&v18[0], r0_0);
    LOGI("v18 values are %d %d", v18[0], v18[1]);*/
    // LOGI("Go to address at %u first", getOriginalOffset(*(uint32_t*)r0_0 + 0x34));

    return oFun_CF3DA8(r0_0);
}

typedef int (*PlantCursorGetGridCoords)(unsigned int*, int);
PlantCursorGetGridCoords oPlantCursorGetGridCoords1 = NULL;

int hookPlantCursorGetGridCoords(unsigned int *gridCoordsToPopulate, int plantCursorAddr) {
    //int stuff1 = *(int*)(plantCursorAddr + 4);
    //int stuff2 = *(int*)(plantCursorAddr + 8);
    //int stuff3 = *(int*)(plantCursorAddr + 0x30);
    //LOGI("stuff1 = %d stuff2 = %d stuff3 = %d", stuff1, stuff2, stuff3);
    return oPlantCursorGetGridCoords1(gridCoordsToPopulate, plantCursorAddr);
}

typedef unsigned int (*PosYToGridY)(float);
PosYToGridY oPosYToGridY = NULL;

// no longer returns -1 for lane 6
unsigned int hookPosYToGridY(float a1) {
    unsigned int result;

    result = std::floorf((a1 + -160.0) / 76.0);
    if (result >= hookGetBoard()->m_rowCount)
        return 0xFFFFFFFF;
    return result;
}

typedef int (*IsValidGridCoordinates)(int, int);
IsValidGridCoordinates oIsValidGridCoordinates = NULL;

// returns true for lane 6 as well
int hookIsValidGridCoordinates(int column, int row) {
    // LOGI("Check valid coordinates %d %d", column, row);
    return column >= 0 && row >= 0 && column < hookGetBoard()->m_columnCount && row < hookGetBoard()->m_rowCount;
}

// Board::AddZombieByType might be responsible for spawning code. Check how it interprets row 6. TODO

// Plant::CreateProjectile: maybe there are no hitboxes associated with these projectiles? That sounds kinda nonsensical

// Board::AddProjectileByType HMMM

// typedef Projectile* (*AddProjectileByType)(int, int, int, int, int*, )

typedef int (*ProjectileCollideWithObject)(int, int);
ProjectileCollideWithObject oProjectileCollideWithObject = NULL;

int hookProjectileCollideWithObject(int a1, int a2) {
    // LOGI("Projectile collide with object %d %d", a1, a2);
    return oProjectileCollideWithObject(a1, a2);
}

typedef unsigned int (*ProjectileAffectTarget)(int, int);
ProjectileAffectTarget oProjectileAffectTarget = NULL;

unsigned int hookProjectileAffectTarget(int a1, int a2) {
    // LOGI("Projectile affect target %d %d", a1, a2);
    return oProjectileAffectTarget(a1, a2);
}

typedef bool (*PlantIsOnValidBoardPosition)(int); // use an int as a substitute for Plant* for now
PlantIsOnValidBoardPosition oPlantIsOnValidBoardPosition = NULL;

// I have no idea what this does tbh it's just here for now to be consistent. It doesn't fix anything directly.
bool hookPlantIsOnValidBoardPosition(int plantAddr) {
    // address of plant class. 98 is row, 9C is column and A0 is isOnBoard
    bool m_isOnBoard = *(bool*)(plantAddr + 0xA0);
    uint32_t m_row = *(uint32_t*)(plantAddr + 0x98);
    uint32_t m_column = *(uint32_t*)(plantAddr + 0x9C);
    // LOGI("isOnBoard %d m_row %d m_column %d", m_isOnBoard, m_row, m_column);

    return m_isOnBoard && m_row < hookGetBoard()->m_rowCount && m_column < hookGetBoard()->m_columnCount;
}

typedef void (*ProjectileFindCollisionTargets)(int, int);
ProjectileFindCollisionTargets oProjectileFindCollisionTargets = NULL;

void hookProjectileFindCollisionTargets(int projAddr, int hitObjsVecAddr) {
    // LOGI("Find collision targets called");

    // boardRow at 94, previous at 98
    int m_boardRow = *(int*)(projAddr + 0x94);
    int m_previousBoardRow = *(int*)(projAddr + 0x98);
    int m_boardRowHeight = *(int*)(projAddr + 0x9C);
    
    // alright, so boardRow is the issue
    //*(int*)(projAddr + 0x98) = 5;
    //*(int*)(projAddr + 0x94) = 5;

    int v8 = m_boardRowHeight - 1 + m_boardRow;
    if (v8 < m_boardRowHeight - 1 + m_previousBoardRow) {
        v8 = m_boardRowHeight - 1 + m_previousBoardRow;
    }
    // LOGI("boardRow %d prevBoardRow %d boardRowHeight %d v8 %d", m_boardRow, m_previousBoardRow, m_boardRowHeight, v8);

    return oProjectileFindCollisionTargets(projAddr, hitObjsVecAddr);
}

bool fromCreateProj;

typedef int (*BoardAddProjectileByType)(int, float, float, float, int, int);
BoardAddProjectileByType oBoardAddProjectileByType = NULL;

int hookBoardAddProjectileByType(int r0_0, float a2, float a3, float a4, int a5, int a6) {
    // if (fromCreateProj) {
        float plantZ = *(float*)(a6 + 0x1C);
        a4 += plantZ;
    // }
    // a6 is a Plant*
    // LOGI("Board add projectile by type %d %d %d %d", r0_0, a2, a3, a4);
    return oBoardAddProjectileByType(r0_0, a2, a3, a4, a5, a6);
}

typedef int (*Temp694E04)(unsigned int, unsigned, int);
typedef unsigned int (*fun_694E04)(unsigned int*, unsigned int*, Temp694E04*);
fun_694E04 oFun_694E04 = NULL;

unsigned int hookFun_694E04(unsigned int* a1, unsigned int* a2, Temp694E04* a3) {
    // LOGI("Sub 694E04 called with vector begin and end %p %p", a1, a2);
  
    return oFun_694E04(a1, a2, a3);
}

typedef int (*fun_9112CC)(int, int, int, int, int);
typedef int (*fun_915F10)(unsigned int*, int);
typedef int (*fun_915C80)(int, int, float, int);
fun_9112CC oFun_9112CC = NULL;
fun_915F10 oFun_915F10 = NULL;
fun_915C80 oFun_915C80 = NULL;

int hookFun_9112CC(int r0_0, int a2, int a3, int a4, int a5) {
    int res = oFun_9112CC(r0_0, a2, a3, a4, a5);
    
    /*
    int m_boardRow = *(int*)(r0_0 + 0x94);
    int m_previousBoardRow = *(int*)(r0_0 + 0x98);
    int m_boardRowHeight = *(int*)(r0_0 + 0x9C);
    LOGI("9112CC values are %d %d %d", m_boardRow, m_previousBoardRow, m_boardRowHeight);
    */

    return res;
}

int hookFun_915F10(unsigned int* r0_0, int a2) {
    int res = oFun_915F10(r0_0, a2);

    /*
    uintptr_t projAddr = reinterpret_cast<uintptr_t>(r0_0);
    int m_boardRow = *(int*)(projAddr + 0x94);
    int m_previousBoardRow = *(int*)(projAddr + 0x98);
    int m_boardRowHeight = *(int*)(projAddr + 0x9C);
    LOGI("915F10 values are %d %d %d", m_boardRow, m_previousBoardRow, m_boardRowHeight);
    */

    return res;
}

int hookFun_915C80(int a1, int a2, float a3, int a4) {
    int res = oFun_915C80(a1, a2, a3, a4);
    
    /*
    int m_boardRow = *(int*)(a1 + 0x94);
    int m_previousBoardRow = *(int*)(a1 + 0x98);
    int m_boardRowHeight = *(int*)(a1 + 0x9C);
    LOGI("915C80 values are %d %d %d", m_boardRow, m_previousBoardRow, m_boardRowHeight);
    */

    return res;
}

typedef int (*YPosToGridRowKeepOnBoard)(float);
YPosToGridRowKeepOnBoard oYPosToGridRowKeepOnBoard = NULL;

// projectiles use this to keep on board. We want to expand check for 6th lane.
int hookYPosToGridRowKeepOnBoard(float a1) {
    int result; // r0

    result = (int)std::floorf((float)(a1 + -160.0) / 76.0);
    if (result <= 0)
        result = 0;
    if (result >= hookGetBoard()->m_rowCount)
        return 5;
    return result;
}

typedef int (*AddZombieByType)(int, int, int, char, int);
AddZombieByType oAddZombieByType = NULL;

int hookAddZombieByType(int a1, int zombieTypeAddr, int a3, char a4, int a5) {
    // LOGI("Add zombie by type args: %d %d %d %d", a1, a3, a4, a5);
    // a3 is the wave number
    // a1 is some kinda address
    // a4 is always empty?
    // a5 is always -1?
    return oAddZombieByType(a1, zombieTypeAddr, a3, a4, a5);
}

typedef int (*ZombieEnterState)(int, int, int);
ZombieEnterState oZombieEnterState = NULL;

int hookZombieEnterState(int a1, int a2, int a3) {
    // a1 is a memory address fer sure
    // LOGI("Zombie enter state called with args %d %d %d", a1, a2, a3);
    // a2 is state:
    // state 1: walking. 2: eating. 3: lost head. 4: collapse. ra sucking is 16. 13 is player lost eating
    // a3 is always 0???

    int result = oZombieEnterState(a1, a2, a3);
    return result;
}

typedef unsigned int (*fun_7483F0)(float*);
fun_7483F0 oFun_7483F0 = NULL;

unsigned int hookFun_7483F0(float* r0_0) {
    // LOGI("Sub_778784 called");
    return oFun_7483F0(r0_0);
}

typedef unsigned int (*fun_772BC0)(int);
fun_772BC0 oFun_772BC0 = NULL;

int hookFun_772BC0(int zombieAddr) {
    // LOGI("Sub_772BC0 called");
    return oFun_772BC0(zombieAddr);
}

typedef int (*fun_399EF4)(int, int, int, int); // function that spawns zombies at a position methinks
fun_399EF4 oFun_399EF4 = NULL;

int hookFun_399EF4(int a1, int r1_0, int a3, int a4) {
    // LOGI("Sub_399EF4 called %d %d %d %d", a1, r1_0, a3, a4);
    return oFun_399EF4(a1, r1_0, a3, a4);
}

typedef int (*ZombieSetPosition)(int, int, int, int);
ZombieSetPosition oZombieSetPosition = NULL;

int hookZombieSetPosition(int zombieAddr, int x, int y, int z) {
    // LOGI("Zombie set position %d %d %d", x, y, z);
    return oZombieSetPosition(zombieAddr, x, y, z);
}

typedef int (*ZombieFunc108)(int);
ZombieFunc108 oZombieFunc108 = NULL;

int hookZombieFunc108(int zombieAddr) {
    // LOGI("Zombie func 108 called");
    return oZombieFunc108(zombieAddr);
}

typedef int (*ZombieFindAttackTarget)(int, int, int);
ZombieFindAttackTarget oZombieFindAttackTarget = NULL;

int hookZombieFindAttackTarget(int zombieAddr, int gridRow, int rectAddr) {
    int res = oZombieFindAttackTarget(zombieAddr, gridRow, rectAddr);
    // int (*rect)[4] = (int(*)[4]) rectAddr;
    // LOGI("Zombie find attack target %d %d array %d %d %d %d result %d", zombieAddr, gridRow, *rect[0], *rect[1], *rect[2], *rect[3], res);
    /*
    if (res != 0) {
        LOGI("Zombie find attack target %d %d %d result %d", zombieAddr, gridRow, rectAddr, res);
    } */
    return res;
}

typedef int (*zombieCheckTargetPlants)(int, int, int, int, int*);
zombieCheckTargetPlants oZombieCheckTargetPlants = NULL;

bool hookZombieCheckTargetPlants(int r0_0, int forcedTarget, int gridRow, int a4, int* a5) {
    bool res = oZombieCheckTargetPlants(r0_0, forcedTarget, gridRow, a4, a5);
    /*
    int ogA5 = *a5;
    int ogA51 = a5[1];
    if (res) {
        LOGI("8A7010 called %d %d %d %d %d original %d %d result %d", r0_0, forcedTarget, gridRow, *a5, a5[1], ogA5, ogA51, res);
    } */
    // LOGI("res = %d %u %u", res, *a5, a5[1]);
    return res;
}

// just another floorf fix. idk related to shovel somehow
typedef unsigned int (*fun_640338)(float, float, unsigned int*, unsigned int*);
fun_640338 oFun_640338 = NULL;

unsigned int hookFun_640338(float a1, float a2, unsigned int* a3, unsigned int* a4) {
    unsigned int v7; // s16
    float v8; // s0
    int v9; // r2
    unsigned int result; // r0
    int v11; // r1
    unsigned int v12; // s0
    unsigned int v13; // r3

    v7 = (int)floorf((float)(a1 + -200.0) * 0.015625);
    v8 = floorf((float)(a2 + -160.0) / 76.0);
    v9 = 0;
    result = v7;
    v11 = 0;
    v12 = (int)v8;
    v13 = v12;
    if (v7 >= hookGetBoard()->m_columnCount)
        result = 0xFFFFFFFF;
    if (result == 0xFFFFFFFF)
        v9 = 1;
    if (v12 >= hookGetBoard()->m_rowCount)
        v13 = 0xFFFFFFFF;
    if (v13 == 0xFFFFFFFF)
        v11 = 1;
    if (v9 | v11)
    {
        result = 0xFFFFFFFF;
        v13 = 0xFFFFFFFF;
    }
    *a3 = result;
    *a4 = v13;
    return result;
}

typedef unsigned int (*fun_6403F8)(uint*, float, float);
fun_640338 oFun_6403F8 = NULL;

unsigned int hookFun_6403F8(uint* a1, float a2, float a3) {
    unsigned int v5; // s16
    float v6; // s0
    int v7; // r2
    unsigned int result; // r0
    int v9; // r1
    unsigned int v10; // s0
    unsigned int v11; // r3

    v5 = (int)floorf((float)(a2 + -200.0) * 0.015625);
    v6 = floorf((float)(a3 + -160.0) / 76.0);
    v7 = 0;
    result = v5;
    v9 = 0;
    v10 = (int)v6;
    v11 = v10;
    if (v5 > getBoard()->m_columnCount)
        result = 0xFFFFFFFF;
    if (result == 0xFFFFFFFF)
        v7 = 1;
    if (v10 > getBoard()->m_rowCount)
        v11 = 0xFFFFFFFF;
    if (v11 == 0xFFFFFFFF)
        v9 = 1;
    if (v7 | v9)
    {
        v11 = 0xFFFFFFFF;
        result = 0xFFFFFFFF;
    }
    *a1 = result;
    a1[1] = v11;
    return result;
}

typedef int (*fun_5123D4)(int, int);
fun_5123D4 oFun_5123D4 = NULL;
typedef int (*Temp_5123D4)(int, int);

int hookFun_5123D4(int a1, int a2) { // does not change a2 at all
    /*
    // LOGI("og a2 = %u %d", a2, !a2 ? -1 : *(int*)a2);
    int v4, v5;
    v4 = *(int*)(a1 + 4);
    v5 = *(int*)(a1 + 8) - v4;
    // LOGI("5123D4 called %d %d v4 %d v5 %d", a1, a2, v4, v5); // careful with this lots of spam
    int myRes = 1;
    if (v5 >= 1) {
        int v6 = (*(Temp_5123D4*)(v4 + 0x14))(v4, a2);
        // LOGI("v6 = %d function at %u with args %u %u", v6, v4 + 0x14, v4, a2);
        if (v6) {
            int v8 = 0;
            int v9 = v5 / 24;
            int v10 = 0x2C;
            //LOGI("v9 = %d", v9);
            while (++v8 < v9) { // just loops v9 times
                bool v12 = (*(Temp_5123D4*)(v4 + v10))(v4 + v10 - 0x14, a2) == 0;
                //if (!v12) {
                //    LOGI("Access function at address %u args %u %u %u res %d", getOriginalOffset(*(int*)(v4 + v10)), *(int*)(v4 + v10), v4 + v10 - 0x14, a2, !v12);
                //}
                v10 += 0x18;
                if (v12) {
                     myRes = 0;
                     break;
                }
            }
        }
        else {
            myRes = 0;
        }
    }

    int res = oFun_5123D4(a1, a2);
    if (res != myRes) {
        LOGI("ALERT ALERT");
    }
    // LOGI("after a2 = %u %d", a2, !a2 ? -1 : *(int*)a2);
    int res = myRes;
    */
    int res = oFun_5123D4(a1, a2);
    // returning true causes all zombie spawns to instantly die
    // returning false disables all collisions again
    return res;
}

typedef int (*fun_50E7D8)(int, char, int);
fun_50E7D8 oFun_50E7D8 = NULL;

int hookFun_50E7D8(int a1, char a2, int a3) {
    // int* a5 = (int*)a1;
    // int oga5 = *a5, oga51 = a5[1];

    int res = oFun_50E7D8(a1, a2, a3);
    return res;
}

// make the view suitable for 6 rows
typedef void* (*moveBoardFunc)(float, int, int, int, int, int);
moveBoardFunc oMoveBoardFunc = nullptr;

void* hkMoveBoardFunc(float panTime, int startX, int endX, int startY, int endY, int curveType)
{
    int yPos = -25;
    LOGI("Move board: %.02f (%d, %d - %d, %d)", panTime, startX, endX, startY, endY);
    if (startY == 0)
        startY = yPos;
    if (endY == 0)
        endY = yPos;

    return oMoveBoardFunc(panTime, startX, endX, startY, endY, curveType);
}

typedef int (*PlantGroupInitWithGridCoords)(int*, unsigned int*);
PlantGroupInitWithGridCoords oPlantGroupInitWithGridCoords = NULL;

int hookPlantGroupInitWithGridCoords(int* plantGroup, unsigned int* a2) {
    int res = oPlantGroupInitWithGridCoords(plantGroup, a2);
    // override isOnBoard 
    int m_gridRow = a2[1];
    int m_gridColumn = *a2;
    // LOGI("Plant group init with coords %d %d original %d new %d", m_gridColumn, m_gridRow, *((bool*)(plantGroup) + 0x6C), m_gridColumn < 9 && m_gridRow < 6);
    *((bool*)(plantGroup) + 0x6C) = (m_gridColumn < hookGetBoard()->m_columnCount && m_gridRow < hookGetBoard()->m_rowCount);
    return res;
}

typedef void (*mowerPlaySmokeEffect)(float*);
mowerPlaySmokeEffect oMowerPlaySmokeEffect = NULL;

void hookMowerPlaySmokeEffect(float* r0_0) {
    LOGI("Play mower smoke anim %f %f %f", r0_0[5], r0_0[6], r0_0[7]);
    oMowerPlaySmokeEffect(r0_0);
}

typedef void (*MowerSpawnOnLawn)(int);
MowerSpawnOnLawn oMowerSpawnOnLawn = NULL;

void hookMowerSpawnOnLawn(int mower) {
    LOGI("custom code I think mower spawning code called for %d, row = %d v4 = %d float56 = %f %f", mower, *(int*)(mower + 0x80), *(int*)(mower + 0x84), ((float*)mower)[5], ((float*)mower)[6]);
    int mowerRow = *(int*)(mower + 0x80);
    float v3 = (0x4C * mowerRow + 0xDE);
    SexyVector3 v5 = SexyVector3(168.0, v3 + 22.0, 0.0);

    typedef void (*Zombie_Fun_13)(int, SexyVector3*);
    ((Zombie_Fun_13)getActualOffset(0x2D874C))(mower, &v5);

    *(char*)(mower + 0xA0) = 0;
    int v4 = *(int*)(mower + 0x84);
    *(float*)(mower + 0xA8) = -1.0;

    if (v4 == 1) {
        if (mowerRow == 5 || mowerRow == 1) {
            hookMowerPlaySmokeEffect((float*)mower);
        }
        *(int*)(mower + 0x84) = 2;
        // rest is sound playing code. Idc
    }
    //oMowerSpawnOnLawn(a1);
}

// nothing is done with this function, it's just used to reverse engineer 990BE4
typedef int (*fun_7207DC)(int, int);
fun_7207DC oFun_7207DC = NULL;

int hookFun_7207DC(int a1, int a2) {
    int res = oFun_7207DC(0, a2); // test, the function doesn't even use the board argument
    /*if (res) {
        LOGI("Function execution is at %u", getOriginalOffset(*(uint*)res + 0x10));
    }*/
    return res;
}

typedef void (*fun_990BE4)(int, float);
fun_990BE4 oFun_990BE4 = NULL;

void* savedUpMower = NULL;
void* mower6Pointer = NULL;

void hookFun_990BE4(int a1, float a2) {
    // LOGI("990BE4 called %d %f", a1, a2);
       
    float v3 = *(float*)(a1 + 0x10);
    uint board = (uint) hookGetBoard();
    int result = *(int*)(board + 0x98); // should be 6
    if (result >= 1) {
        float v6 = a2;
        int v7 = result + 1;
        int v8 = 0;
        // LOGI("v7 = %d", v7);

        // 7207DC cannot make a lane 5 lawnmower. Hacky solution below
        do {
            if (v7 == 7 && (float)(v3 + (float)((float)v8 * 0.1)) <= v6 && savedUpMower != NULL) {
                result = (int)savedUpMower;
                savedUpMower = NULL;
            }
            else {
                result = hookFun_7207DC(board, v7 - 2);
            }
            //if (v7 == 7) {
            //    LOGI("LANE 5 result = %d, v3 = %f v8 = %d v6 = %f result 0x84 = %d 0x80 = %d", result, v3, v8, v6, !result ? -1 : *(int*)(result + 0x84), !result ? -1 : *(int*)(result + 0x80));
            //}
            // LOGI("result = %d, v3 = %f v8 = %d v6 = %f result 0x84 = %d 0x80 = %d", result, v3, v8, v6, !result ? -1 : *(int*)(result + 0x84), !result ? -1 : *(int*)(result + 0x80));
            if (result && (float)(v3 + (float)((float)v8 * 0.1)) <= v6 && *(int*)(result + 0x84) == 1) {
                // LOGI("Call lawnmower creator!");
                // (*(int*)(result + 0x80))++;
                if (v7 == 6) {
                    // clone the mower object. Size: 0xC8
                    void* newMower = operator new(0xC8u);
                    memcpy(newMower, (void*)result, 0xC8u);
                    *(int*)((int)newMower + 0x80) = 5;
                    
                    mower6Pointer = newMower;

                    savedUpMower = (void*) result;
                    result = (int) newMower;
                    // the problem is, this mower is not tracked by the board to be destroyed
                }
                hookMowerSpawnOnLawn(result);
                ++v8;
            }
            --v7;
        } while (v7 > 1);
    }

    // return oFun_990BE4(a1, a2);
}

// shovel check
typedef uint(*fun_6404CC)(int);
fun_6404CC oFun_6404CC = NULL;

uint hookFun_6404CC(int a1) {
    unsigned int result;
    
    result = (a1 - 0xA0) / 0x4C;
    if (result >= hookGetBoard()->m_rowCount) {
        return 0xFFFFFFFF;
    }
    return result;
}

typedef int (*fun_104B6CC)(int, int*, int, int);
fun_104B6CC oFun_104B6CC = NULL;

int hookFun_104B6CC(int r0_0, int* a2, int a3, int a4) {
    LOGI("shovel digging code called");
    return oFun_104B6CC(r0_0, a2, a3, a4);
}

typedef int (*fun_7373A8)(int, int, int, float);
fun_7373A8 oFun_7373A8 = NULL;

int hookFun_7373A8(int r0_0, int a2, int a3, float a4) {
    // LOGI("7373A8 called %d %d %d %f", r0_0, a2, a3, a4);
    return oFun_7373A8(r0_0, a2, a3, a4);
}

typedef int (*fun_716E78)(int, float);
fun_716E78 oFun_716C5C = NULL;

int hookFun_716C5C(int a1, float a2) {
    // LOGI("716E78 TRIGGERED %d %f v45 = %f %f v7 = %f", a1, a2, *(float*)(a1 + 0x10), *(float*)(a1 + 0x14), *(float*)(a1 + 0x2C));
 /*   uint board = (uint)hookGetBoard();
    LOGI("Function called at board had address %d", getOriginalOffset(*(uint*)board + 0xCC));*/
    // leads to sub_7268F8 -> sub_72692C
    return oFun_716C5C(a1, a2 + 2);
}

typedef int (*BoardRenderLawn)(int*, int, int, int, int);
BoardRenderLawn oBoardRenderLawn = NULL;

bool calledFromRenderLawn;

int hookBoardRenderLawn(int* board, int x, int y, int a4, int a5) {
    // is this a rendering function? a2 is the x coordinate to render
    // ...yup, a2 is the x coordinate. a3 is the y coordinate
    // LOGI("Board fun 72692c called %d %d %d %d rowcount = %d", x, y, a4, a5, hookGetBoard()->m_rowCount);
    // LOGI("diffY = %d newa3 = %d", diffY, a3);
    if (a5 == 1536) {
        a5 += 150;
    }

    if (hookGetBoard()->m_rowCount == 6) {
        y -= 150;
        calledFromRenderLawn = true;
    }
    return oBoardRenderLawn(board, x, y, a4, a5);
}

// banana launcher's cursor
// typedef int (*fun_54AE70)(int*, int); // TODO: Check this out later 

typedef int (*GetBoardPlanting)();
GetBoardPlanting oGetBoardPlanting = NULL;

int hookGetBoardPlanting() {
    return oGetBoardPlanting();
}

typedef int (*PlantCursorCanPlantOnCursor)(int*);
typedef int (*TempCF3CC4)(int*, int*);
PlantCursorCanPlantOnCursor oPlantCursorCanPlantOnCursor = NULL;

int hkPlantCursorCanPlantOnCursor(int* a1) { // plant cursor fix
    int v11[2];
    int board = (*(TempCF3CC4*)(*a1 + 0x34))(v11, a1);
    // LOGI("Access address at %u", getOriginalOffset(*a1 + 0x34));
    int v2 = v11[0];
    int canPlantTypeGoHere = false;
    if (v11[0] >= 0) {
        int v4 = v11[1];
        Board* board = hookGetBoard();
        if (v4 < board->m_rowCount && v2 < board->m_columnCount) { // hardcode check in the original
            int boardPlanting = hookGetBoardPlanting();
            int v10[2] = { v2, v4 };
            RtWeakPtr<int> v9 = RtWeakPtr<int>();
            typedef void (*fromOther)(RtWeakPtr<int>*, RtWeakPtr<int>*);
            ((fromOther)(getActualOffset(0x10B59B8)))(&v9, (RtWeakPtr<int>*)(a1 + 0xC));
            canPlantTypeGoHere = newCanPlantTypeGoHere(boardPlanting, v10, (int)&v9, a1[0xF]);
            typedef void (*dtor)(RtWeakPtr<int>*);
            ((dtor)(getActualOffset(0x10B58F8)))(&v9);
        }
    }

    return canPlantTypeGoHere;
}

typedef int (*boardSetup)(int, uint*);
boardSetup oBoardSetup = NULL;

int hookBoardSetup(int board, uint *a2) {
    LOGI("Board setup %d %p", board, a2);
    printBoard(board);
    int res = oBoardSetup(board, a2);
    printBoard(board);
    return res;
}

typedef int (*hasPerkHandlerModuleProperties)(int);
hasPerkHandlerModuleProperties oHasPerkHandlerModuleProperties = NULL;

int hookHasPerkHandlerModuleProperties(int r0_0) {
    LOGI("Has perk handler module");
    bool result = 0;
    int begin = *(int*)(r0_0 + 0x70);
    int end = *(int*)(r0_0 + 0x74);

    int ans = 0;
    if (begin != end) {
        int v4 = 0;
        int v5 = 0;
        ans = 1;
        while (true) {
            RtWeakPtr<int> v14 = RtWeakPtr<int>(); // probably a pointer to the level module object
            typedef void (*fromOther)(RtWeakPtr<int>*, RtWeakPtr<int>*);
            ((fromOther)(getActualOffset(0x10B59B8)))(&v14, (RtWeakPtr<int>*)(begin + v4));
            // does not run 10CC81C, maybe there are side effects?
            int v8 = 0;
            int v9 = 0;
            typedef int (*RtWeakPtrRegistry)();
            int v7 = ((RtWeakPtrRegistry)(getActualOffset(0x10C6764)))();
            RtWeakPtr<int> a1;
            typedef void (*sub_10CC81C)(RtWeakPtr<int>*, int, RtWeakPtr<int>*);
            ((sub_10CC81C)(getActualOffset(0x10CC81C)))(&a1, v7, &v14);

            if (!a1.is_invalid()) {
                int typeId = a1.type_id();
                typedef int (*sub_10CC404)(int, int);
                int* v11 = (int*)((sub_10CC404)(getActualOffset(0x10CC404)))(v7, typeId);
                if (v11) {
                    typedef int (*resolveWeakPtr)(int*, RtWeakPtr<int>*);
                    v9 = ((resolveWeakPtr)(getActualOffset(0x10C8780)))(v11, &a1);
                }
            }

            typedef void (*dtor)(RtWeakPtr<int>*);
            ((dtor)(getActualOffset(0x10B58F8)))(&a1);

            if (v9) {
                typedef int (*temp)(int, int);
                temp v12 = *(temp*)(*(int*)v9 + 0x10); // v12 are all isType functions
                typedef int (*sub_A239B4)();
                int v13 = ((sub_A239B4)(getActualOffset(0xA239B4)))();

                // LOGI("access fun at %u with args %d %d", getOriginalOffset((uint) v12), v9, v13);

                v8 = v12(v9, v13); // probably checking if v9 is the proper
            }
            ((dtor)(getActualOffset(0x10B58F8)))(&v14);

            if (v8) {
                break;
            }
            ++v5;
            v4 += 8;
            if (v5 >= (end - begin) / 8) {
                ans = 0;
                break;
            }
        }
    }
    /*int ogAns = oHasPerkHandlerModuleProperties(r0_0);
    if (ans != ogAns) {
        LOGI("OH NOOOOOO");
    }*/
    LOGI("Res = %d", ans);
    return ans;
}

typedef int (*BoardHandleTouch)(uint*, int*);
BoardHandleTouch oBoardHandleTouch = NULL;

int hookBoardHandleTouch(uint* a1, int* a2) {
    LOGI("Handle touch");
    return oBoardHandleTouch(a1, a2);
}

typedef int (*fun)(int*);
fun oFun = NULL;

int hookFun(int* a) {
    LOGI("Fun called %d %d %d %d", a[9], a[10], a[11], a[12]);
    if (a[10] == -100) {
        int v2 = *(int*)((int)a + 0x14);
        if (v2) {
            /*typedef int* temp(int);
            return *(temp*)(*(int*)v2 + 0x74)(v2);*/
            LOGI("Jump to addr %u", getOriginalOffset(*(int*)v2 + 0x74));
            //return (*(int(__fastcall**)(int))(*(_DWORD*)v2 + 0x74))(v2);
        }
        a[10] = 0;
    }
    //if (((int*)a)[9] == -100) {
    //    LOGI("Fun called");
    //}
    int res = oFun(a);
    return res;
}

typedef int (*fun_71FBF8)(int);
fun_71FBF8 oFun_71FBF8 = NULL;

int hookFun_71FBF8(int a1) {
    LOGI("71FBF8 called");
    uint board = (uint) hookGetBoard();
    int res = oFun_71FBF8(a1);
    int mX = *(int*)(board + 0x368);
    int mY = *(int*)(board + 0x36C);
    int mWidth = *(int*)(board + 0x370);
    int mHeight = *(int*)(board + 0x374);
    LOGI("mX = %d mY = %d mWidth = %d mHeight = %d", mX, mY, mWidth, mHeight);
    LOGI("Lawnapp + 6C8 = %d", *(int*)(getLawnApp() + 0x6C8));
    return res;
}

void lawnAppInit() { // won't work. Function hookings are done after lawn app is already init
    uint lawnApp = getLawnApp();
    LOGI("mWidth = %d mHeight = %d", *(int*)(lawnApp + 0x88), *(int*)(lawnApp + 0x8C));

    // *(int*)(lawnApp + 0x8C) = 1727;
}

typedef int (*fun2)(int);
fun2 oFun2 = NULL;

int hkFun2(int gLawnApp) {
    float v3 = (float)*(int*)(gLawnApp + 0x88);
    float v4 = *(float*)(gLawnApp + 0x6CC);
    int v6 = (int)(float)((float)*(int*)(gLawnApp + 0x51C) - *(float*)(gLawnApp + 0x6CC));
    int v7 = (int)(float)((float)*(int*)(gLawnApp + 0x520) - *(float*)(gLawnApp + 0x6D0));
    // -v6 set to 100, -v7 set to 104 of board
    // v6 set to 824, v7 set to 828 of board
    LOGI("v3 = %f v4 = %f v6 = %d v7 = %d");
    int res = oFun2(gLawnApp);

    return res;
}

typedef int (*renderLawn)(int);
renderLawn oRenderLawn = NULL;

int hkRenderLawn(int stageModule) {
    int res = oRenderLawn(stageModule);

    int resValue = *(int*)(getActualOffset(0x1CFC8CC));
    LOGI("Resolution = %d", resValue);
    return res;
}

typedef int (*StageModuleGetLawnTextures)(uint*);
StageModuleGetLawnTextures oStageModuleGetLawnTextures = NULL;

int hkStageModuleGetLawnTextures(uint *stageModule) {
    typedef bool (*rtIsValidPtr)(long long*);
    rtIsValidPtr isValidPtr = (rtIsValidPtr) getActualOffset(0x0287E28);
    /*if (!isValidPtr((long long*)stageModule + 9)
        || !isValidPtr((long long*)stageModule + 10)
        || !isValidPtr((long long*)stageModule + 11)) {

        typedef int (*sub_4C1034)(uint*);
        int v3 = ((sub_4C1034)getActualOffset(0x4C1034))(stageModule);
        typedef int 
    }*/
    return oStageModuleGetLawnTextures(stageModule);
}

typedef int (*fun4)();
fun4 oFun4 = NULL;

int hkFun4() {
    int res = oFun4();
    LOGI("resolution = %d", res);
    return res;
}

typedef int (*fun5)(int);
fun5 oFun5 = NULL;

int hkFun5(int a) {
    LOGI("fun5 called val = %d", *(int*)(getLawnApp() + 0x8C));
    // *(int*)(getLawnApp() + 0x8C) += 100;
    int res = oFun5(a);
    // *(int*)(getLawnApp() + 0x8C) -= 100;
    return res;
}

typedef int (*fun6)(int, int*, int);
fun6 oFun6 = NULL;

int hkFun6(int a1, int* a2, int a3) {
    LOGI("Touch delegate called");
    int res = oFun6(a1, a2, a3);
    return res;
}

typedef int (*seedChooserPress)(int, int, int);
seedChooserPress oSeedChooserPress = NULL;

int hkSeedChooserPress(int a1, int a2, int a3) {
    LOGI("Pressed on seed chooser");
    int res = oSeedChooserPress(a1, a2, a3);
    return res;
}

bool callFromSetup = false;

typedef int (*boardSetupUI)(int, int);
boardSetupUI oBoardSetupUI = NULL;

int hkBoardSetupUI(int board, int a2) {
    LOGI("board + 0x9C = %d, a2 = %d", *(int*)(board + 0x9C), a2);
    if (a2 == 3) {
        callFromSetup = true;
    }
    int res = oBoardSetupUI(board, a2);
    return res;
}

typedef int (*fun_72215C)(int, int);
fun_72215C oFun_72215C = NULL;

int hkFun_72215C(int a1, int a2) {
    /*if (callFromSetup) {
        callFromSetup = false;
        return 0;
    }
    else {*/
        return oFun_72215C(a1, a2);
    //}
}

typedef int (*fun7)(int);
fun7 oFun7 = NULL;

int hkFun7(int boardAddr) {
    LOGI("fun7 called");
    if (callFromSetup) {
        callFromSetup = false;
        return oFun7(boardAddr);
    }
    else {
        return oFun7(boardAddr);
    }
}

typedef int (*widgetFun9)(int, int);
widgetFun9 oWidgetFun9 = NULL;

int hkWidgetFun9(int x, int y) {
    // LOGI("widget fun 9");
    // return false;
    return oWidgetFun9(x, y);
}

typedef bool (*seedChooserViewSelector)(int, int, int);
seedChooserViewSelector oSeedChooserViewSelector = NULL;

bool hkSeedChooserViewSelector(int a1, int a2, int a3) {
    // LOGI("called seed chooser init view selector");
    return oSeedChooserViewSelector(a1, a2, a3);
    // return false;
}

typedef int (*widgetFun51)(int, int, int, int, int);
widgetFun51 oWidgetFun51 = NULL;

int hkWidgetFun51(int a1, int a2, int a3, int a4, int a5) {
    if (calledFromRenderLawn) {
        calledFromRenderLawn = false;
        // a3 += 100;
        // LOGI("a1 = %d x = %d y = %d a4 = %d a5 = %d", a1, a2, a3, a4, a5);
        // int* a1ptr = (int*)a1;
        // LOGI("getBoard = %d a1 9 10 11 12 = %d %d %d %d", (int)hookGetBoard(), a1ptr[9], a1ptr[10], a1ptr[11], a1ptr[12]);

        // so addresses 36, 40, 44, 48 are these args. 36 and 40 are the current rendering position of the widget
    }
    return oWidgetFun51(a1, a2, a3, a4, a5);
}
//
//typedef int (*widgetFun26)(int, )

typedef bool (*boardCanAddGridItemHere)(Board*, uint, uint, int, int);
boardCanAddGridItemHere oBoardCanAddGridItemHere = NULL;

bool hkBoardCanAddGridItemHere(Board* board, uint col, uint row, int a4, int a5) {
    /*
    int v10 = 0;
    if (col < board->m_columnCount && row < board->m_rowCount && *(int*)((uint)board + 20 * col + 4 * row + 0x174) == 1) {
        int v10 = 0;
        int v27 = 0;
        uint v25 = (uint)((col << 6) + 0xE8);
        uint v26 = (uint)(0x4C * row + 0xDE);
        typedef int (*sub_72A6D0)(int, int, int);
        //if ( !((sub_72A6D0)(getActualOffset(0x72A6D0)))(a1,  )
    }
    */
    // LOGI("ask for can add grid item here col %d row %d", col, row);
    return oBoardCanAddGridItemHere(board, col, row, a4, a5);
}

typedef int (*fun_54AE70)(int*, int);
fun_54AE70 oFun_54AE70 = NULL;

int hkFun_54AE70(int* a1, int a2) {
    //return oFun_54AE70(a1, a2);

    typedef int (*sub_405138)(int);
    sub_405138 fun_405138 = (sub_405138)getActualOffset(0x405138);

    int v3 = a2;
    int v4 = *(int*)(a2 + 0x28);
    if (v4 == 4)
    {
        fun_405138((int)a1);
        // ((sub_405138)(getActualOffset(0x405138))((int)a1);
        v4 = *(int*)(v3 + 0x28);
    }
    int result = 0;
    if (v4 == 3)
    {
        typedef int (*sub_405058)(int);
        sub_405058 fun_405058 = (sub_405058)(getActualOffset(0x405058));
        if (fun_405058((int)a1))
        {
            typedef int (*sub_40520C)(int);
            ((sub_40520C)(getActualOffset(0x40520C)))((int)a1);
        }
        else
        {
            Board* board = hookGetBoard();
            int mX = board->m_lawnRect.mX;
            int mY = board->m_lawnRect.mY; // D0 is mX, D1 is mY, D2 is mWidth, D3 is mHeight
            int xCoord = a1[1];
            int yCoord = a1[2];
            int maxY = std::ceilf(board->m_lawnRect.mHeight * (board->m_rowCount == 6 ? 1.2 : 1.0)) + mY;
            int maxX = board->m_lawnRect.mWidth + mX;
            int cursorWithinBounds = yCoord < maxY && xCoord < maxX; // within bounds
            // LOGI("mY = %d mHeight = %d maxY = %d rowCount = %d", mY, board->m_lawnRect.mHeight, maxY, board->m_rowCount);
            // LOGI("cursor within bounds = %d", cursorWithinBounds);

            if (cursorWithinBounds)
            {
                bool v11 = xCoord < mX || yCoord < mY;
                if (!v11)
                {
                    int v12 = a1[0xC];
                    int v13[2] = { xCoord, yCoord };
                    typedef char (*sub_A59B34)(int, int*);
                    ((sub_A59B34)(getActualOffset(0xA59B34)))(v12, v13);
                }
            }
            fun_405138((int)a1);
        }
        return 1;
    }
    return result;
}

typedef int (*fun_A5921C)(int, int*);
fun_A5921C oFun_A59B34 = NULL;

int hkFun_A59B34(int a1, int* a2) {
    LOGI("A59B34 called a2 = %d a2[1] = %d", *a2, a2[1]);
    // return false;
    return oFun_A59B34(a1, a2);
}

typedef int (*fun_A578E4)(int*, int);
fun_A578E4 oFun_A578E4 = NULL;

int hkFun_A578E4(int* a1, int a2) {
    // a2 = 1 -> doesn't shoot, resets. 10: aiming anim. 11 = just shot anim. 12 = reset to idle no ammo anim
    // PlantBanana + 0x7C = anim state
    LOGI("A578E4 called a2 = %d", a2);
    return oFun_A578E4(a1, a2);
}

typedef uint* (*fun_6402CC)(uint*, int, int);
fun_6402CC oFun_6402CC = NULL;

uint* hkFun_6402CC(uint* res, int a2, int a3) {
    Board* board = hookGetBoard();

    int v3 = 0;
    int v4 = 0;
    uint v5 = (a3 - 0xA0) / 0x4C;
    int v6 = a2 - 0xC8 + ((unsigned int)((a2 - 0xC8) >> 0x1F) >> 0x1A);
    if (v5 >= board->m_rowCount)
        v5 = 0xFFFFFFFF;
    if (v5 == 0xFFFFFFFF)
        v3 = 1;
    uint v7 = v6 >> 6;
    if (v7 >= board->m_columnCount)
        v7 = 0xFFFFFFFF;
    if (v7 == 0xFFFFFFFF)
        v4 = 1;
    if (v4 | v3)
    {
        v5 = 0xFFFFFFFF;
        v7 = 0xFFFFFFFF;
    }

    *res = v7;
    res[1] = v5;
    return res;
}

typedef void (*fun_649EC0)(int*, int, int);
fun_649EC0 oFun_649EC0 = NULL;

void hkFun_649EC0(int* a1, int a2, int a3) {
    unsigned int v6; // r5
    unsigned int v7; // r8
    int CanPlantTypeGoHere; // r6
    unsigned int* v9; // r0
    RtWeakPtr<int> v12; // [sp+0h] [bp-30h] BYREF
    unsigned int v13; // [sp+8h] [bp-28h] BYREF
    unsigned int v14; // [sp+Ch] [bp-24h]
    int v15; // [sp+10h] [bp-20h]

    Board* board = hookGetBoard();

    v6 = 0;
    *a1 = 0;
    a1[1] = 0;
    a1[2] = 0;
    do
    {
        v7 = 0;
        do
        {
            v14 = v6;
            v13 = v7;
            v12 = RtWeakPtr<int>();

            typedef void (*fromOther)(RtWeakPtr<int>*, int);
            ((fromOther)(getActualOffset(0x10B59B8)))(&v12, a3);

            CanPlantTypeGoHere = newCanPlantTypeGoHere(a2, (int*)&v13, (int)&v12, 0xFFFFFFFF);

            typedef void (*dtor)(RtWeakPtr<int>*);
            ((dtor)(getActualOffset(0x10B58F8)))(&v12);

            if (CanPlantTypeGoHere)
            {
                v14 = v6;
                v13 = v7;
                v9 = (unsigned int*)a1[1];
                if ((unsigned int)v9 >= a1[2])
                {
                    typedef int (*sub_64AFE4)(int*, uint*);
                    ((sub_64AFE4)getActualOffset(0x64AFE4))(a1, &v13);
                }
                else
                {
                    *v9 = v7;
                    v9[1] = v6;
                    a1[1] = (int)(v9 + 2);
                }
            }
        } while (++v7 < board->m_columnCount);
    } while (++v6 < board->m_rowCount);
}

//typedef int (*fun_7207DC)(int, int);
//fun_7207DC oFun_7207DC = NULL;
//
//int hkFun_7207DC(int board, int mowerRow) {
//    return oFun_7207DC(board, mowerRow);
//
//    typedef int (*getDbInstance)();
//    getDbInstance getDbInstanceFun = (getDbInstance)getActualOffset(0x274CE4);
//    int pvzDbInstance = getDbInstanceFun();
//
//    typedef int* (*sub_DDCEB4)(int*, int, int);
//    int v14[5];
//    ((sub_DDCEB4)getActualOffset(0xDDCEB4))(v14, pvzDbInstance, 0x42);
//    
//    while (v14[2] != v14[4]) {
//        typedef RtWeakPtr<int>* (*sub_10C5EF8)(RtWeakPtr<int>*, int*);
//        RtWeakPtr<int> v12;
//        ((sub_10C5EF8)getActualOffset(0x10C5EF8))(&v12, v14);
//        RtWeakPtr<int> v13 = RtWeakPtr<int>();
//
//        typedef void (*fromOther)(RtWeakPtr<int>*, RtWeakPtr<int>*);
//        ((fromOther)(getActualOffset(0x10B59B8)))(&v13, &v12);
//
//        typedef int (*getRtWeakPtrRegistry)();
//        int v3 = ((getRtWeakPtrRegistry)getActualOffset(0x10C6764))();
//
//        typedef void (*sub_10CC81C)(RtWeakPtr<int>*, int, RtWeakPtr<int>*);
//        RtWeakPtr<int> v15;
//        ((sub_10CC81C)getActualOffset(0x10CC81C))(&v15, v3, &v13);
//
//        int mowerAddr = 0;
//        int v5 = 0;
//        if (!v15.is_invalid()) {
//            int typeId = v15.type_id();
//            
//            typedef int (*sub_10CC404)(int, int);
//            int* v7 = (int*) ((sub_10CC404)getActualOffset(0x10CC404))(v3, typeId);
//            if (v7) {
//                typedef int (*resolveWeakPtr)(int*, RtWeakPtr<int>*);
//                v5 = ((resolveWeakPtr)(getActualOffset(0x10C8780)))(v7, &v15);
//            }
//        }
//
//        typedef void (*dtor)(RtWeakPtr<int>*);
//        ((dtor)(getActualOffset(0x10B58F8)))(&v15);
//
//        if (v5) {
//            typedef int (*isType)(int, int);
//            isType isTypeCheck = *(isType*)(*(int*)v5 + 0x10);
//            // lawnmowerType = 
//        }
//    }
//}

typedef int (*fun10)(int, int);
fun10 oFun10 = NULL;

int hkFun10(int a1, int a2) {
    LOGI("FB1368 called %d %d", a1, a2);
    return oFun10(a1, a2);
}

typedef int (*fun11)(int);
fun11 oFun11 = NULL;

int hkFun11(int mower) {
    LOGI("Fun 11 called");
    return oFun11(mower);
}

typedef void (*mower_func_3)(int);
mower_func_3 oMowerFunc3 = NULL;

int firstMowerCalled = 0;

void hkMowerFunc3(int mower) {
    LOGI("mower func 3 called for %d og row = %d 2D0 = %d", mower, *(int*)(mower + 0x80), *(int*)((int)hookGetBoard() + 0x2D0));
    if (!firstMowerCalled) {
        firstMowerCalled = mower;
    }

    if (mower == firstMowerCalled && mower6Pointer != 0) {
        hkMowerFunc3((int)mower6Pointer);
    }
    else {
        oMowerFunc3(mower);
    }

    // oMowerFunc3(mower);
    // LOGI("after row = %d", *(int*)(mower + 0x80));
}

typedef void (*fun12)(int);
fun12 oFun12 = NULL;

void hkFun12(int a1) {
    LOGI("8D88A4 called");
    oFun12(a1);
}

typedef int (*fun13)(float*, int);
fun13 oFun13 = NULL;

int hkFun13(float* a1, int a2) {
    LOGI("fun 13 %d", a2);
    float v3 = a1[6];
    int v9[2];
    typedef void (*temp)(int*);
    (*(temp*)(*(uint*)a1 + 0x60))(v9);
    LOGI("access fun at address %u", getOriginalOffset(*(uint*)a1 + 0x60));
    float v4 = floorf((float)(v3 + -160.0) / 76.0);
    int v5 = 0;
    int v6 = 0;
    int v7 = (int)v4;
    return oFun13(a1, a2);
}

typedef int (*zombieSetSpawnPos)(int, SexyVector3*);
zombieSetSpawnPos oZombieSetSpawnPos = NULL;

int hkZombieSetSpawnPos(int a1, SexyVector3* a2) {
    LOGI("mX = %f mY = %f mZ = %f", a2->mX, a2->mY, a2->mZ);
    return oZombieSetSpawnPos(a1, a2);
}

typedef void (*fun80)(int);
fun80 oFun80 = NULL;

void hkFun80(int a1) {
    /*oFun80(a1);
    return;*/

    typedef void (*subFB2BB4)(int, uint8_t);
    subFB2BB4 makeMower = (subFB2BB4)getActualOffset(0xFB2BB4);
    for (uint8_t i = 0; i < getBoard()->m_rowCount; ++i) {
        makeMower(a1, i);
    }
    typedef int (*subFB2D18)(int);
    ((subFB2D18)getActualOffset(0xFB2D18))(a1);
}

typedef void (*fun81)(int, int);
fun81 oFun81 = NULL;

void hkFun81(int a1, int a2) { // 6F73E0
    LOGI("sandstorm code called!");
    oFun81(a1, a2);
}

typedef void* (*fun82)(int);
fun82 oFun82 = NULL;

void hkFun82(int a1) {
    LOGI("fun82 called");
    oFun82(a1);
}

typedef void (*fun84)(int, int, int, int, int, int*);
fun84 oFun84 = NULL;

void hkFun84(int a1, int a2, int a3, int a4, int a5, int* a6) {/*
    LOGI("fun access at %u", getOriginalOffset(*(uint*)a2 + 0x6C));
    LOGI("v37 fun access = %u", getOriginalOffset(*(uint*)a4 + 0x34));*/
    oFun84(a1, a2, a3, a4, a5, a6);
    //LOGI("finishes fun84");
    LOG_RETURN_ADDRESS(0);
}

typedef void (*fun86)(int*, int);
fun86 oFun86 = NULL;

void hkFun86(int* a1, int a2) {
    LOGI("Building symbols for SpawnZombiesJitteredInstruction");

    typedef int (*fun5)(int*, SexyString*);
    typedef int (*fun6)(int);
    typedef int (*fun7)(int*, int, int, int);
    typedef void (*registerProperty)(int*, int, SexyString*, int, int);
    typedef void (*registerProperty2)(int*, int, const char*, int, int);
    typedef int (*getPropertyOfType)(int*, int, int);

    registerProperty funRegisterProps = *(registerProperty*)(*a1 + 0x34);
    registerProperty2 funRegisterProps2 = *(registerProperty2*)(*a1 + 0x34);
    getPropertyOfType funGetPropType = *(getPropertyOfType*)(*a1 + 0x1C);

    //fun5 v5 = *(fun5*)(*a1 + 0x2C);
    //SexyString v15 = "ZombieBullProps"; // parent class
    //int v6 = v5(a1, &v15);
    //int v7 = (*(fun6*)(*(uint*)v6 + 0x34))(v6);
    //(*(fun7*)(*a1 + 0x30))(a1, a2, v7, 0);

    SexyString type = "RtWeakPtr<ZombieType>";
    typedef int (*getTemplate)(int*, SexyString*);
    getTemplate getTemplateFunc = (getTemplate)getActualOffset(0x294138);
    int v5 = getTemplateFunc(a1, &type);
    funRegisterProps2(a1, a2, "\bType", 0u, v5);

    type = "RowSpawnInstruction::RowSpawnInstruction";
    typedef int (*getTypeRef)(int*, SexyString*);
    getTypeRef funGetTypeRef = (getTypeRef)getActualOffset(0x121F8A0);
    //int typeRef = funGetTypeRef(a1, &type); // all this does is allowing for row 6 to be read from the files
    int typeInt = funGetPropType(a1, 6, 4u);
    funRegisterProps2(a1, a2, "\x06Row", 8u, typeInt);

    SexyString v14 = "FriendZombie";
    int v11 = funGetPropType(a1, 3, 1u);
    funRegisterProps(a1, a2, &v14, 0xCu, v11);

    LOGI("Symbols built");
}

typedef int (*fun89)(int, int, int);
fun89 oFun89 = NULL;

int hkFun89(int a1, int rowCount, int row) {
    LOGI("fun89 called %d %d %d", a1, rowCount, row);
    return oFun89(a1, rowCount, row);
}

typedef int (*getLowTideRow)(int*, int);
getLowTideRow oGetLowTideRow = NULL;

int hkGetLowTideRow(int* a1, int a2) {
    a1[9] = 0x7F7FFFFF;
    a1[0xA] = 0x7F7FFFFF;
    a1[0xB] = 0;
    typedef int (*sub112D078)(int);
    int res = ((sub112D078)getActualOffset(0x112D078))(a2) % getBoard()->m_rowCount;
    a1[0xC] = res; // this sets the random row for the first zombie to spawn in the ambush
    return res;
}

typedef void (*fun90)(int, int);
fun90 oFun90 = NULL;

void hkFun90(int a1, int a2) {
    LOGI("90 is called to get all the zombies");
    LOG_RETURN_ADDRESS(0);
    oFun90(a1, a2);
}

typedef void (*fun95)(int);
fun95 oFun95 = NULL;

bool fromFun95;
int cntFrom95 = 0;
void hkFun95(int a1) {
    fromFun95 = true;
    cntFrom95 = 0;
    oFun95(a1);
    fromFun95 = false;
}

typedef int (*fun96)(int, SexyVector3*, int);
fun96 oFun96 = NULL;

int hkFun96(int a1, SexyVector3* a2, int a3) {
    if (fromFun95) {
        ++cntFrom95;
        if (cntFrom95 == 2 && getBoard()->m_rowCount == 6) {
            // add to the origin
            a2->mY += GRID_TILE_HEIGHT;
        }
    }
    return oFun96(a1, a2, a3);
}

typedef bool (*subA9E7F0)(int, int);
subA9E7F0 oSubA9E7F0 = NULL;

bool hkSubA9E7F0(int revivalSubsystem, int packetCursor) {
    if (packetCursor) {
        typedef int (*getPacketCursorType)();
        int packetCursorType = ((getPacketCursorType)getActualOffset(0x929E00))();
        typedef int (*isType)(int, int);
        isType isTypeCheck = *(isType*)(*(int*)packetCursor + 0x10);
        if (!isTypeCheck(packetCursor, packetCursorType)) {
            return false;
        }
        RtWeakPtr<int> v7 = RtWeakPtr<int>();
        v7.FromOther((RtWeakPtr<int>*)(packetCursor + 0x28));
        if (!v7.is_valid()) {
            return false;
        }
        int seedPacket = (int) v7.get();
        RtWeakPtr<int> v36 = RtWeakPtr<int>();
        v36.FromOther((RtWeakPtr<int>*)(seedPacket + 0x110));
        if (!v36.is_valid()) {
            return false;
        }

        int plantType = (int)v36.get();
        typedef int (*getTypeIntensiveCarrot)();
        int intensiveCarrotType = ((getTypeIntensiveCarrot)getActualOffset(0xAA0738))();
        isType isTypeCheck2 = *(isType*)(*(int*)plantType + 0x10);
        return isTypeCheck2(plantType, intensiveCarrotType);
    }
    return false;
}

typedef int (*fun110)(int);
fun110 oFun110 = NULL;

int hkFun110(int a1) {
    fromCreateProj = true;
    LOGI("plant create proj");
    // LOGI("the vtable at 0x64 is %u", getOriginalOffset(**(uint**)(a1 + 0x64)));
    int res = oFun110(a1);
    fromCreateProj = false;
    LOG_RETURN_ADDRESS(0);
    return res;
}

typedef void (*updateSinkDist)(int, int);
void hkUpdateSinkDist(int a1, int a2) {
    float mX = *(float*)(a1 + 0x14);
    int gridX = (int)((mX - 0xE8) / 64.0);
    if (gridX < 5) {
        *(float*)(a1 + 0x1C) = -(552.0 - mX) * 10.0 / 64.0;
    }
}

typedef int (*renderPopAnim)(int, SexyVector3*, int);
renderPopAnim oRenderPopAnim = NULL;

int hkRenderPopAnim(int a1, SexyVector3* a2, int a3) {
    LOGI("Popanim rendered at %f %f %f", a2->mX, a2->mY, a2->mZ);
    LOGI("Popanim vftable = %u", getOriginalOffset(*(uint*)a1));
    int gridX = (int)(a2->mX + 98 - 0xE8) / 64.0;
    if (gridX < 5) {
        a2->mZ -= (5 - gridX) * 10;
    }
    return oRenderPopAnim(a1, a2, a3);
}

typedef int (*fun170)(int, int);
fun170 oFun170 = NULL;

int hkFun170(int a1, int a2) {
    LOGI("this shit no crash");
    LOGI("vftable 1 = %u 2 = %u", getOriginalOffset(*(uint*)a1), getOriginalOffset(*(uint*)a2));
    RtWeakPtr<int> contents;
    contents.FromOther((RtWeakPtr<int>*)(a1 + 0xF8));
    int content = (int)contents.get();
    LOGI("content vftable = %u", getOriginalOffset(*(uint*)content)); // it's either a PlantType or ZombieType
    return oFun170(a1, a2);
}

typedef void (*fun171)(PlantGroup*, PlantConditions, float, float);
fun171 oFun171 = NULL;

void disableWithCond(PlantGroup *plantGroup, PlantConditions plantCond, float duration, float eventTime) {
    LOGI("Disable with cond called condition %d duration %f event %f", plantCond, duration, eventTime);
    oFun171(plantGroup, plantCond, duration, eventTime);
    LOGI("Disable with cond finished");
}

typedef void (*fun172)(Plant*, PlantConditions);
fun172 oFun172 = NULL;

void applyEffectsForCond(Plant* a1, PlantConditions a2) { // this function handles spawning the GIs apparently
    // LOGI("vftable attached entities is %u", getOriginalOffset(*(uint*)(a1 + 0x1C8)));
    LOGI("Apply condition %d", a2);
    if (a2 == plant_sheeped) {
        LOGI("a2 is sheeped");
        // return;
        //RtWeakPtr<int> props = RtWeakPtr<int>();
        //props.FromOther((RtWeakPtr<int>*)(a1 + 0x108));
        //int propsClass = (int)props.get();
        ////std::vector<PlantAction>
        //std::vector<PlantAction>* plantActions = (std::vector<PlantAction>*)(propsClass + 0x68);
        //if (plantActions->empty()) {
        //    return;
        //}

        
        /*RtWeakPtr<int> tmp = RtWeakPtr<int>();
        tmp.FromOther((RtWeakPtr<int>*)(a1 + 0xE8));
        int animRig = (int)tmp.get();
        LOGI("got rig");

        typedef void (*setModifier)(int, float);
        setModifier func = *(setModifier*)(*(uint*)animRig + 0x38);
        LOGI("got func");
        func(animRig, 0.5);
        *(float*)(a1 + 0x8C) = 0.5;*/
        
        /*TimeMgr* mgr = TimeMgr::GetInstance();
        SexyVector3 curLoc = a1->m_position;
        a1->m_relocateFromX = curLoc.mX;
        a1->m_relocateFromY = curLoc.mY;
        a1->m_relocateToX = curLoc.mX + 64;
        a1->m_relocateToY = curLoc.mY;
        a1->m_relocateApexHeight = 76;
        a1->m_relocationType = 1;
        a1->m_relocateStartTime = mgr->m_curTime;
        a1->m_relocateEndTime = mgr->m_curTime + 1.0;
        a1->m_relocating = true;*/
    }
    
    oFun172(a1, a2);
}

typedef void (*fun178)(int, int);
fun178 oFun178 = NULL;

void removeEffectsForCond(int a1, int a2) {
    if (a2 == 0x1A) {
        RtWeakPtr<int> tmp = RtWeakPtr<int>();
        tmp.FromOther((RtWeakPtr<int>*)(a1 + 0xE8));
        int animRig = (int)tmp.get();

        typedef void (*setModifier)(int, float);
        setModifier func = *(setModifier*)(*(uint*)animRig + 0x38);
        func(animRig, 1.0);
        *(float*)(a1 + 0x8C) = 1.0;
        return;
    }
    else {
        oFun178(a1, a2);
    }
}

typedef void (*fun173)(int);
fun173 oFun173 = NULL;

void hkFun173(int a1) {
    LOGI("plant framework play attack anim %u", getOriginalOffset(*(uint*)a1));
    LOG_RETURN_ADDRESS(0);
    // oFun173(a1);
}

typedef void (*fun174)(int, int, float, float, float);
fun174 oFun174 = NULL;

void hkFun174(int plant, int cond, float duration, float eventTime, float data) {
    LOGI("apply condition %d with duration %f eventtime %f", cond, duration, eventTime);
    oFun174(plant, cond, duration, eventTime, data);
}

typedef int (*fun175)(int, int);
fun175 oFun175 = NULL;

int hkFun175(int plant, int a2) {
    LOGI("unkint for animrig vtable = %u animrig = %u", getOriginalOffset(*(uint*)(plant + 0x18)), getOriginalOffset(*(uint*)plant));
    LOG_RETURN_ADDRESS(0);
    return oFun175(plant, a2);
}

typedef int (*fun176)(int, int);
fun176 oFun176 = NULL;

int hkFun176(int a1, int a2) {
    LOGI("fun176 called");
    LOG_RETURN_ADDRESS(0);
    return oFun176(a1, a2);
}

typedef float (*getLaunchIntervalScaling)(int);
getLaunchIntervalScaling oGetLaunchIntervalScaling = NULL;

float hkGetLaunchIntervalScaling(int a1) {
    return 0.5;
}

typedef void (*fun202)(int, RtWeakPtr<PlantGroup>*);
fun202 oFun202 = NULL;

void hkFun202(int a1, RtWeakPtr<PlantGroup>* a2) {
    LOGI("Fun 202 called");
    return;
}

typedef void (*subFE4E4C)(int, Zombie*, int, char);
subFE4E4C oSub205 = NULL;

void hkSub205(int a1, Zombie* a2, int a3, char a4) {
    // LOGI("a1 vftable = %u", getOriginalOffset(*(uint*)a1));
    oSub205(a1, a2, a3, a4); // any effect will cut off at the ground
    // LOGI("zombie flags = %u", a2->m_zombieFlags);
    return;
}

typedef void (*sub206)(Zombie*, float*);
sub206 oSub206 = NULL;

void hkSub206(Zombie* a1, float* a2) {
    LOGI("shadow render called");
    LOGI("a2 vtable = %u", getOriginalOffset(*(uint*)a2));
    LOG_RETURN_ADDRESS(0);
    return;
}

typedef void (*fun207)(Board*);
fun207 oFun207 = NULL;

void hkFun207(Board* b) {
    if (b->m_boardState == 7) {
        b->m_boardState = 3;
    }
    else {
        oFun207(b);
    }
}

struct UnkDrawStruct {
    float x;
    float y;
    int unkInt;
    float unkFloat;
    int unkColorInt;
    int unkZeros[3];
};

void DrawRect(int renderDevice, CornerRect* rect, Color* color, int a4) {
    typedef bool (*sub134F42C)(int);
    if (((sub134F42C)getActualOffset(0x134F42C))(renderDevice) && color->alpha >= 6) {
        int v9 = renderDevice + 0x806C;
        int v8 = *(int*)v9;
        int v10 = *(int*)(v8 + 0x14C);
        int v11 = *(int*)(v8 + 0x1AC);
        int v12 = *(int*)(*(int*)(v11 + 0x60) + 0x20);
        int v13 = *(int*)(*(int*)(v11 + 0x6C) + 0x20);

        int unk1[5];
        memset(unk1, 0, sizeof(unk1));

        if (v12 == 0xFFFF) {
            v12 = 2;
        }
        unk1[1] = v12;
        typedef void (*sub13B1F68)(int, int*);
        ((sub13B1F68)getActualOffset(0x13B1F68))(v10 + 0x9CC, unk1);

        memset(unk1, 0, sizeof(unk1));
        int v15 = 2;
        if (!a4) v15 = 6;
        if (v13 != 0xFFFF) {
            v15 = v13;
        }
        unk1[1] = v15;
        ((sub13B1F68)getActualOffset(0x13B1F68))(v10 + 0xA50, unk1);

        float unkOffset = *(float*)(renderDevice + 4);
        int v23 = *(int*)(*(int*)(v11 + 0x78) + 0x20);

        uint32_t R0 = (color->blue << 16) & 0xFFFF0000;
        uint32_t v25 = (color->red & 0xFF) | ((color->green & 0xFF) << 8);
        R0 &= 0x00FFFFFF;
        R0 |= (color->alpha << 24);
        R0 |= v25;

        UnkDrawStruct unkStruct[4];
        for (int i = 0; i < 4; ++i) {
            memset(unkStruct[i].unkZeros, 0, sizeof(unkStruct[i].unkZeros));
        }

        unkStruct[0].unkFloat = 1.0;
        unkStruct[0].unkColorInt = R0;
        unkStruct[0].unkInt = v23;

        for (int i = 1; i < 4; ++i) {
            unkStruct[i].unkFloat = unkStruct[0].unkFloat;
            unkStruct[i].unkInt = unkStruct[0].unkInt;
            unkStruct[i].unkColorInt = unkStruct[0].unkColorInt;
        }
        // LOGI("unkOffset = %f", unkOffset);
        /*unkStruct[0].x = unkOffset + rect->mX;
        unkStruct[0].y = unkOffset + rect->mY;

        unkStruct[1].x = unkOffset + rect->mX2;
        unkStruct[1].y = unkOffset + rect->mY;

        unkStruct[2].x = unkOffset + rect->mX;
        unkStruct[2].y = unkOffset + rect->mY2;
        
        unkStruct[3].x = unkOffset + rect->mX2;
        unkStruct[3].y = unkOffset + rect->mY2;*/

        int v33 = *(int*)(renderDevice + 0x48);
        if (*(int*)(renderDevice + 0x44) != v33) {
            FPoint vertices[4];
            vertices[0] = rect->topLeft;
            vertices[1] = rect->bottomLeft;
            vertices[2] = rect->topRight;
            vertices[3] = rect->bottomRight;

            for (int i = 0; i < 4; ++i) {
                vertices[i].x += unkOffset;
                vertices[i].y += unkOffset;
            }

            for (int i = 0; i < 4; ++i) {
                float v42[2];
                typedef void (*sub10AB3E8)(float*, int, FPoint*);
                ((sub10AB3E8)getActualOffset(0x10AB3E8))(v42, v33 - 0x24, &vertices[i]);
                vertices[i].x = unkOffset + v42[0];
                vertices[i].y = unkOffset + v42[1];
                unkStruct[i].x = vertices[i].x;
                unkStruct[i].y = vertices[i].y;
            }
        }

        typedef void (*sub134F4D0)(int, int, int, int, float, float);
        ((sub134F4D0)getActualOffset(0x134F4D0))(renderDevice, 0, 0, 0xDE1, 1.0, 1.0);
        typedef void (*sub134F594)(int, int, int, UnkDrawStruct*, int, int);
        ((sub134F594)getActualOffset(0x134F594))(renderDevice, 5, 2, unkStruct, *(int*)(renderDevice + 0x28), *(int*)(renderDevice + 0x2C));
    }
}

typedef void (*drawCursor)(Board*, int graphicsState, int gridX, int gridY);
drawCursor oDrawCursor = NULL;
bool fromDrawCursor = false;

void graphicsFillCornerRect(Graphics* graphics, CornerRect* rect);

void GetCornerRect(CornerRect* res, int x, int y, int width, int height) {
    res->topLeft = FPoint(x, y);
    res->topRight = FPoint(x + width, y);
    res->bottomLeft = FPoint(x, y + height);
    res->bottomRight = FPoint(x + width, y + height);
}

void hkDrawCursor(Board* board, Graphics* graphics, int gridX, int gridY) {
    // LOGI("graphics state 0x50 vtbl = %x", getOriginalOffset(**(uint**)(graphics + 0x50)));
    fromDrawCursor = true;
    // oDrawCursor(board, graphics, gridX, gridY);
    /*typedef void (*sub1124614)(int);
    ((sub1124614)getActualOffset(0x1124614))(graphics);*/
    graphics->UnkFunc();

    graphics->mTransX = (float)*(int*)((uint)board + 0x24);
    graphics->mTransY = (float)*(int*)((uint)board + 0x28);

    auto sexyApp = SexyApp::Get();
    int v9 = sexyApp->Scale(0xC8); // 200, left edge of the board

    int v11 = (int)(graphics->mScaleOrigX + floorf((v9 - graphics->mScaleOrigX) * graphics->mScaleX));
    int v12 = sexyApp->Scale(0xA0); // 160, top edge of the board
    int v13 = (int)(graphics->mScaleOrigY + floorf((v12 - graphics->mScaleOrigY) * graphics->mScaleY));

    Color fillColor = Color(0xFF, 0xFF, 0xFF, 0x95);
    graphics->mColor = fillColor;

    int v19 = (int)(sexyApp->Scale(gridX * GRID_TILE_WIDTH * graphics->mScaleX) + v11);
    int v20 = (int)sexyApp->Scale(GRID_TILE_WIDTH);
    int v22 = (int)sexyApp->Scale(board->m_rowCount * GRID_TILE_HEIGHT);

    typedef void (*fillRect)(int, int, int, int, int);
    fillRect pFillRect = (fillRect)getActualOffset(0x1124920);
    // pFillRect(graphics, v19, v13, int(v20 * scaleX), int(v22 * scaleY)); // this draws the column
    CornerRect rect1; // vertical rect
    GetCornerRect(&rect1, v19, v13, v20 * graphics->mScaleX, v22 * graphics->mScaleY);

    auto roofStage = RoofStage::Get();
    int heights[board->m_columnCount + 1];
    memset(heights, 0, sizeof(heights));
    if (roofStage) {
        roofStage->GetHeights(heights);
        for (int i = 0; i <= board->m_columnCount + 1; ++i) {
            heights[i] = -heights[i]; // slight jank since y goes from top to bottom
            heights[i] = sexyApp->Scale(heights[i]) * graphics->mScaleY;
        }
    }
    
    float leftYOffset = 0;
    float rightYOffset = 0;
    /*
    if (gridX < 5) {
        leftYOffset = sexyApp->Scale(16 * (5 - gridX)) * graphics->mScaleY;
        rightYOffset = sexyApp->Scale(16 * (4 - gridX)) * graphics->mScaleY;
    }
    */
    leftYOffset = heights[gridX];
    rightYOffset = heights[gridX + 1];

    rect1.topLeft.y += leftYOffset;
    rect1.bottomLeft.y += leftYOffset;
    rect1.topRight.y += rightYOffset;
    rect1.bottomRight.y += rightYOffset;

    graphicsFillCornerRect(graphics, &rect1);

    int v23 = (int)(sexyApp->Scale(gridY * GRID_TILE_HEIGHT * graphics->mScaleY) + v13);
    int v24 = (int)sexyApp->Scale(board->m_columnCount * GRID_TILE_WIDTH);
    int v26 = (int)sexyApp->Scale(GRID_TILE_HEIGHT);
    // pFillRect(graphics, v11, v23, int(v24 * scaleX), int(v26 * scaleY));

    for (int i = 0; i < board->m_columnCount; ++i) {
        GetCornerRect(&rect1, v11 + v20 * graphics->mScaleX * i, v23, v20 * graphics->mScaleX, v26 * graphics->mScaleY);

        leftYOffset = heights[i];
        rightYOffset = heights[i + 1];
        /*
        leftYOffset = 0;
        rightYOffset = 0;
        if (i < 5) {
            leftYOffset = sexyApp->Scale(16 * (5 - i)) * graphics->mScaleY;
            rightYOffset = sexyApp->Scale(16 * (4 - i)) * graphics->mScaleY;
        }
        */
        rect1.topLeft.y += leftYOffset;
        rect1.bottomLeft.y += leftYOffset;
        rect1.topRight.y += rightYOffset;
        rect1.bottomRight.y += rightYOffset;
        graphicsFillCornerRect(graphics, &rect1);
    }

    /*
    GetCornerRect(&rect1, v11, v23, v24 * scaleX, v26 * scaleY);
    graphicsFillCornerRect(graphics, &rect1);
    */
    graphics->UnkFunc2();
    fromDrawCursor = false;
}

void graphicsFillCornerRect(Graphics* graphics, CornerRect* rect) {
    // Only use this for drawing planting highlights

    int alpha = graphics->mColor.alpha;
    if (alpha) {
        int renderDevice = graphics->mRenderDevice;
        /*
        typedef void (*renderDeviceFun)(int, int);
        renderDeviceFun pFun = *(renderDeviceFun*)(*(uint*)renderDevice + 0x18);
        pFun(renderDevice, (uint)graphics + 0x54);
        */
        CallVirtualFunc<void, int, int*>((void*)renderDevice, 6, renderDevice, &graphics->mRenderContext);

        FPoint* points[] = {&rect->topLeft, &rect->topRight, &rect->bottomLeft, &rect->bottomRight};
        for (int i = 0; i < 4; ++i) {
            points[i]->x += graphics->mTransX;
            points[i]->y += graphics->mTransY;
        }

        // TODO: Decode Rect::Intersection
        DrawRect(renderDevice, rect, & graphics->mColor, graphics->mDrawMode); // 0x44 is drawMode
    }
}

typedef void (*renderRect)(int, Rect*, Color*, int);
renderRect oRenderRect = NULL;

void hkRenderRect(int a1, Rect* a2, Color* a3, int a4) {
    /*if (fromDrawCursor) {
        LOGI("Rect = %d %d %d %d", a2->mX, a2->mY, a2->mX + a2->mWidth, a2->mY + a2->mHeight);
    }*/
    if (!fromDrawCursor) {
        oRenderRect(a1, a2, a3, a4);
    }
    else {
        auto rect = CornerRect(FPoint(a2->mX, a2->mY), FPoint(a2->mX + a2->mWidth, a2->mY),
            FPoint(a2->mX, a2->mY + a2->mHeight), FPoint(a2->mX + a2->mWidth, a2->mY + a2->mHeight));
        DrawRect(a1, &rect, a3, a4);
    }
}

typedef void (*fun290)(int, int, int, UnkDrawStruct*, int, int);
fun290 oFun290 = NULL;

void hkFun290(int renderDevice, int unk1, int unk2, UnkDrawStruct* a4, int a5, int a6) {
    /*if (fromDrawCursor) {
        for (int i = 0; i < 4; ++i) {
            UnkDrawStruct tmp = a4[i];
            LOGI("tmp point = %f %f", tmp.x, tmp.y);
        }
    }*/
    oFun290(renderDevice, unk1, unk2, a4, a5, a6);
}

typedef int (*fun310)(int);
fun310 oFun310 = NULL;

int hkFun310(int a1) {
    LOG_RETURN_ADDRESS(0);
    *(int*)(a1 + 0x19C) = 1;
    return oFun310(a1);
}

std::mt19937 mt_rand2; // Optionally, you can seed the generator here or in a function
std::uniform_real_distribution<float> floatDist(0.0, 1.0);

typedef bool (*zombossFireActionDefGetTarget)(ZombossFireActionDefinition* def, ZombieZombossMech* zomboss, std::vector<Point>* res);
zombossFireActionDefGetTarget oZombossFireActionDefGetTarget = NULL;

bool hkZombossFireActionDefGetTarget(ZombossFireActionDefinition* def, ZombieZombossMech* zomboss, std::vector<Point>* res) {
    LOGI("Custom get target code");
    int gridCoords[2];
    zomboss->GetGridCoords(gridCoords);

    Rect targetRect = Rect(def->FireMinColumn, gridCoords[1] - 1 - def->FireRowRange, def->FireMaxColumn + 1, 2 * def->FireRowRange + 2);

    std::vector<TargetingType> targetTypes;
    if (def->FireTargetEmptyWeight >= 1) {
        TargetingType targetType;
        targetType.m_type = 0;
        targetType.m_weight = def->FireTargetEmptyWeight;
        targetType.m_checkTarget = CreateDelegate(
            0x1BC52C4,
            getActualOffset(0x3AECB0),
            0, 0, 0,
            getActualOffset(0x3B2708)
        );

        targetTypes.push_back(targetType);
    }

    if (def->FireTargetPlantWeight >= 1) {
        TargetingType targetType;
        targetType.m_type = 1;
        targetType.m_weight = def->FireTargetPlantWeight;
        targetType.m_checkTarget = CreateDelegate(
            0x1BC52C4,
            getActualOffset(0x3AED84),
            0, 0, 0,
            getActualOffset(0x3B2708)
        );

        targetTypes.push_back(targetType);
    }

    if (def->FireTargetRailsWeight >= 1) {
        TargetingType targetType;
        targetType.m_type = 2;
        targetType.m_weight = def->FireTargetRailsWeight;
        targetType.m_checkTarget = CreateDelegate(
            0x1BC52C4,
            getActualOffset(0x3AEE58),
            0, 0, 0,
            getActualOffset(0x3B2708)
        );

        targetTypes.push_back(targetType);
    }

    if (def->FireTargetTilesWeight >= 1) {
        TargetingType targetType;
        targetType.m_type = 3;
        targetType.m_weight = def->FireTargetTilesWeight;
        targetType.m_checkTarget = CreateDelegate(
            0x1BC52C4,
            getActualOffset(0x3AEE94),
            0, 0, 0,
            getActualOffset(0x3B2708)
        );
        
        targetTypes.push_back(targetType);
    }

    LOGI("Fire weights = %d %d %d %d", def->FireTargetEmptyWeight, def->FireTargetPlantWeight, def->FireTargetRailsWeight, def->FireTargetTilesWeight);

    WeightedVector chooser;

    int idx = 0;
    for (auto& targetType : targetTypes) {
        def->GetTargetsForType(
            zomboss,
            &targetRect,
            &targetType.m_targets,
            &targetType.m_checkTarget
        );
        if (!targetType.m_targets.empty()) {
            chooser.Add(idx, targetType.m_weight);
            // LOGI("Chooser added type %d with weight %d", targetType.m_type, targetType.m_weight);
        }
        ++idx;
    }

    int rocketsToFire = def->FireRocketCount.Min + floatDist(mt_rand2) * (def->FireRocketCount.Max - def->FireRocketCount.Min);

    if (!chooser.WeightedVec.empty()) {
        typedef int (*chooseWeightedIndex)(WeightedVector*);
        int chosenIdx = ((chooseWeightedIndex)getActualOffset(0x3AF080))(&chooser);

        /*
        for (int i = 0; i < targetTypes.size(); ++i) {
            if (targetTypes[i].m_type == 2) {
                chosenIdx = i;
            }
        }
        */

        auto zombossRig = zomboss->m_animRig.get();
        ZombieAnimRig_ZombossMech_Future* futureMechRig = NULL;
        if (zombossRig->IsType(ZombieAnimRig_ZombossMech_Future::GetRTClass())) {
            futureMechRig = (ZombieAnimRig_ZombossMech_Future*)zombossRig;
            futureMechRig->m_queuedTileClass = -1;
        }

        if (targetTypes[chosenIdx].m_type == 2) {
            Point randomRailCell = targetTypes[chosenIdx].m_targets[floatDist(mt_rand2) * (targetTypes[chosenIdx].m_targets.size() - 1)];

            typedef void (*sub3AF0E8)(ZombossFireActionDefinition* def, std::vector<Point>* res, Point*);
            ((sub3AF0E8)getActualOffset(0x3AF0E8))(def, res, &randomRailCell);
        }
        else if (targetTypes[chosenIdx].m_type == 3) {
            LOGI("Chose to attack power tiles");
            std::vector<PowerTileClass> tileClasses;
            for (auto& point : targetTypes[chosenIdx].m_targets) {
                auto powerTile = GetPowerTileAt(point.x, point.y);
                auto propPtr = powerTile->props;
                auto props = (GridItemPowerTileProps*) propPtr.get();
                tileClasses.push_back(props->TileClass);
            }
            // this is weighting tile frequency
            int tileClassToAttack = tileClasses[floatDist(mt_rand2) * (tileClasses.size() - 1)];
            
            if (futureMechRig) {
                futureMechRig->m_queuedTileClass = tileClassToAttack;
            }

            for (auto& point : targetTypes[chosenIdx].m_targets) {
                auto powerTile = GetPowerTileAt(point.x, point.y);
                auto propPtr = powerTile->props;
                auto props = (GridItemPowerTileProps*)propPtr.get();

                if (props->TileClass == tileClassToAttack) {
                    LOGI("Found tile to attack");
                    res->push_back(point);
                }
            }
        } else {
            def->GetAmountTargets(zomboss, rocketsToFire, &targetTypes[chosenIdx].m_targets, res);
        }
    }

    return !res->empty();
}

#pragma endregion

// provides the getBoard() implementation for other files
Board* getBoard() {
    return hookGetBoard();
}

Board* GetBoard() {
    return hookGetBoard();
}

int getWeakPtr(int addr) {
    RtWeakPtr<int> a = RtWeakPtr<int>();
    a.FromOther((RtWeakPtr<int>*)addr);
    return (int) a.get();
}

__attribute__((constructor))
// This is automatically executed when the lib is loaded
// Run your initialization code here
void libPVZ2ExampleMod_main()
{
	LOGI("Initializing %s", LIB_TAG);
    // New, easier to manage way of adding typenames to the plant/zombie name mapper
    REGISTER_PLANT_TYPENAME("funny_tomato");

    REGISTER_ZOMBIE_TYPENAME("steam_mechanic2");

    // Function hooks
    PVZ2HookFunction(0x105123C, (void*)hkCreateZombieTypenameMap, (void**)&oZombieAlmanacCtor, "ZombieAlmanac::ZombieAlamanc");
    PVZ2HookFunction(0xD994B8, (void*)hkCreatePlantNameMapper, (void**)&oPlantNameMapperCtor, "PlantNameMapper::PlantNameMapper");
    PVZ2HookFunction(0x7942BC, (void*)hkCamelZombieFunc, (void**)&oCamelZombieFunc, "CamelZombie::vftable_func_0xEC");
    PVZ2HookFunction(0x890108, (void*)hkInitZombiePianoList, (void**)&oInitZombiePianoList, "ZombiePiano::getTypenameList");
    PVZ2HookFunction(0x127415C, (void*)HkReinitForSurfaceChange, (void**)&oRFSC, "ReinitForSurfaceChanged");
    PVZ2HookFunction(0x71A004, (void*)hkBoardCtor, (void**)&oBoardCtor, "Board::Board");
    PVZ2HookFunction(0x884B6C, (void*)hkMagicianHealerImmuneToShrink, (void**)&dispose, "ZombieCarnieMagician::IsImmuneToShrink");
    PVZ2HookFunction(0x881C4C, (void*)hkMagicianHealerImmuneToShrink, (void**)&dispose, "ZombieRomanHealer::IsImmuneToShrink");
    PVZ2HookFunction(0x86CCAC, (void*)hkMagicianHealerConditionFunc, (void**)&dispose, "ZombieCarnieMagician::ConditionFunc");
    PVZ2HookFunction(0x84EAA0, (void*)hkMagicianHealerConditionFunc, (void**)&dispose, "ZombieRomanHealer::ConditionFunc");
    PVZ2HookFunction(0x86CCC0, (void*)hkMagicianInitializeFamilyImmunities, (void**)&dispose, "ZombieRomanHealer::InitializeFamilyImmunities");

    // BoardPlanting__CanPlantAt	.text	00649FF4	000003D0	00000048	00000004	R	.	.	.	.	.	B	T	.
    // int __fastcall BoardPlanting::CanPlantAt(int r0_0, int *a2, RtWeakPtr *a3, int a4, int a5)
    // a2 is column, a2[1] is row
    // 14 is can't plant on graves, 50 is can't plant on another plant\
    // 0 is can plant. what if i permanently returns 0?
//     function is not even called when im outside of the main lawn!!!
//     a4 is 0 all the time. a5 is -1 sometimes and 0 most times??? -1 is before it's added to the lawn?
    // 
    // BoardPlanting__AddPlantAt	.text	00649784	0000020C	00000050	00000004	R	.	.	.	.	.	B	T	.
    // int __fastcall BoardPlanting::AddPlantAt(int r0_0, int a2, int a3, RtWeakPtr *a4, unsigned __int8 *a5)
    // a2 is column and a3 is row, 0-indexed

    // BoardPlanting__CanPlantTypeGoHere	.text	006490B8	00000088	00000030		R	.	.	.	.	.	B	T	.
    // int __fastcall BoardPlanting::CanPlantTypeGoHere(int r0_0, int *a2, int a3, int a4)

    PVZ2HookFunction(0x952864, (void*)hookGetBoard, (void**)&oGetBoard, "GetBoard");

#define SOTRUE
#ifdef SOTRUE
    PVZ2HookFunction(0x649FF4, (void*)newCanPlantAt, (void**)&oCanPlantAt, "BoardPlanting::CanPlantAt");
    PVZ2HookFunction(0x649784, (void*)newAddPlantAt, (void**)&oAddPlantAt, "BoardPlanting::AddPlantAt");
    PVZ2HookFunction(0x6490B8, (void*)newCanPlantTypeGoHere, (void**)&oCanPlantTypeGoHere, "BoardPlanting::CanPlantTypeGoHere");
    // PVZ2HookFunction(0xCF3DA8, (void*)hookFun_CF3DA8, (void**)&oFun_CF3DA8, "PlantCursor::sub_CF3DA8");
    // PVZ2HookFunction(0xCF3BE4, (void*)hookPlantCursorGetGridCoords, (void**)&oPlantCursorGetGridCoords, "PlantCursor::sub_CF3BE4");
    PVZ2HookFunction(0x640534, (void*)hookPosYToGridY, (void**)&oPosYToGridY, "Board::PosYToGridY");
    PVZ2HookFunction(0x6406A4, (void*)hookIsValidGridCoordinates, (void**)&oIsValidGridCoordinates, "BoardUtil::IsValidGridCoordinates");

    // Next problem: projectiles don't really have collision in 6th lane? Zombies also ignore plants entirely
    // 
    // projectile collision can be fixed with proper boardRows

    // PVZ2HookFunction(0x913424, (void*)hookProjectileCollideWithObject, (void**)&oProjectileCollideWithObject, "Projectile::CollideWithObject");
    // PVZ2HookFunction(0x91395C, (void*)hookProjectileAffectTarget, (void**)&oProjectileAffectTarget, "Projectile::AffectTarget");
    PVZ2HookFunction(0xE83AB0, (void*)hookPlantIsOnValidBoardPosition, (void**)&oPlantIsOnValidBoardPosition, "Plant::IsOnValidBoardPosition");
    // THE FUNCTION ABOVE IS NOT YET PORTED TO 9.6

    // PVZ2HookFunction(0x916ECC, (void*)hookProjectileFindCollisionTargets, (void**)&oProjectileFindCollisionTargets, "Projectile::FindCollisionTargets");
    // PVZ2HookFunction(0x72B08C, (void*)hookBoardAddProjectileByType, (void**)&oBoardAddProjectileByType, "Board::AddProjectileByType");
    // PVZ2HookFunction(0x694E04, (void*)hookFun_694E04, (void**)&oFun_694E04, "sub_694E04");
    // PVZ2HookFunction(0x9112CC, (void*)hookFun_9112CC, (void**)&oFun_9112CC, "sub_9112CC");
    // PVZ2HookFunction(0x915F10, (void*)hookFun_915F10, (void**)&oFun_915F10, "sub_915F10");
    // PVZ2HookFunction(0x915C80, (void*)hookFun_915C80, (void**)&oFun_915C80, "sub_915C80");

    // projectile fix
    PVZ2HookFunction(0x640618, (void*)hookYPosToGridRowKeepOnBoard, (void**)&oYPosToGridRowKeepOnBoard, "YPosToGridRowKeepOnBoard");


    // bug: can't plant at (0, 0) for some reason. planting reason 1? No error message associated

    // okay now fix zombies interacting with plants
    // PVZ2HookFunction(0x72B4D8, (void*)hookAddZombieByType, (void**)&oAddZombieByType, "Board::AddZombieByType");
    // PVZ2HookFunction(0x8AEC44, (void*)hookZombieEnterState, (void**)&oZombieEnterState, "Zombie::EnterState");

    // 49 and 127 sets a walking state. 49 is called on zombie spawn.
    // 61 and 168 enters state 0. 62 is state 1 again. 179 is state 3. 57 is 9

    // PVZ2HookFunction(0x778784, (void*)hookFun_7483F0, (void**)&oFun_7483F0, "sub_7483F0");
    // PVZ2HookFunction(0x772BC0, (void*)hookFun_772BC0, (void**)&oFun_772BC0, "sub_772BC0");

    // these 2 below only called when spawning zombies on CYS. Idc rn
    // PVZ2HookFunction(0x399EF4, (void*)hookFun_399EF4, (void**)&oFun_399EF4, "sub_399EF4");
    // PVZ2HookFunction(0x8AE694, (void*)hookZombieSetPosition, (void**)&oZombieSetPosition, "Zombie::SetPosition");

    // PVZ2HookFunction(0x8B2C5C, (void*)hookZombieFunc108, (void**)&oZombieFunc108, "ZombieFunc108");
    // PVZ2HookFunction(0x8B2D98, (void*)hookZombieFindAttackTarget, (void**)&oZombieFindAttackTarget, "Zombie::FindAttackTarget");
    // PVZ2HookFunction(0x8A7A90, (void*)hookZombieCheckTargetPlants, (void**)&oZombieCheckTargetPlants, "Zombie::CheckTarget3");
    // PVZ2HookFunction(0x50E7D8, (void*)hookFun_50E7D8, (void**)&oFun_50E7D8, "sub_50E7D8");

    // some lane 6 fixes
    PVZ2HookFunction(0x640338, (void*)hookFun_640338, (void**)&oFun_640338, "sub_640338");
    PVZ2HookFunction(0x6403F8, (void*)hookFun_6403F8, (void**)&oFun_6403F8, "sub_6403F8");

    // TODO  imp-unique function that checks for 5 rows: 7D276C. TODO

    // allows zombies to target plants on row 6
    PVZ2HookFunction(0xF3A78C, (void*)hookPlantGroupInitWithGridCoords, (void**)&oPlantGroupInitWithGridCoords, "PlantGroup::InitWithGridCoords");

    // make lawn with 6 lanes move upwards a bit
    // PVZ2HookFunction(0x334ED8, (void*)hkMoveBoardFunc, (void**)&oMoveBoardFunc, "MoveBoard");
    // PVZ2HookFunction(0x7373A8, (void*)hookFun_7373A8, (void**)&oFun_7373A8, "sub_7373A8");
    // PVZ2HookFunction(0x716E78, (void*)hookFun_716C5C, (void**)&oFun_716C5C, "sub_716C5C");
    PVZ2HookFunction(0x72692C, (void*)hookBoardRenderLawn, (void**)&oBoardRenderLawn, "sub_72692C");

    // shovel and plantfood cursor code fix
    PVZ2HookFunction(0x6404CC, (void*)hookFun_6404CC, (void**)&oFun_6404CC, "sub_6404CC");
    // FUNCTION ABOVE IS NOT YET MIGRATED


    // PVZ2HookFunction(0x104B6CC, (void*)hookFun_104B6CC, (void**)&oFun_104B6CC, "sub_104B6CC");

    // plant cursor stops showing up on lane 6 fix
    PVZ2HookFunction(0xCF3CC4, (void*)hkPlantCursorCanPlantOnCursor, (void**)&oPlantCursorCanPlantOnCursor, "sub_CF3CC4");
    PVZ2HookFunction(0x2AD5A0, (void*)hookGetBoardPlanting, (void**)&oGetBoardPlanting, "GetBoardPlanting");

    // sub_A9EBD8 looks interesting. Row iteration?

    // (0, 0) fix is within canPlantAt

    // check for module loading
    // PVZ2HookFunction(0x2E3C78, (void*)hookHasPerkHandlerModuleProperties, (void**)&oHasPerkHandlerModuleProperties, "LevelDefinition::HasPerkModuleProperties");

    // board touch
    // PVZ2HookFunction(0x72E610, (void*)hookBoardHandleTouch, (void**)&oBoardHandleTouch, "Board::HandleTouch");
    // PVZ2HookFunction(0x10AA0BC, (void*)hookFun, (void**)&oFun, "Fun widget 51");
    // PVZ2HookFunction(0xF16FFC, (void*)hookFun, (void**)&oFun, "F16FFC");

    // 71FBF8 is the calculate backdrop function
    // PVZ2HookFunction(0x71FBF8, (void*)hookFun_71FBF8, (void**)&oFun_71FBF8, "71FBF8");

    // PVZ2HookFunction(0x60954C, (void*)hkSeedChooserPress, (void**)&oSeedChooserPress, "SeedChooserPress");
    // PVZ2HookFunction(0x1241510, (void*)hookFun, (void**)&oFun, "HA"); // 26 controls the screen's rendering
    // PVZ2HookFunction(0x1241534, (void*)hookFun, (void**)&oFun, "HA");    
    // 
    //PVZ2HookFunction(0x5D33BC, (void*)hkLawnAppInit, (void**)&oLawnAppInit, "lawnAppInit"); cannot hook these are init after lawnAppInit
    // lawnAppInit();
    // PVZ2HookFunction(0x5D52F0, (void*)hkFun2, (void**)&oFun2, "fun2");

    // lawnApp offset 01CF299C

    // PVZ2HookFunction(0x4C0B44, (void*)hkRenderLawn, (void**)&oRenderLawn, "renderLawnStage");

    // filters for 6 lane levels
    // PVZ2HookFunction(0x10B2AB4, (void*)hkFun3, NULL, "StrFormat");
    //PVZ2HookFunction(0x1243018, (void*)hkFun4, (void**)&oFun4, "fun4");
    // PVZ2HookFunction(0x71FE8C, (void*)hkFun5, (void**)&oFun5, "fun5");

    // NOTE FOR NON MIGRATED FUNS:
    // 0xE83AB0
    // PVZ2HookFunction(0x2A4768, (void*)hkFun6, (void**)&oFun6, "fun6");

    // PVZ2HookFunction(0x71B1AC, (void*)hkBoardSetupUI, (void**)&oBoardSetupUI, "board::SetupUI");
    // PVZ2HookFunction(0x72215C, (void*)hkFun_72215C, (void**)&oFun_72215C, "72215C");
    // PVZ2HookFunction(0x7280C4, (void*)hkFun7, (void**)&oFun7, "7280C4");
    // PVZ2HookFunction(0x124086C, (void*)hkWidgetFun9, (void**)&oWidgetFun9, "widget fun 9");
    // PVZ2HookFunction(0x610274, (void*)hkSeedChooserViewSelector, (void**)&oSeedChooserViewSelector, "seed chooser view selector");


    // track execution from renderLawn. Let's see
    // PVZ2HookFunction(0x10AA0BC, (void*)hkWidgetFun51, (void**)&oWidgetFun51, "widget fun 51");

    PVZ2HookFunction(0x721704, (void*)hkBoardCanAddGridItemHere, (void**)&oBoardCanAddGridItemHere, "Board::CanAddGridItemHere");

    // banana cursor
    PVZ2HookFunction(0x54AE70, (void*)hkFun_54AE70, (void**)&oFun_54AE70, "banana cursor expand board range");
    // PVZ2HookFunction(0xA59B34, (void*)hkFun_A59B34, (void**)&oFun_A59B34, "A59B34");
    // PVZ2HookFunction(0xA578E4, (void*)hkFun_A578E4, (void**)&oFun_A578E4, "A578E4");
    PVZ2HookFunction(0x6402CC, (void*)hkFun_6402CC, (void**)&oFun_6402CC, "banana cursor allow shoot on lane 6");

    // celery stalker PF fix
    // PVZ2HookFunction(0x649EC0, (void*)hkFun_649EC0, (void**)&oFun_649EC0, "649EC0 celery stalker fix");

    // mower spawning code WIP
    
    /*
    PVZ2HookFunction(0x8D8A54, (void*)hookMowerSpawnOnLawn, (void**)&oMowerSpawnOnLawn, "sub_8D8A54");
    PVZ2HookFunction(0x8D9A1C, (void*)hookMowerPlaySmokeEffect, (void**)&oMowerPlaySmokeEffect, "sub_8D9A1C");
    PVZ2HookFunction(0x7207DC, (void*)hookFun_7207DC, (void**)&oFun_7207DC, "sub_7207DC");
    PVZ2HookFunction(0x990BE4, (void*)hookFun_990BE4, (void**)&oFun_990BE4, "sub_990BE4");  
    // PVZ2HookFunction(0xFB3168, (void*)hkFun10, (void**)&oFun10, "FB3168");
    PVZ2HookFunction(0x8D882C, (void*)hkFun11, (void**)&oFun11, "8D882C");
    PVZ2HookFunction(0x8D9C7C, (void*)hkMowerFunc3, (void**)&oMowerFunc3, "Mower func 3");
    // PVZ2HookFunction(0x8D88A4, (void*)hkFun12, (void**)&oFun12, "8D88A4");
    PVZ2HookFunction(0x2D8AD8, (void*)hkFun13, (void**)&oFun13, "fun 13");
    */
    PVZ2HookFunction(0xFB256C, (void*)hkFun80, (void**)&oFun80, "fun80"); // fix mowers

    // PVZ2HookFunction(0x2D874C, (void*)hkZombieSetSpawnPos, (void**)&oZombieSetSpawnPos, "Zombie::SetSpawnPosition"); // just used for debugging

    // allow lane 6 spawns
    //PVZ2HookFunction(0x2904C4, (void*)hkGetRowToSpawn, (void**)&oGetRowToSpawn, "fun83"); // implement get row to spawn logic for higher row numbers
    //// PVZ2HookFunction(0x2903D4, (void*)hkFun84, (void**)&oFun84, "fun84"); // called from 28EFF8 called from 28EEFC
    //// PVZ2HookFunction(0x28EEFC, (void*)hkFun85, (void**)&oFun85, "fun85");
    //// PVZ2HookFunction(0x28CE98, (void*)hkFun86, (void**)&oFun86, "fun86"); // forces reads of row spawns to be 1-indexed
    //PVZ2HookFunction(0x28F474, (void*)hkSetWaveZombiesPositions, (void**)&oSetWaveZombiesPositions, "fun87"); // true lane 6 fix for non crashing. drops support for spawn patterns

    //// random rows allow lane 6
    //PVZ2HookFunction(0x72CBAC, (void*)hkCanAddZombieToRow, (void**)&oCanAddZombieToRow, "fun85"); // this still all passes the check. arguably unnecessary ngl
    //PVZ2HookFunction(0x1087CFC, (void*)hkDecideRandomRow, (void**)&oDecideRandomRow, "fun88");
    //// PVZ2HookFunction(0x1087E5C, (void*)hkFun89, (void**)&oFun89, "fun89");
    //PVZ2HookFunction(0x72BB20, (void*)hkDecideRandomRow, (void**)&oChooseRowForZombie, "ChooseRowForZombie 72BB20");

    // ambush fixes
    /*PVZ2HookFunction(0xEC03F8, (void*)hkGetLowTideRow, (void**)&oGetLowTideRow, "GetLowTideRow");
    PVZ2HookFunction(0x2922D4, (void*)hkGetLowTideRow, (void**)&oGetLowTideRow, "ZombieRainSpawner");*/
    // PVZ2HookFunction(0xC9B6A0, (void*)hkFun90, (void**)&oFun90, "fun90");
    /*PVZ2HookFunction(0xEBF84C, (void*)hkSpawnAmbushBeach, (void**)&oSpawnAmbushBeach, "SpawnAmbushBeach");
    PVZ2HookFunction(0x29184C, (void*)hkZombieRainSpawnerSpawnAmbush, (void**)&oZombieRainSpawnerSpawnAmbush, "ZombieRainSpawner::SpawnAmbush");*/

    // jam effects fixes
    PVZ2HookFunction(0xD1CEC8, (void*)hkFun95, (void**)&oFun95, "fun95");
    PVZ2HookFunction(0x7401A8, (void*)hkFun96, (void**)&oFun96, "fun96");

    initPlantsFix();
    initZombiesFix();
    initSixLaneSpawnsHooks();
    initSixLaneAmbushFix();

    SixLaneModule::modInit();
    SixLaneModuleProps::modInit();
#endif

    // initZombotanyHooks();

    initTideHooks();
    GridItemPuddle::modInit();

    PVZ2HookFunction(0xA9E7F0, (void*)hkSubA9E7F0, (void**)&oSubA9E7F0, "A9E7F0");

    GridItemRailAlt::modInit();
    GridItemRailAltProps::modInit();
    initRailAlt();

    ZombieSteamMechanic::modInit();

    /*
    PVZ2HookFunction(0xE864FC, (void*)hkFun110, (void**)&oFun110, "fun110");
    PVZ2HookFunction(0xCE0148, (void*)hkUpdateSinkDist, NULL, "updatesink dist");
    PVZ2HookFunction(0x72B08C, (void*)hookBoardAddProjectileByType, (void**)&oBoardAddProjectileByType, "Board::AddProjectileByType");
    PVZ2HookFunction(0x7401A8, (void*)hkRenderPopAnim, (void**)&oRenderPopAnim, "RenderPopAnim");
    */

    // PVZ2HookFunction(0x6B6AC0, (void*)hkFun170, (void**)&oFun170, "fun170");

    // PVZ2HookFunction(0xF3E09C, (void*)disableWithCond, (void**)&oFun171, "fun171");
    PVZ2HookFunction(0xE89E48, (void*)applyEffectsForCond, (void**)&oFun172, "fun172");
    // PVZ2HookFunction(0xE8B0A0, (void*)removeEffectsForCond, (void**)&oFun178, "fun178");
    // PVZ2HookFunction(0x46C280, (void*)hkFun173, (void**)&oFun173, "fun173");
    // PVZ2HookFunction(0xE819D0, (void*)hkFun174, (void**)&oFun174, "Plant ApplyCond");
    //PVZ2HookFunction(0xE864FC, (void*)hkFun110, (void**)&oFun110, "fun110");
    // PVZ2HookFunction(0x277418, (void*)hkFun175, (void**)&oFun175, "fun175");
    // PVZ2HookFunction(0x469234, (void*)hkFun176, (void**)&oFun176, "fun176");

    // PVZ2HookFunction(0x46BDE0, (void*)hkGetLaunchIntervalScaling, (void**)&oGetLaunchIntervalScaling, "launch interval scaling");

    // Enable for this for frog code
    // initPlantCustomConditionsHooks();

    // PVZ2HookFunction(0x6748B4, (void*)hkFun202, (void**)&oFun202, "plant relocation function");

    // PVZ2HookFunction(0xFE4E4C, (void*)hkSub205, (void**)&oSub205, "fun205"); // this handles rendering the tide effect and hiding the bottom
    // PVZ2HookFunction(0x8BFE78, (void*)hkSub206, (void**)&oSub206, "sub206");
    initSkyCityHooks();

    // PVZ2HookFunction(0x705F0C, (void*)hkFun207, (void**)&oFun207, "board logic"); // troll

    initSurvivalHooks();


    initRoofHooks();
    
    PVZ2HookFunction(0x7367AC, (void*)hkDrawCursor, (void**)&oDrawCursor, "draw cursor highlights");
    // PVZ2HookFunction(0x134952C, (void*)hkRenderRect, (void**)&oRenderRect, "render rectangle");
    // PVZ2HookFunction(0x134F594, (void*)hkFun290, (void**)&oFun290, "fun 290");

    // PVZ2HookFunction(0x3CA1E4, (void*)hkFun310, (void**)&oFun310, "fun 310");

    PVZ2HookFunction(0x3AE108, (void*)hkZombossFireActionDefGetTarget, (void**)&oZombossFireActionDefGetTarget, "zomboss fire action definition get target");
    

    LOGI("Finished initializing");
}