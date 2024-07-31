#include "ReflectionBuilder.h"
#include "RtClass.h"
#include "memUtils.h"

void ForceRTClassRebuild(uint classPtr, uint getClassFunc)
{
    Sexy::RtClass** rtClass = reinterpret_cast<Sexy::RtClass**>(getActualOffset(classPtr));
    *rtClass = nullptr;
    rtClassCtorFunc pFunc = (rtClassCtorFunc)getActualOffset(getClassFunc);
    pFunc();
}

void ReplaceRTClassPtr(uint classPtr, Sexy::RtClass* newClassPtr)
{
    Sexy::RtClass** rtClass = reinterpret_cast<Sexy::RtClass**>(getActualOffset(classPtr));
    *rtClass = newClassPtr;
}