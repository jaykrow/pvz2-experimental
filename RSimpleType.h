#pragma once
#include "ReflectionFieldType.h"

namespace Reflection
{
	class RSimpleType
	{
	public:
		char pad_0000[52]; //0x0000
		uint32_t m_size; //0x0034
		char pad_0038[4]; //0x0038
		ReflectionFieldType m_type; //0x003C
	}; //Size: 0x0040

}