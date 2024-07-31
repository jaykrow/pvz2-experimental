#include "pch.h"
#include "ZombotanyHooks.h"
#include "Logging.h"
#include "memUtils.h"
#include "ZombotanyBaseProps.h"
#include "PvZ2/Board.h"
#include "PvZ2/RtWeakPtr.h"
#include <assert.h>
#include "ZombieConditions.h"

#include <atomic>
#include <mutex>

typedef void (*ZombieBullSpawn)(int);
ZombieBullSpawn oZombieBullSpawn = NULL;

void hkZombieBullSpawn(int a1) {
    if (!getZombotanyBaseProps(a1)) {
        oZombieBullSpawn(a1);
        return;
    }

    *(uint8_t*)(a1 + 0x30A) = 0;
    *(int*)(a1 + 0x30C) = 0;
    *(uint8_t*)(a1 + 0x308) = 0;

    typedef void (*ZombieFun49)(int); // zombie spawn callback
    ((ZombieFun49)getActualOffset(0x8AEAF8))(a1);

    // Zombie::EnterState(a1, 16, 0); // skip setting the bull charging stage

    //float v4[3] = { 875.0, *(float*)(a1 + 0x18), *(float*)(a1 + 0x1C) }; // bull spawns at mX = 920
    //typedef int (*ZombieSetSpawnPosition)(int, float*);
    //((ZombieSetSpawnPosition)getActualOffset(0x2D874C))(a1, v4);
}

typedef void (*zombieBullThrowRider)(int);
zombieBullThrowRider oZombieBullThrowRider = NULL;

void hkZombieBullThrowRider(int a1) {
    ZombotanyBaseProps* props = getZombotanyBaseProps(a1);
    if (!props) {
        oZombieBullThrowRider(a1);
        return;
    }

    // LOGI("Zombotany throw rider hasAction = %d", *(uint8_t*)(a1 + 0x308));
    typedef bool (*zombieHasCondition)(int, int);
    zombieHasCondition checkZombieHasCondition = (zombieHasCondition)getActualOffset(0x8AF9E4);

    if (!*(uint8_t*)(a1 + 0x308)
        && !checkZombieHasCondition(a1, zombie_condition_shrinking)
        && !checkZombieHasCondition(a1, zombie_condition_shrunken)) {
        // LOGI("Can fire projectile");
        // typedef int (*zombieGetAnimRig)(int);
        // int animRig = ((zombieGetAnimRig)getActualOffset(0x74142C))(a1);

        // typedef void (*zombieBullAnimRigHideRiderImp)(int);
        // ((zombieBullAnimRigHideRiderImp)getActualOffset(0x760F8C))(animRig);

        int board = (int)getBoard();

        typedef int (*getZombieTypeDirectoryInstance)();
        int typeDir = ((getZombieTypeDirectoryInstance)getActualOffset(0x28FE30))();

        typedef void (*getZombieType)(RtWeakPtr<int>*, int, std::string*);
        RtWeakPtr<int> zType;

        std::string name = props->zombieType;

        if (checkZombieHasCondition(a1, zombie_condition_hypnotized)) {
            name = "hypno_" + name;
        }

        const char* zombieTypeStr = name.c_str();
        // LOGI("Zombotany throws a %s", zombieTypeStr);

        ((getZombieType)getActualOffset(0x28FE9C))(&zType, typeDir, &name);

        typedef int (*addZombieByType)(int, int, int, char, int);
        addZombieByType funAddZombieByType = (addZombieByType)getActualOffset(0x72B4D8);
        int spawnedZombie = funAddZombieByType(board, (int)&zType, -1, 6, -1);

        typedef void (*dtor)(RtWeakPtr<int>*);
        ((dtor)(getActualOffset(0x10B58F8)))(&zType);

        *(char*)(spawnedZombie + 0x30D) = true;

        // skip hypnotized bull check for now

        float newX = *(float*)(a1 + 0x14) - 60;
        float newY = *(float*)(a1 + 0x18);
        float newZ = *(float*)(a1 + 0x1C) + 50;

        typedef int (*boardEntitySetPosition)(int, SexyVector3*);
        boardEntitySetPosition funBoardEntitySetPosition = (boardEntitySetPosition)getActualOffset(0x2D850C);
        SexyVector3 newCoords = SexyVector3(newX, newY, newZ);
        funBoardEntitySetPosition(spawnedZombie, &newCoords);

        // a function call to 0x338 of the spawned zombie's vtable to animate its launching animation (for imps only)

        // sound playing code

        // set 0x308 to true to prevent further imp throws
        // *(char*)(a1 + 0x308) = true;
    }
}

