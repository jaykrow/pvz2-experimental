#include "pch.h"
#include "RoofModule.h"
#include "PvZ2/Board.h"
#include "DelegateBase.h"
#include "RoofModuleProps.h"
#include "MessageRouter.h"
#include "Messages.h"

DEFINE_STATIC_RT_CLASS_MEMBERS(RoofModule);

bool RoofModule::IsEffectSloped(int offsets[2], RtWeakPtr<void>* pam) {
	for (auto& sloped : GetProps()->SlopedEffects) {
		typedef void (*getPopAnimRes)(RtWeakPtr<Sexy::PopAnim>*, SexyString*);
		getPopAnimRes pGetPopAnimRes = (getPopAnimRes)getActualOffset(0xC90A14);

		RtWeakPtr<Sexy::PopAnim> popAnimPtr;
		pGetPopAnimRes(&popAnimPtr, &sloped.Name);

		auto popAnim = popAnimPtr.get();

		RtWeakPtr<void> slopedPam;
		slopedPam.FromOther(&popAnim->pamRtId);
		if (slopedPam.equals(pam)) {
			// LOGI("Effect match detected");
			offsets[0] = sloped.OffsetX;
			offsets[1] = sloped.OffsetY;
			return true;
		}
	}

	return false;
}

inline bool IsProjectileInList(Projectile* proj, const std::vector<RtWeakPtr<ProjectilePropertySheet>>& list) {
	for (auto& projectileProp : list) {
		if (proj->m_propertySheetPtr.equals((RtWeakPtr<void>*) & projectileProp)) {
			return true;
		}
	}

	return false;
}

bool RoofModule::IsProjectileImmune(Projectile* proj) {
	auto props = GetProps();
	return IsProjectileInList(proj, props->ImmuneToSlope);
}

/*
bool RoofModule::IsProjectileSpawnIgnoresSlope(Projectile* proj) {
	auto props = GetProps();
	return IsProjectileInList(proj, props->SpawnIgnoresSlope);
}
*/

bool RoofModule::IsProjectileSpawnIgnoresSlope(RtWeakPtr<ProjectilePropertySheet>* projectileProp) {
	auto props = GetProps();
	for (auto& prop : props->SpawnIgnoresSlope) {
		if (projectileProp->equals(& prop)) {
			return true;
		}
	}
	return false;
}

bool RoofModule::IsProjectileFollowsSlope(Projectile* proj) {
	auto props = GetProps();
	return IsProjectileInList(proj, props->FollowsSlope);
}

int RoofModule::GetProjectileTolerance(Projectile* proj) {
	auto props = GetProps();
	for (auto& killTolerance : props->KillTolerances) {
		if (proj->m_propertySheetPtr.equals((RtWeakPtr<void>*) & killTolerance.Projectile)) {
			return killTolerance.ExtraHeight;
		}
	}
	return 0;
}

RoofModule* RoofModule::GetInstance() {
	return (RoofModule*) GetBoard()->levelModuleManager->GetModule(RoofModule::GetRTClass());
}

void RoofModule::ModInit() {
	LOGI("RoofModule init");
	
	vftable = copyVFTable(getActualOffset(LEVELMODULE_VTBL), LEVELMODULE_VTBL_NFUNC);

	patchVFTable(vftable, (void*)RoofModule::GetRTClass, 0);

	GetRTClass();

	LOGI("RoofModule finish init");
}
