#include "pch.h"
#include "RoofHooks.h"
#include "PvZ2/Board.h"
#include "memUtils.h"
#include "PacketCursor.h"
#include "ZombieParticle.h"
#include "RoofModule.h"
#include "RoofModuleProps.h"
#include "Graphics.h"
#include "PopAnim.h"
#include "TimeMgr.h"
#include "PlantPropertySheet.h"
#include "RoofPlantFixes.h"
#include "RoofStage.h"
#include "RoofStageProps.h"
#include "SexyApp.h"

float getKillHeight(float mX, float mY) {
	auto roofStage = RoofStage::Get();
	if (roofStage) {
		return roofStage->GetKillDepth(mX, mY);
	}
	else {
		return -MAXFLOAT;
	}
}

typedef void (*plantCursorGetGridCoords)(int*, int);
plantCursorGetGridCoords oPlantCursorGetGridCoords = NULL;

void hkPlantCursorGetGridCoords(int* res, int cursor) {
	int mX = *(int*)(cursor + 4);
	int mY = *(int*)(cursor + 8);
	*(int*)(cursor + 8) += GetSlopeOffset(mX, mY);

	oPlantCursorGetGridCoords(res, cursor);

	*(int*)(cursor + 8) = mY;
}

typedef int (*convertCursorToGridX)(int);
convertCursorToGridX oConvertCursorToGridX = NULL;

// todo: this is stupidly hacky but its the cleanest solution since getGridX is always called before getGridY

int savedX;
int hkConvertCursorToGridX(int x) {
	savedX = x;
	return oConvertCursorToGridX(x);
}

typedef int (*convertCursorToGridY)(int);
convertCursorToGridY oConvertCursorToGridY = NULL;

int hkConvertCursorToGridY(int y) {
	y += GetSlopeOffset(savedX, y);

	return oConvertCursorToGridY(y);
}

typedef void (*drawBoardCursorHighlights)(Board*, int);
drawBoardCursorHighlights oDrawBoardCursorHighlights = NULL;

void hkDrawBoardCursorHighlights(Board* board, int graphicsState) {
	for (void* cursorPtr : board->cursors) {
		int cursor = (int) cursorPtr;
		if (!*(bool*)(cursor + 0x1C)) {
			int mY = *(int*)(cursor + 8);
			int mX = *(int*)(cursor + 4);

			typedef bool (*isType)(int, Sexy::RtClass*);
			isType pIsType = *(isType*)(*(uint*)cursor + 0x10);
			
			typedef Sexy::RtClass* (*getType)();
			auto packetCursorType = PacketCursor::GetRTClass();
			if (pIsType(cursor, packetCursorType)) {
				PacketCursor* cursorCasted = (PacketCursor*)cursor;

				typedef bool (*sub405058)(PacketCursor*); // some kind of boundary checking function?
				if (((sub405058)getActualOffset(0x405058))(cursorCasted) || !cursorCasted->CanPlantOnCursor()) {
					continue;
				}

				typedef void (*getCursorCoords)(int*, int);
				getCursorCoords pFunc = *(getCursorCoords*)(*(uint*)cursor + 0x34);
				int res[2];
				pFunc(res, cursor);

				typedef void (*drawHighlights)(Board*, int, int, int);
				((drawHighlights)getActualOffset(0x7367AC))(board, graphicsState, res[0], res[1]);
				continue;
			}

			auto shovelCursorType = ((getType)getActualOffset(0x104B01C))();
			auto pfCursorType = ((getType)getActualOffset(0x8A649C))();

			typedef bool (*sub405058)(int); // is in bounds function
			if ((pIsType(cursor, shovelCursorType) || pIsType(cursor, pfCursorType)) && 
				((sub405058)getActualOffset(0x405058))(cursor)) {
				continue;
			}

			int gridX = hkConvertCursorToGridX(mX);
			int gridY = hkConvertCursorToGridY(mY);

			if (gridX < 0 || gridY < 0) {
				return;
			}

			typedef void (*drawHighlights)(Board*, int, int, int);
			((drawHighlights)getActualOffset(0x7367AC))(board, graphicsState, gridX, gridY);
		}
	}
}

