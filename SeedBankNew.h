#pragma once

#include "UIWidget.h"
#include "SeedBankProperties.h"

class SeedBankNew : public SlidingWidget
{
public:
	uint8_t m_packetCount;
	int m_bankType;
	RtWeakPtr<SeedBankProperties> seedBankProps;
	std::vector<SexyString> m_autofillSeedTypes;
	std::vector<int> m_autofillSeedLevels;
	std::vector<int> m_autofillSeedMasteryLevels;
};

static_assert(sizeof(SeedBankNew) == 0x184);

