#pragma once
#include "RClass.h"
#include "memUtils.h"

#define RTOBJECT_GETTYPE_INDEX 0
#define RTOBJECT_FUNC1_INDEX 1
#define RTOBJECT_CONSTRUCT_INDEX 2
#define RTOBJECT_DESTRUCT_INDEX 3
#define RTOBJECT_ISTYPE_INDEX 4
#define RTOBJECT_FUNC5_INDEX 5
#define RTOBJECT_FUNC6_INDEX 6

typedef void* (*ParameterlessConstructorFunc)();

namespace Sexy
{
    class RtClass
    {
    public:
        const char* TypeName;
        class RtClass* m_parentType;
        void* m_fConstructor;
        char pad_0010[20]; //0x0010
        class Reflection::RClass* m_rclass; //0x0024

        RtClass()
        {
            typedef void(*rtClassCtorFunc)(void*);
            rtClassCtorFunc pFunc = (rtClassCtorFunc)getActualOffset(0x123D2D8);
            pFunc(this);
        }

        void SetRClass(Reflection::RClass* rclass);

        virtual void Function0() {};
        virtual void Function1() {};
        virtual void Function2() {};
        virtual void Function3() {};
        virtual void Function4() {};
        virtual void Function5() {};
        virtual void Function6() {};
        virtual void Function7() {};
        virtual void* RegisterClass(const char* typeName, RtClass* parentType, ParameterlessConstructorFunc ctor) {};
    };
}

typedef Sexy::RtClass* (*rtClassCtorFunc)();
typedef Sexy::RtClass* (*rtClassGetClassFunc)();
typedef void(*rtClassSetRClassFunc)(Sexy::RtClass*, Reflection::RClass*);