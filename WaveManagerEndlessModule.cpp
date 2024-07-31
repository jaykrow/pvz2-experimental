#include "pch.h"
#include "WaveManagerEndlessModule.h"
#include "SurvivalModule.h"
#include "assert.h"
#include "WaveManagerProperties.h"
#include "MessageRouter.h"
#include "Messages.h"
#include "memUtils.h"
#include "PvZ2/Board.h"
#include "TimeMgr.h"
#include "LevelEscalationModule.h"
#include "WaveManagerEndlessModuleProps.h"
#include <set>
#include <random>
#include "StageModule.h"
#include "PVZDB.h"
#include "DangerRoomModernDesigner.h"

std::random_device rd;
std::mt19937 gen(rd());

DEFINE_STATIC_RT_CLASS_MEMBERS(WaveManagerEndlessModule);

void WaveManagerProgress(WaveManager* manager) {
	typedef void (*WaveManagerProgress)(WaveManager*);
	((WaveManagerProgress)GetActualOffset(0x56D20C))(manager);
}

void GenerateWaveManagerData(WaveManagerGenerationData* data, WaveManagerEndlessModule* module, int zombiesToSelect) {
	data->unk = 0;
	data->unk4 = 0;
	data->unk5 = 0;
	data->unk6 = 0;

	RtWeakPtr<WaveManagerEndlessModuleProps> propsPtr;
	propsPtr.FromOther((RtWeakPtr<WaveManagerEndlessModuleProps>*) & module->propSheetPtr);

	auto props = propsPtr.get();

	data->TotalWaves = (props->FlagsPerChooser + 1) * props->WavesPerFlag;
	data->WavesPerFlag = props->WavesPerFlag;
	data->StartingPoints = module->m_currentPoints;
	data->PointIncrementPerWave = props->PointIncrementPerWave;
	data->MaxPoints = props->MaxPoints;

	typedef time_t (*GetRandSeed)(int type);
	data->RandSeed = ((GetRandSeed)GetActualOffset(0x721A54))(1);

	LOGI("lots of wave data set");

	// Add zombies to the pool
	auto waveGenData = data->GetOrCreate(0);
	std::vector<int> available;
	for (int i = 0; i < props->ZombiePool.size(); ++i) {
		available.push_back(i);
	}
	while (zombiesToSelect-- && !available.empty()) {
		std::uniform_int_distribution<> dis(0, available.size() - 1);
		int k = dis(gen);
		int realIndex = available[k];
		available.erase(available.begin() + k);
		
		waveGenData->zombieTypes.push_back(props->ZombiePool[realIndex].get());
	}

	LOGI("waves selecting zombies");

	StageModule* stageModule = StageModule::GetInstance(GetBoard());
	auto stageProps = stageModule->GetProps();
	RtWeakPtr<ZombieType> basic, armor1, armor2;
	ZombieType::GetFromName(&basic, stageProps->basicZombieTypeName);
	ZombieType::GetFromName(&armor1, stageProps->armor1ZombieTypeName);
	ZombieType::GetFromName(&armor2, stageProps->armor2ZombieTypeName);

	waveGenData->zombieTypes.push_back(basic.get());
	waveGenData->zombieTypes.push_back(armor1.get());
	waveGenData->zombieTypes.push_back(armor2.get());

	LOGI("waves generated");

	// TODO: Add plant food creation code. WaveGenerationData.id represents the wave number
}

static DangerRoomModernDesigner* portalGenerator;
static UnkMTRand* portalRandInstance;

