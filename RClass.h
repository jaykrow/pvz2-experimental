#pragma once
#include "RField.h"
#include <vector>

namespace Sexy
{
    class RtClass;
}

namespace Reflection
{
    class RClass
    {
    public:
        char pad_0000[52]; //0x0000
        uint32_t m_classSize; //0x0034
        char pad_0038[32]; //0x0038
        class Sexy::RtClass* m_rtclass; //0x0058
        class std::vector<RField*> m_fields; //0x005C

        // void DumpFields(bool dumpBase);
    };

    class RClassRef
    {
    public:
        virtual void Function0() {}
        virtual void Function1() {}
        virtual void Function2() {}
        virtual void Function3() {}
        virtual void Function4() {}
        virtual void Function5() {}
        virtual void Function6() {}
        virtual void Function7() {}
        virtual void Function8() {}
        virtual void Function9() {}
        virtual void Function10() {}
        virtual void Function11() {}
        virtual void Function12() {}
        virtual RClass* GetRClass() {}
    };
}