typedef void (*fun292)(int);
fun292 oFun292 = NULL;

void hkFun292(int a1) {
	LOG_RETURN_ADDRESS(0);
	oFun292(a1);
}

typedef float (*realObjectGetSinkDist)(RealObject*, SexyVector3*);
realObjectGetSinkDist oRealObjGetSinkDist = NULL;

float hkRealObjectGetSinkDist(RealObject* a1, SexyVector3* a2) { // this should constraint board entities
	float ans = oRealObjGetSinkDist(a1, a2);
	// now add the roof sinking
	float offset = GetSlopeOffset(a2->mX, a2->mY);
	
	/*
	typedef Sexy::RtClass* (*GridItemAnimGetClass)();
	auto animClass = ((GridItemAnimGetClass)getActualOffset(0x411984))();
	if (a1->IsType(animClass)) { // rendering of these do not scale mZ up. Should probably be fixed at the root
		// this brings another problem: death anim now renders at a weird offset
		offset = SexyApp::Get()->Scale(offset);
	}
	*/

	ans += offset;

	return ans;
}

typedef float (*projUpdatePosition)(Projectile*, SexyVector3*);
projUpdatePosition oProjUpdatePosition = NULL;

float hkProjUpdatePosition(Projectile* proj, SexyVector3* pos) {
	LOGI("current position = %f %f %f", proj->m_position.mX, proj->m_position.mY, proj->m_position.mZ);
	/*if (pos->mX < GRID_TILE_LEFT + 5 * GRID_TILE_WIDTH) {
		pos->mZ -= ((GRID_TILE_LEFT + 5 * GRID_TILE_WIDTH) - pos->mX) / 4;
	}*/
	LOG_RETURN_ADDRESS(0);
	return oProjUpdatePosition(proj, pos);
}

typedef Projectile* (*plantCreateProj)(Plant*);
plantCreateProj oPlantCreateProj = NULL;

Projectile* hkPlantCreateProj(Plant* plant) {
	Projectile* res = oPlantCreateProj(plant);
	if (res->m_position.mX < GRID_TILE_LEFT + 5 * GRID_TILE_WIDTH) {
		res->m_position.mZ -= ((GRID_TILE_LEFT + 5 * GRID_TILE_WIDTH) - res->m_position.mX) / 4;
	}
	return res;
}

typedef void (*plantRenderShadows)(Plant*, int);
plantRenderShadows oPlantRenderShadows = NULL;

// TODO: just edit plant and zombies' shadow code to be the same as projectiles
void hkPlantRenderShadows(Plant* plant, int graphicsState) {
	// TODO: take into account relocation code
	float ogY = plant->m_position.mY;
	plant->m_position.mY -= GetSlopeOffset(plant->m_position.mX, plant->m_position.mY);
	oPlantRenderShadows(plant, graphicsState);
	plant->m_position.mY = ogY;
}

typedef void (*zombieRenderShadows)(Zombie*, int);
zombieRenderShadows oZombieRenderShadows = NULL;

void hkZombieRenderShadows(Zombie* zombie, int graphicsState) {
	float ogY = zombie->m_position.mY;
	zombie->m_position.mY -= GetSlopeOffset(zombie->m_position.mX, zombie->m_position.mY);
	oZombieRenderShadows(zombie, graphicsState);
	zombie->m_position.mY = ogY;
}

typedef void (*projRenderShadows)(Projectile*, int);
projRenderShadows oProjRenderShadows = NULL;

void hkProjectileRenderShadows(Projectile* proj, int graphicsState) {
	float ogY = proj->m_position.mY;
	proj->m_position.mY -= GetSlopeOffset(proj->m_position.mX, proj->m_position.mY);
	oProjRenderShadows(proj, graphicsState);
	proj->m_position.mY = ogY;
}

typedef void (*projUpdateVelocity)(Projectile*, float);
projUpdateVelocity oProjUpdateVelocity = NULL;