void AddPortalSpawns(WaveManagerProperties* waveManagerProps) {
	// We'll create a fake danger room modern designer object and call its generator function
	// 
	// TODO: We can just have a real danger room modern designer and then register it with a callback
	if (portalGenerator == NULL) {
		portalGenerator = new DangerRoomModernDesigner();
		portalGenerator->LevelRange = ValueRange(1, 30);
		portalGenerator->PortalTypesMin = ValueRange(1, 1);
		portalGenerator->PortalTypesMax = ValueRange(4, 4);
		portalGenerator->FirstPortalWavesMin = ValueRange(6, 7);
		portalGenerator->FirstPortalWavesMax = ValueRange(1, 2);
		portalGenerator->NextPortalWavesMin = ValueRange(4, 6);
		portalGenerator->NextPortalWavesMax = ValueRange(2, 3);
		portalGenerator->PortalCountInWaveMin = ValueRange(1, 1);
		portalGenerator->PortalCountInWaveMax = ValueRange(2, 3);
		portalGenerator->ColumnToSpawnMin = ValueRange(8, 8);
		portalGenerator->ColumnToSpawnMax = ValueRange(6, 7);
		portalGenerator->ZombiesToSpawnMin = ValueRange(1, 2);
		portalGenerator->ZombiesToSpawnMax = ValueRange(3, 4);
		portalGenerator->ZombieRandomListStartMin = ValueRange(0, 0);
		portalGenerator->ZombieRandomListStartMax = ValueRange(0, 0);
		portalGenerator->ZombieRandomListEndMin = ValueRange(0, 0);
		portalGenerator->ZombieRandomListEndMax = ValueRange(3, 3);

		portalGenerator->PortalTypePool.push_back(StringWeightedOption("dangerroom_egypt_new", 5.0));
		portalGenerator->PortalTypePool.push_back(StringWeightedOption("dangerroom_egypt_2", 5.0));
		portalGenerator->PortalTypePool.push_back(StringWeightedOption("dangerroom_pirate_new", 5.0));
		portalGenerator->PortalTypePool.push_back(StringWeightedOption("dangerroom_pirate_2", 5.0));
		portalGenerator->PortalTypePool.push_back(StringWeightedOption("dangerroom_west_new", 5.0));
		portalGenerator->PortalTypePool.push_back(StringWeightedOption("dangerroom_west_2", 5.0));
		portalGenerator->PortalTypePool.push_back(StringWeightedOption("dangerroom_future_new", 5.0));
		portalGenerator->PortalTypePool.push_back(StringWeightedOption("dangerroom_future_2", 5.0));
		portalGenerator->PortalTypePool.push_back(StringWeightedOption("dangerroom_dark_new", 5.0));
		portalGenerator->PortalTypePool.push_back(StringWeightedOption("dangerroom_dark_2", 5.0));
		portalGenerator->PortalTypePool.push_back(StringWeightedOption("dangerroom_beach_new", 5.0));
		portalGenerator->PortalTypePool.push_back(StringWeightedOption("dangerroom_beach_2", 5.0));
		portalGenerator->PortalTypePool.push_back(StringWeightedOption("dangerroom_iceage_new", 5.0));
		portalGenerator->PortalTypePool.push_back(StringWeightedOption("dangerroom_iceage_2", 5.0));
		portalGenerator->PortalTypePool.push_back(StringWeightedOption("dangerroom_lostcity_new", 5.0));
		portalGenerator->PortalTypePool.push_back(StringWeightedOption("dangerroom_lostcity_2", 5.0));
		portalGenerator->PortalTypePool.push_back(StringWeightedOption("dangerroom_eighties_new", 5.0));
		portalGenerator->PortalTypePool.push_back(StringWeightedOption("dangerroom_eighties_2", 5.0));
		portalGenerator->PortalTypePool.push_back(StringWeightedOption("dangerroom_dino_new", 5.0));
		portalGenerator->PortalTypePool.push_back(StringWeightedOption("dangerroom_dino_2", 5.0));
		
		// BELOW IS SETTINGS FOR VANILLA TEST OBB
		/*
		portalGenerator->PortalTypePool.push_back(StringWeightedOption("1", 10.0));
		portalGenerator->PortalTypePool.push_back(StringWeightedOption("2", 10.0));
		portalGenerator->PortalTypePool.push_back(StringWeightedOption("3", 10.0));
		portalGenerator->PortalTypePool.push_back(StringWeightedOption("dangerroom_egypt", 10.0));
		portalGenerator->PortalTypePool.push_back(StringWeightedOption("dangerroom_pirate", 10.0));
		portalGenerator->PortalTypePool.push_back(StringWeightedOption("dangerroom_west", 10.0));
		portalGenerator->PortalTypePool.push_back(StringWeightedOption("dangerroom_future", 10.0));
		portalGenerator->PortalTypePool.push_back(StringWeightedOption("dangerroom_dark", 10.0));
		portalGenerator->PortalTypePool.push_back(StringWeightedOption("dangerroom_beach", 10.0));
		portalGenerator->PortalTypePool.push_back(StringWeightedOption("dangerroom_iceage", 10.0));
		portalGenerator->PortalTypePool.push_back(StringWeightedOption("dangerroom_lostcity", 10.0));
		// portalGenerator->PortalTypePool.push_back(StringWeightedOption("dangerroom_eighties", 10.0));
		portalGenerator->PortalTypePool.push_back(StringWeightedOption("dangerroom_dino", 10.0));
		*/
	}

	if (portalRandInstance == NULL) {
		portalRandInstance = new UnkMTRand();

		typedef time_t (*GetRandSeed)(int type);
		time_t seed = ((GetRandSeed)GetActualOffset(0x721A54))(1);

		typedef void (*initWithSeed)(UnkMTRand*, time_t);
		((initWithSeed)getActualOffset(0x112CE5C))(portalRandInstance, seed);
	}

	typedef void (*generatePortalWaves)(DangerRoomModernDesigner*, int*, WaveManagerProperties*);
	int levelAndRng[2] = { SurvivalModule::GetInstance()->chooserCount + 1, (int) portalRandInstance };

	LOGI("called to generate portal with level %d", levelAndRng[0]);

	((generatePortalWaves)getActualOffset(0x90BA04))(portalGenerator, levelAndRng, waveManagerProps);
}

