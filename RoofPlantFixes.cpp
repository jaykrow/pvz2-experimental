#include "pch.h"
#include "RoofPlantFixes.h"
#include "PlantFramework.h"
#include "Graphics.h"
#include "RoofHooks.h"
#include "PvZ2/Board.h"
#include "SexyApp.h"
#include "RoofStage.h"

typedef void (*plantSpikeweedRenderRigs)(PlantSpikeweed*, Graphics*);
plantSpikeweedRenderRigs oPlantSpikeweedRenderRigs = NULL;

void hkPlantSpikeweedRenderRigs(PlantSpikeweed* plant, Graphics* graphics) {
	if (plant->m_plantfoodSpikesActive) {
		int offsetX = 0x87;

		for (int i = 0; i < 9; ++i, offsetX += GRID_TILE_WIDTH) {
			if (plant->m_plantfoodSpikes[i].is_valid()) {
				float offsetY = GRID_TILE_HEIGHT * plant->mPlant->m_row;

				float mX = GRID_TILE_LEFT + GRID_TILE_WIDTH / 2 + GRID_TILE_WIDTH * i;
				offsetY -= GetSlopeOffset(mX, offsetY);

				auto sexyApp = SexyApp::Get();
				float scaledX = sexyApp->Scale(offsetX);
				float scaledY = sexyApp->Scale(offsetY + 0x68);

				// LOGI("ScaledX = %f ScaledY = %f", scaledX, scaledY);

				graphics->mTransX += scaledX;
				graphics->mTransY += scaledY;

				auto animRig = plant->m_plantfoodSpikes[i].get();
				animRig->Render(graphics);

				graphics->mTransX -= scaledX;
				graphics->mTransY -= scaledY;
			}
		}
	}

	hkPlantRenderRigs(plant->mPlant, graphics);
}

void initRoofPlantFixes() {
	PVZ2HookFunction(0xB02BB0, (void*)hkPlantSpikeweedRenderRigs, (void**)&oPlantSpikeweedRenderRigs, "plant spikeweed render");
}