#pragma once

namespace Reflection
{
    enum ReflectionFieldType
    {
        Type_Unknown = 0x0,
        Type_Elipsis = 0x1,
        Type_Void = 0x2,
        Type_Bool = 0x3,
        Type_Char = 0x4,
        Type_Wchar_t = 0x5,
        Type_IntType = 0x6,
        Type_UIntType = 0x7,
        Type_FloatType = 0x8,
        Type_HRESULT = 0x9,
    };
}