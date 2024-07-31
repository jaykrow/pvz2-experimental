#pragma once
#include "BoardEntity.h"
#include "Plant.h"

class PlantGroup : public BoardEntity
{
public:
	int m_gridX; // 0x64
	int m_gridY; // 0x68
	bool m_isOnBoard; // 0x6C
	bool m_groupPlantfoodRender;
	std::vector<RtWeakPtr<Plant>> m_plants;
};