void hkProjectileUpdateVelocity(Projectile* proj, float a2) { // a2 is timeStep
	oProjUpdateVelocity(proj, a2);

	auto roofModule = RoofModule::GetInstance();

	if (roofModule) {
		bool followsSlope = roofModule->IsProjectileFollowsSlope(proj);

		if (followsSlope) {
			proj->UpdateSinkDistance(false);
		}

		if (!roofModule->IsProjectileImmune(proj) && !followsSlope) {
			float killHeight = getKillHeight(proj->m_position.mX, proj->m_position.mY);
			int tolerance = roofModule->GetProjectileTolerance(proj);

			if (proj->m_position.mZ + tolerance < killHeight) {
				proj->PlayImpact(NULL);
				proj->Die();
			}
		}
	}
}

typedef Projectile* (*addProjectileByType)(Board*, float, float, float, RtWeakPtr<ProjectilePropertySheet>*, BoardEntity*);
addProjectileByType oAddProjectileByType = NULL;

Projectile* hkAddProjectileByType(Board* board, float mX, float mY, float mZ, RtWeakPtr<ProjectilePropertySheet>* type, BoardEntity* owner) {
	if (owner && owner->IsType(Plant::GetRTClass())) {
		auto roofModule = RoofModule::GetInstance();
		if (roofModule) {
			if (!roofModule->IsProjectileSpawnIgnoresSlope(type)) {
				mZ += owner->m_position.mZ;
				LOGI("Projectile doesn't ignore slope, spawns at %f", mZ);
			}
		}
	}

	return oAddProjectileByType(board, mX, mY, mZ, type, owner);
}

typedef void (*setFromResource)(Effect_PopAnim*, int, int);
setFromResource oSetFromResource = NULL;

float SexyAppScale(int gSexyApp, float a2) {
	return *(float*)(gSexyApp + 0x698) * a2;
}

float SexyAppUnscale(int gSexyApp, float a2) {
	return a2 / *(float*)(gSexyApp + 0x698);
}

void hkSetFromResource(Effect_PopAnim* a1, int a2, int objClass) {
	int offsets[2];
	auto roofModule = RoofModule::GetInstance();
	if (roofModule && roofModule->IsEffectSloped(offsets, (RtWeakPtr<void>*)(a2 + 8))) {
		float mX = SexyAppUnscale(getSexyApp(), a1->screenSpaceOrigin.mX);
		float mY = SexyAppUnscale(getSexyApp(), a1->screenSpaceOrigin.mY);
		float slopeHeight = GetSlopeOffset(mX + offsets[0], mY + offsets[1]);
		a1->screenSpaceOrigin.mY -= SexyAppScale(getSexyApp(), slopeHeight);
	}

	oSetFromResource(a1, a2, objClass);
}

typedef void (*setPositionAndLayer)(Effect_PopAnim*, SexyVector3*, int);
setPositionAndLayer oSetPositionAndLayer = NULL;

void hkSetPositionAndLayer(Effect_PopAnim* a1, SexyVector3* a2, int layer) {
	if (a1->animRig.is_valid()) {
		PopAnimRig* rig = a1->animRig.get();
		RtWeakPtr<void> rigPamRtId;
		rigPamRtId.FromOther(& rig->popAnim->pamRtId);

		int offsets[2];
		auto roofModule = RoofModule::GetInstance();
		if (roofModule && roofModule->IsEffectSloped(offsets, &rigPamRtId)) {
			float slopeHeight = GetSlopeOffset(a2->mX + offsets[0], a2->mY + offsets[1]);
			a2->mZ += slopeHeight;
		}
	}

	oSetPositionAndLayer(a1, a2, layer);
}

typedef void (*plantRenderRigs)(Plant*, Graphics*);
plantRenderRigs oPlantRenderRigs = NULL;

