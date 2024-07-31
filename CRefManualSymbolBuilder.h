#pragma once
#include "RClass.h"
#include "ReflectionFieldType.h"
#include "DelegateBase.h"
#include "memUtils.h"
#include "Logging.h"

namespace Reflection
{
    class CRefManualSymbolBuilder
    {
    public:
        typedef void(*BuildSymbolsFunc)(Reflection::CRefManualSymbolBuilder*, Reflection::RClass*);

        void GetInnerTemplateTypeName(const SexyString& typeName)
        {
            typedef void(*func)(SexyString&, const SexyString&);
            func pFunc = (func)getActualOffset(0x121F944);
            SexyString outStr;
            pFunc(outStr, typeName);
            LOGI("Out inner typename: %s", outStr.c_str());
        }

        /*void GetMapInnerTemplateTypeName(const SexyString& typeName)
        {
            typedef void(*func)(SexyString&, const SexyString&);
            func pFunc = (func)GetActualOffset(0x121FACC);
            SexyString outStr;
            pFunc(outStr, typeName);
            LOGI("Out inner typename: %s", outStr.c_str());
        }*/

        virtual void Function0() {};
        virtual void Function1() {};
        virtual void Function2() {};
        virtual void Function3() {};
        virtual void Function4() {};
        virtual void RegisterClassWithProperties(const SexyString& className, BuildSymbolsFunc buildPropsFunc, uint classSize, int unk) {};
        virtual void Function6() {};
        virtual void* GetPropertyOfType(ReflectionFieldType type, uint size) {};
        virtual void Function8() {};
        virtual void Function9() {};
        virtual void* RegisterPointerProperty(int, void*, void*) {};
        virtual RClassRef* GetClassRef(const SexyString& className) {};
        virtual void SetRClassWithParent(void*, void*, int) {};
        virtual void RegisterProperty(void* rClass, const SexyString& name, uint offset, void* type) {};
        virtual void Function14() {};
        virtual void RegisterCallback(void* rClass, const SexyString& eventName, Sexy::DelegateBase* delegate, int unk, int unk2) {};
    };
}