typedef void (*zombieBullPlayDeath)(int);
zombieBullPlayDeath oZombieBullPlayDeath = NULL;

void hkZombieBullPlayDeath(int a1) {
    // LOGI("Bull play death");

    if (!getZombotanyBaseProps(a1)) {
        oZombieBullPlayDeath(a1);
        return;
    }

    typedef void(*zombieFun197)(int);
    ((zombieFun197)getActualOffset(0x8C2110))(a1); // does not play the bull's death sound effect
}

void zombotanyAnimRigHeadHide(int animRig) {
    typedef int (*funSetLayerVisible)(int, SexyString*, bool);
    funSetLayerVisible setLayerVisible = (funSetLayerVisible)getActualOffset(0x667590);

    SexyString layerName = "zombie_skull";
    setLayerVisible(animRig, &layerName, false);
}

typedef void (*zombieDie)(int);
zombieDie oZombieDie = NULL;

void hkZombieDie(int a1) {
    // LOGI("zombie dying state 3");

    if (getZombotanyBaseProps(a1)) {
         typedef int (*zombieGetAnimRig)(int);
         int animRig = ((zombieGetAnimRig)getActualOffset(0x74142C))(a1);

         //typedef void (*zombieBullAnimRigHideRiderImp)(int);
         //((zombieBullAnimRigHideRiderImp)getActualOffset(0x760F8C))(animRig); // can be used by coders to hide the head

         zombotanyAnimRigHeadHide(animRig);

        *(uint8_t*)(a1 + 0x308) = true; // prevents any more shots
    }

    oZombieDie(a1);
}

void zombotanyAnimRigArmHide(int animRig, int damageState) { // replaces bull's 75ED64 logic
    if (damageState == 0) {
        return;
    }
    typedef int (*funSetLayerVisible)(int, SexyString*, bool);
    funSetLayerVisible setLayerVisible = (funSetLayerVisible)getActualOffset(0x667590);

    SexyString layerName = "damage2_bull_head_01"; // arm with bone layer
    setLayerVisible(animRig, &layerName, true);

    layerName = "zombie_zombotany_hand_outer";
    setLayerVisible(animRig, &layerName, false);

    layerName = "zombie_zombotany_arm_outer_lower";
    setLayerVisible(animRig, &layerName, false);

    layerName = "zombie_zombotany_arm_outer_upper_01";
    setLayerVisible(animRig, &layerName, false);
}

typedef void (*zombieBullUpdateAnimRigDamagedState)(int);
zombieBullUpdateAnimRigDamagedState oZombieBullUpdateAnimRigDamagedState = NULL;

void hkZombieBullUpdateAnimRigDamagedState(int r0_0) {
    if (!getZombotanyBaseProps(r0_0)) {
        oZombieBullUpdateAnimRigDamagedState(r0_0);
        return;
    }

    RtWeakPtr<int> v11 = RtWeakPtr<int>();
    v11.FromOther((RtWeakPtr<int>*)(r0_0 + 0x300));

    int zombotanyProps = (int) v11.get();   
    float zombieMaxHealth = *(float*)(zombotanyProps + 0x30);
    // LOGI("v8 = %f", v8);
    
    float zombieCurrentHealth = *(float*)(r0_0 + 0xF8);

    float armDropFraction = *(float*)(zombotanyProps + 0x68);
    int newDamageState = zombieCurrentHealth < zombieMaxHealth * armDropFraction;

    /* // original zombie bull code to get new damage state
    typedef int (*sub_16879F8)(int, uint); // pretty sure this function just performs a division
    int newDamageState = 2 - ((sub_16879F8)getActualOffset(0x16879F8))((int)zombieCurrentHealth, (int)zombieMaxHealth / 3);
    if (newDamageState < 0) {
        newDamageState = 0;
    }
    if (newDamageState >= 2) {
        newDamageState = 2;
    }
    */

    if (newDamageState != *(int*)(r0_0 + 0x30C)) {
        *(int*)(r0_0 + 0x30C) = newDamageState;
        
        typedef int (*zombieGetAnimRig)(int);
        int animRig = ((zombieGetAnimRig)getActualOffset(0x74142C))(r0_0);

        /* // original zombie bull code to set anim rig states
        typedef void (*sub_75ED64)(int, int);
        ((sub_75ED64)getActualOffset(0x75ED64))(animRig, newDamageState);
        */

        zombotanyAnimRigArmHide(animRig, newDamageState);
    }

    typedef void (*zombieFun29)(int);
    ((zombieFun29)getActualOffset(0x8AEE80))(r0_0);
}