void hkPlantRenderRigs(Plant* plant, Graphics* graphics) {
	if ((plant->m_realObjectFlags & 4) == 0) {
		graphics->UnkFunc();

		plant->UpdateGroundClipRect(graphics);

		float v8 = 0.0; // I think this is rotation related
		if (plant->m_relocating) {
			typedef float (*subE81EE4)(Plant*);
			v8 = ((subE81EE4)getActualOffset(0xE81EE4))(plant);

			float v6 = SexyAppScale(getSexyApp(), plant->m_relocateToX + plant->m_relocationOffsetFromDestination.mX);
			graphics->mTransX += graphics->mScaleOrigX + v6;

			float v9 = SexyAppScale(getSexyApp(), plant->m_relocateToY + plant->m_relocationOffsetFromDestination.mY);
			graphics->mTransY += graphics->mScaleOrigY + v9;
		}
		else {
			float mXScaled = SexyAppScale(getSexyApp(), plant->m_position.mX);
			graphics->mTransX += graphics->mScaleOrigX + (mXScaled - graphics->mScaleOrigX) * graphics->mScaleX;

			float mYScaled = SexyAppScale(getSexyApp(), plant->m_position.mY);
			graphics->mTransY += graphics->mScaleOrigY + (mYScaled - graphics->mScaleOrigY) * graphics->mScaleY;
		}
		
		float ogTransX = graphics->mTransX;
		float ogTransY = graphics->mTransY;

		PopAnimRig* plantfoodShine = plant->m_plantFoodShine.get();
		if (plantfoodShine->m_state) {
			float v21 = SexyAppScale(getSexyApp(), -68.0);
			float v23 = SexyAppScale(getSexyApp(), -160.0);

			// Added code to render plantfoodShine at Z offset
			float v40 = CallVirtualFunc<float, void*>(plant->plantFramework, 13, plant->plantFramework);
			graphics->mTransY += SexyAppScale(getSexyApp(), v40 - plant->m_position.mZ) * graphics->mScaleY;
			// end of added code

			Sexy::Transform2D transMatrix;
			transMatrix.mat[0][0] *= graphics->mScaleX * 1.5;
			transMatrix.mat[0][1] *= graphics->mScaleX * 1.5;
			transMatrix.mat[0][2] = v21 * graphics->mScaleX * 1.5 + graphics->mScaleX * transMatrix.mat[0][2];
			transMatrix.mat[1][0] *= graphics->mScaleY * 1.5;
			transMatrix.mat[1][1] *= graphics->mScaleY * 1.5;
			transMatrix.mat[1][2] = graphics->mScaleY * 1.5 * transMatrix.mat[1][2] + v23 * graphics->mScaleY * 1.5;
			plantfoodShine->TransformAndRender(graphics, transMatrix);

			graphics->mTransX = ogTransX;
			graphics->mTransY = ogTransY;
		}

		PopAnimRig* animRig = plant->m_animRig.get();
		if (animRig) {
			Color color = animRig->GetColor();
			if (plant->m_fadeInEndTime < MAXFLOAT) {
				float fadeInStartTime = plant->m_fadeInEndTime - plant->m_fadeInDuration;
				float opacity = (TimeMgr::GetInstance()->m_curTime - fadeInStartTime) / plant->m_fadeInDuration;
				if (opacity < 0) {
					opacity = 0;
				}
				else {
					opacity = std::min(opacity, 1.0f);

					/*
					typedef void (*interpolateOpacity)(float opacity, int type);
					opacity = ((interpolateOpacity)getActualOffset(0x46CDFC))(opacity, 2);
					*/
					opacity *= opacity;
				}

				color.alpha = opacity * 255;
				animRig->SetColor(color);
			}

			float v40 = CallVirtualFunc<float, void*>(plant->plantFramework, 13, plant->plantFramework);
			graphics->mTransY += SexyAppScale(getSexyApp(), v40 - plant->m_position.mZ) * graphics->mScaleY;

			float sproutScale = 1.0f;

			if (plant->m_sproutScalingEndTime < MAXFLOAT) {
				float sproutScalingStartTime = plant->m_sproutScalingEndTime - plant->m_sproutScalingDuration;
				float scale = (TimeMgr::GetInstance()->m_curTime - sproutScalingStartTime) / plant->m_sproutScalingDuration;
				scale = std::max(scale, 0.0f);
				scale = std::min(scale, 1.0f);
				scale *= scale;
				if (TimeMgr::GetInstance()->m_curTime >= plant->m_sproutScalingEndTime) {
					plant->m_sproutScalingEndTime = MAXFLOAT;
				}
				sproutScale = scale;
			}

			Sexy::Transform2D transMatrix;
			if (v8 != 0.0f) {
				int v80[4];
				memset(v80, 0, sizeof(v80));
				// LOGI("before virtual func call v8 = %f", v8);
				// LOGI("anim rig = %u anim rig func 16 = %u vtbl = %u", (uint)animRig, getOriginalOffset((uint) GetVirtualFunc(animRig, 16)), getOriginalOffset(*(uint*)animRig));
				CallVirtualFunc<void, void*, int*>(animRig, 16, animRig, v80);
				// LOGI("After call");
				float v52 = v80[0] + v80[2] * 0.5;
				float v53 = v80[1] + v80[3] * 0.5;
				transMatrix.mat[0][2] -= v52;
				transMatrix.mat[1][2] -= v53;
				typedef void (*rotate)(Sexy::Transform2D*, float);
				((rotate)getActualOffset(0x4A05BC))(&transMatrix, v8);
				transMatrix.mat[0][2] += v52;
				transMatrix.mat[1][2] += v53;
			}

			float v54 = sproutScale * plant->m_baseScale * graphics->mScaleX;
			if (plant->m_mirror) {
				transMatrix.mat[0][0] *= -v54;
				transMatrix.mat[0][1] *= -v54;
				transMatrix.mat[0][2] *= -v54;
				transMatrix.mat[1][0] *= v54;
				transMatrix.mat[1][1] *= v54;
				transMatrix.mat[1][2] *= v54;

				auto props = plant->GetProps<PlantPropertySheet>();
				float scaledCenterX = SexyAppScale(getSexyApp(), props->ArtCenter.mX * -v54);
				float scaledCenterY = SexyAppScale(getSexyApp(), props->ArtCenter.mY * v54);
				transMatrix.mat[0][2] -= scaledCenterX;
				transMatrix.mat[1][2] -= scaledCenterY;
			}
			else {
				for (int i = 0; i < 2; ++i) {
					for (int j = 0; j < 3; ++j) {
						transMatrix.mat[i][j] *= v54;
					}
				}
				auto props = plant->GetProps<PlantPropertySheet>();
				float scaledCenterX = SexyAppScale(getSexyApp(), props->ArtCenter.mX * v54);
				float scaledCenterY = SexyAppScale(getSexyApp(), props->ArtCenter.mY * v54);
				transMatrix.mat[0][2] -= scaledCenterX;
				transMatrix.mat[1][2] -= scaledCenterY;
			}

			animRig->TransformAndRender(graphics, transMatrix);
		}

		graphics->UnkFunc2();
	}
}

