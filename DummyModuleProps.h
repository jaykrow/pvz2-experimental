#pragma once
#include "SeedBankProperties.h"

class DummyModuleProps : public SeedBankProperties
{
	DECLARE_STATIC_RT_CLASS_MEMBERS(DummyModuleProps);

	DEFINE_RT_CLASS_BUILD_SYMBOLS_WITHOUT_CLASS_PROPERTIES(SeedBankProperties);

	DEFINE_RT_CLASS_CONSTRUCT_FUNCTION(DummyModuleProps, 0xD56634);

	DEFINE_RT_CLASS_GET_CLASS_FUNCTION(DummyModuleProps, 0xD55E8C);

	DEFINE_RT_CLASS_REGISTER_CLASS_FUNCTION(DummyModuleProps);

	static void* GetModuleType();
};