void CreateAndSetNewWaves(WaveManagerEndlessModule* module) {
	WaveManagerGenerationData waveManagerGenData;
	GenerateWaveManagerData(&waveManagerGenData, module, std::min(6, SurvivalModule::GetInstance()->chooserCount + 1));
	LOGI("Got wave manager generation data");

	RtWeakPtr<WaveManagerProperties> waveManagerPropsPtr;
	WaveManager* manager = &module->waveManager;

	typedef WaveManagerProperties* (*createObject)(Sexy::RtClass*, RtTypeId, int);
	WaveManagerProperties* generatedProps = ((createObject)getActualOffset(0x54B580))(
		WaveManagerProperties::GetRTClass(),
		ActiveWaveActions,
		1);

	// TODO: Add code to take stuff from existing WaveManagerProperties

	typedef void (*generateWavesFromData)(WaveManagerGenerationData*, WaveManagerProperties*);
	((generateWavesFromData)getActualOffset(0x56CA24))(&waveManagerGenData, generatedProps);
	LOGI("called generate waves");

	// for some reason it's overridden. Original code also does this at 6BBA64
	generatedProps->FlagWaveInterval = waveManagerGenData.WavesPerFlag;

	AddPortalSpawns(generatedProps);

	RtWeakPtr<LevelDefinition> levelDefPtr;
	levelDefPtr.FromOther(&GetBoard()->levelDefinition);
	LevelDefinition* levelDef = levelDefPtr.get();

	RtWeakPtr<int> lootConfigPtr;
	lootConfigPtr.FromOther((RtWeakPtr<int>*) & levelDef->lootConfig);
	int lootConfig = (int)lootConfigPtr.get();

	typedef void (*waveManagerSetProps)(WaveManager*, RtWeakPtr<WaveManagerProperties>*, int, bool hasDynamicZombies);
	RtWeakPtr<WaveManagerProperties> managerPropsPtr;
	managerPropsPtr.FromOther((RtWeakPtr<WaveManagerProperties>*) & generatedProps->m_thisPtr);
	((waveManagerSetProps)getActualOffset(0x56B6A0))(&module->waveManager, &managerPropsPtr, lootConfig, 0);

	LOGI("set props successfully");
}

void PrepareWaveManager(WaveManagerEndlessModule* module) {
	WaveManager* manager = &module->waveManager;

	RtWeakPtr<WaveManagerEndlessModuleProps> propsPtr;
	propsPtr.FromOther((RtWeakPtr<WaveManagerEndlessModuleProps>*) & module->propSheetPtr);

	auto props = propsPtr.get();

	LOGI("Preparing wave manager for next level");
	// TODO: Add an increment per chooser property
	module->m_currentPoints += props->PointIncrementPerWave * props->WavesPerFlag * props->FlagsPerChooser;

	manager->m_currentWave = -1;
	manager->m_waves.clear();
	manager->m_waves.shrink_to_fit();

	LOGI("waves cleared");
	/*
	auto escalationModule = (LevelEscalationModule*) GetBoard()->levelModuleManager->GetModule(LevelEscalationModule::GetRTClass());
	// call the setup function
	LOGI("escalation module got");
	typedef void (*escalationSetup)(LevelEscalationModule*);
	escalationSetup pFunc = *(escalationSetup*)(*(uint*)escalationModule + 0x48);
	LOGI("pFunc got");
	pFunc(escalationModule);
	*/
	
	CreateAndSetNewWaves(module);

	// TODO: Figure out how to load zombie resource groups

	typedef void (*getResourceGroups)(int, LevelModule*, uint);
	// LOGI("Got wave manager module");
	// LOGI("vtbl function at 0x38 is %u", getOriginalOffset(*(uint*)(*(uint*)module + 0x38)));
	((getResourceGroups)getActualOffset(0x707AC0))(0, module, (uint) & GetBoard()->levelModuleManager->unkVec1);

	LOGI("got resource groups info");

	typedef void (*loadResources)(LevelModuleManager*);
	((loadResources)getActualOffset(0x7083B4))(GetBoard()->levelModuleManager);

	typedef void (*loadResourcesForReals)(LevelModuleManager*);
	((loadResourcesForReals)getActualOffset(0x708520))(GetBoard()->levelModuleManager);

	LOGI("Wave manager prepared");
}