typedef void (*fun760F24)(int, int);
fun760F24 oFun760F24 = NULL;

void hkFun760F24(int zombie, int zombieCondition) {
    // this function sets the bull to rearing on hypno. We don't want that for zombotanies since they don't have the animation
    if (!getZombotanyBaseProps(zombie)) {
        oFun760F24(zombie, zombieCondition);
        return;
    }

    if (zombieCondition != zombie_condition_gummed) {
        if (zombieCondition != zombie_condition_hypnotized) {
            return;
        }
        typedef void (*sub8B25E8)(int, int);
        ((sub8B25E8)getActualOffset(0x8B25E8))(zombie, 0);
    }
}

void* bullAnimRigVftable = NULL;

typedef void* (*bullAnimRigConstruct)();
bullAnimRigConstruct oBullAnimRigConstruct = NULL;

void* hkBullAnimRigConstruct() {
    void* res = oBullAnimRigConstruct();
    
    //LOGI("Constructs bulls anim rig");
    if (!bullAnimRigVftable) {
        bullAnimRigVftable = copyVFTable(getActualOffset(0x1BE2814), 67);

        // DC contains getHeadLayerToDrop, E0 contains getArmLayerToDrop
        // redirects these to future basic's corresponding animations
        patchVFTable(bullAnimRigVftable, (void*)getActualOffset(0x2E9FF4), 55);
        patchVFTable(bullAnimRigVftable, (void*)getActualOffset(0x2EA258), 56);

        //LOGI("Patched the VFTable");
    }

    *(void**)res = bullAnimRigVftable;

    return res;
}

void initZombotanyHooks() {
    PVZ2HookFunction(0x760B70, (void*)hkZombieBullThrowRider, (void**)&oZombieBullThrowRider, "ZombieBull::ThrowRider");
    // PVZ2HookFunction(0x75FB68, (void*)hkZombieBullEnterChargingState, (void**)&oZombieBullEnterChargingState, "ZombieBull::EnterChargingState");
    // PVZ2HookFunction(0x76035C, (void*)hkFun_76035C, (void**)&oFun_76035C, "fun_76035C"); // used to test collision. Only handles collision while charging
    PVZ2HookFunction(0x75F778, (void*)hkZombieBullPlayDeath, (void**)&oZombieBullPlayDeath, "ZombieBull::PlayDeath"); // remove zombie bull's death sound
    PVZ2HookFunction(0x75EB64, (void*)hkZombieBullSpawn, (void**)&oZombieBullSpawn, "760354");
    PVZ2HookFunction(0x8B83A8, (void*)hkZombieDie, (void**)&oZombieDie, "Zombie:::Die");
    PVZ2HookFunction(0x75EC04, (void*)hkZombieBullUpdateAnimRigDamagedState, (void**)&oZombieBullUpdateAnimRigDamagedState, "ZombieBull::UpdateAnimRigDamageState");

    // Disables rearing on hypno
    PVZ2HookFunction(0x760F24, (void*)hkFun760F24, (void**)&oFun760F24, "fun760F24");

    // hook bull's anim rig to my custom vftable 
    PVZ2HookFunction(0x761B68, (void*)hkBullAnimRigConstruct, (void**)&oBullAnimRigConstruct, "ZombieAnimRig_Bull::Construct");

    ZombotanyBaseProps::modInit();
}