typedef void (*gridItemAnimRender)(BoardEntity*, Graphics*);
gridItemAnimRender oGridItemAnimRender = NULL;

void hkGridItemAnimRender(BoardEntity* entity, Graphics* graphics) { // mZ does not get scaled in the code for whatever reason
	float ogmZ = entity->m_position.mZ;
	entity->m_position.mZ = SexyApp::Get()->Scale(entity->m_position.mZ);
	oGridItemAnimRender(entity, graphics);
	entity->m_position.mZ = ogmZ;
}

typedef void (*fun302)(int, Zombie*, int, char);
fun302 oFun302 = NULL;

void hkFun302(int a1, Zombie* a2, int a3, char a4) {
	if (a2->IsType(Zombie::GetRTClass())) {
		return;
	}
	oFun302(a1, a2, a3, a4);
}

typedef void (*fun303)(Zombie*, Graphics*);
fun303 oFun303 = NULL;

void hkFun303(Zombie* a1, Graphics* a2) {
	// a1->m_realObjectFlags &= ~1u;

	oFun303(a1, a2);

	a1->m_realObjectFlags |= 1;
}

typedef void (*zombieClipGroundEffect)(Rect*, Zombie*, char*);
zombieClipGroundEffect oZombieClipGroundEffect = NULL;

void hkZombieClipGroundEffect(Rect* a1, Zombie* a2, char* a3) {
	float ogY = a2->m_position.mY;
	a2->m_position.mY -= GetSlopeOffset(a2->m_position.mX, a2->m_position.mY);
	oZombieClipGroundEffect(a1, a2, a3);
	a2->m_position.mY = ogY;
}