void WaveManagerEndlessOnLevelStart(WaveManagerEndlessModule* a1) {
	// LOGI("Endless wave manager on start");
	// Will crash if there is no SurvivalModule
	if (SurvivalModule::GetInstance() == NULL) {
		LOGW("WaveManagerEndlessModule detected, but no SurvivalModule");
	}
	auto survivalModuleProps = SurvivalModule::GetInstance()->GetProps();
	int flagCount = survivalModuleProps->FlagsBetweenChoosers;

	typedef void (*WaveManagerOnStart)(WaveManagerModule*);
	((WaveManagerOnStart)getActualOffset(0x56AD38))(a1);

	// LOGI("Set flag count of %d", flagCount);
	MessageRouter::GetInstance()->ExecuteMessage(Messages::Get(SetFlagCount), flagCount);

	/*LOGI("cur time = %f", TimeMgr::GetInstance()->m_curTime);
	WaveManager* waveManager = &a1->waveManager;
	LOGI("paused = %d state = %d", waveManager->m_paused, waveManager->m_state);
	LOGI("last wave time = %f next wave time = %f", waveManager->m_lastWaveTime, waveManager->m_nextWaveTime);*/
}

bool WaveManagerEndlessCanWin(WaveManagerEndlessModule* a1) {
	return false;
}

void WaveManagerEndlessProgress(WaveManagerEndlessModule* a1) {
	WaveManagerProgress(&a1->waveManager);
}

void WaveManagerEndlessModule::RegisterCallbacks(WaveManagerEndlessModule* module) {
	LOGI("Endless module register callbacks");
	auto moduleManager = GetBoard()->levelModuleManager;
	auto startDelegate = CreateDelegate(
		(uint)WaveManagerEndlessOnLevelStart,
		false,
		(uint)module,
		true,
		GetActualOffset(0x573D84)
	);
	moduleManager->levelStartEvent.AddDelegate(startDelegate);

	auto progressDelegate = CreateDelegate(
		(uint)WaveManagerEndlessProgress,
		false,
		(uint)module,
		true,
		GetActualOffset(0x573D84)
	);
	moduleManager->unkEvent12.AddDelegate(progressDelegate);

	auto winConDelegate = CreateDelegate(
		(uint)WaveManagerEndlessCanWin,
		false,
		(uint)module,
		true,
		GetActualOffset(0x573DA0)
	);
	typedef void (*addDelegateRetBool)(Sexy::Event*, Sexy::DelegateBase*, int);
	((addDelegateRetBool)GetActualOffset(0x4B4B14))(&moduleManager->winCond, &winConDelegate, 0);

	MessageRouter* router = MessageRouter::GetInstance();
	auto levelEndedDelegate = CreateDelegate(
		(uint)PrepareWaveManager,
		false,
		(uint) module,
		true,
		getActualOffset(0xD527E0)
	);
	typedef void (*registerMsgDelegate)(MessageRouter*, uint, Sexy::DelegateBase*);
	((registerMsgDelegate)getActualOffset(0x5FDA50))(router, Messages::Get(SurvivalLevelEnded), &levelEndedDelegate);
}

void EndlessModuleSetup(WaveManagerEndlessModule* self) {
	RtWeakPtr<WaveManagerEndlessModuleProps> propsPtr;
	propsPtr.FromOther((RtWeakPtr<WaveManagerEndlessModuleProps>*) & self->propSheetPtr);
	auto props = propsPtr.get();

	self->m_currentPoints = props->StartingPoints;
}

void EndlessModuleSetupInitialWaves(WaveManagerEndlessModule* self) {
	CreateAndSetNewWaves(self);
}

void WaveManagerEndlessModule::ModInit() {
	LOGI("WaveManagerEndlessModule init");

	vftable = copyVFTable(getActualOffset(WAVEMANAGERMODULE_VTBL), WAVEMANAGERMODULE_VFUNCS);

	patchVFTable(vftable, (void*) WaveManagerEndlessModule::GetRTClass, 0);
	patchVFTable(vftable, (void*) WaveManagerEndlessModule::RegisterCallbacks, 7);
	patchVFTable(vftable, (void*) EndlessModuleSetup, 17);
	patchVFTable(vftable, (void*) EndlessModuleSetupInitialWaves, 18);

	GetRTClass();

	LOGI("WaveManagerEndlessModule finish init");
}