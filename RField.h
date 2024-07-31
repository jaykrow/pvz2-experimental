#pragma once
#include "RSimpleType.h"

namespace Reflection
{
	class RField
	{
	public:
		char pad_0000[44]; //0x0000
		std::string m_fieldName; //0x002C
		char pad_0038[12]; //0x0038
		uint32_t m_fieldOffset; //0x0044
		RSimpleType* m_fieldType; //0x0048
	};
}