void initRoofHooks() {
	
	PVZ2HookFunction(0xCF3BE4, (void*)hkPlantCursorGetGridCoords, (void**)&oPlantCursorGetGridCoords, "plant cursor get grid coords");
	PVZ2HookFunction(0x6404B0, (void*)hkConvertCursorToGridX, (void**)&oConvertCursorToGridX, "convert cursor to grid x");
	PVZ2HookFunction(0x6404CC, (void*)hkConvertCursorToGridY, (void**)&oConvertCursorToGridY, "convert cursor to grid y");

	PVZ2HookFunction(0x73580C, (void*)hkDrawBoardCursorHighlights, (void**)&oDrawBoardCursorHighlights, "draw board all cursors highlights");

	// PVZ2HookFunction(0xE8B690, (void*)hkFun292, (void**)&oFun292, "fun 292");
	PVZ2HookFunction(0xCDFFD0, (void*)hkRealObjectGetSinkDist, (void**)&oRealObjGetSinkDist, "real object get sink dist");

	// now to fix projectile spawns. Override getProjectileOffset
	// PVZ2HookFunction(0x2ABF94, (void*)hkProjUpdatePosition, (void**)&oProjUpdatePosition, "projectile update pos");
	// PVZ2HookFunction(0xE864FC, (void*)hkPlantCreateProj, (void**)&oPlantCreateProj, "plant create projectile");
	// zombie created projectiles are probably fine since they needed to support tides already
	// still need to handle PF projectiles

	PVZ2HookFunction(0xE84E7C, (void*)hkPlantRenderShadows, (void**)&oPlantRenderShadows, "plant render shadows");
	PVZ2HookFunction(0x8BFE78, (void*)hkZombieRenderShadows, (void**)&oZombieRenderShadows, "zombie render shadows");
	PVZ2HookFunction(0x91537C, (void*)hkProjectileRenderShadows, (void**)&oProjRenderShadows, "projectile render shadows");

	PVZ2HookFunction(0x912FA0, (void*)hkProjectileUpdateVelocity, (void**)&oProjUpdateVelocity, "projectile update velocity");

	// TODO: we got some more shit like zombie particles and fallen sun (from shovel) to deal with

	// TODO: Uhh, recreate the ActivatePlantfood and CreateProjectile functions to pass the Plant itself into offsets. Orrr
	//	just override them and the GetProjectileOffset function to maintain the Plant

	// TODO: DrawPlantingHighlights is called from another source: vasebreaker powerup stuff. Idc rn
	
	PVZ2HookFunction(0x72B08C, (void*)hkAddProjectileByType, (void**)&oAddProjectileByType, "add projectile by type");
	// PVZ2HookFunction(0x1246A14, (void*)hkFun301, (void**)&oFun301, "fun 301");
	PVZ2HookFunction(0xE47544, (void*)hkSetFromResource, (void**)&oSetFromResource, "set from resource");
	PVZ2HookFunction(0x7401A8, (void*)hkSetPositionAndLayer, (void**)&oSetPositionAndLayer, "set position and layer");
	
	// PVZ2HookFunction(0xC90A14, (void*)hkFun300, (void**)&oFun300, "fun 300");
	PVZ2HookFunction(0xE852BC, (void*)hkPlantRenderRigs, (void**)&oPlantRenderRigs, "plant render rigs");

	PVZ2HookFunction(0x412EDC, (void*)hkGridItemAnimRender, (void**)&oGridItemAnimRender, "grid item anim render");

	// PVZ2HookFunction(0xFE4E4C, (void*)hkFun302, (void**)&oFun302, "fun 302");
	// PVZ2HookFunction(0x8BE450, (void*)hkFun303, (void**)&oFun303, "fun 303");
	PVZ2HookFunction(0xCDFE08, (void*)hkZombieClipGroundEffect, (void**)&oZombieClipGroundEffect, "zombie ground effect clip");

	RoofModule::ModInit();
	RoofModuleProps::ModInit();

	RoofStage::ModInit();
	RoofStageProps::ModInit();

	initRoofPlantFixes();
}