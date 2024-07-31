#pragma once
#include "RtObject.h"
#include "PvZ2/RtWeakPtr.h"
#include "ReflectionBuilder.h"
#include "memUtils.h"

#define GAMEOBJECT_CONSTRUCTOR_OFFSET 0x54B4BC
#define GAMEOBJECT_GETTYPE_OFFSET 0x54AF8C
#define GAMEOBJECT_FUNCTION_DIE_OFFSET 0x54BA18

using namespace Sexy;

class Renderable
{
public:
    virtual ~Renderable() {};
    virtual void Draw(void* g) {};
    virtual int GetRenderOrder() {};
};

class GameObject : public RtObject
{
public:
    DEFINE_RT_CLASS_GET_CLASS_WRAPPER(GAMEOBJECT_GETTYPE_OFFSET)

    int unk_0x4;
    RtWeakPtr<GameObject> m_thisPtr;

    void Die()
    {
        typedef void(*func)(void*);
        func pFunc = (func)getActualOffset(GAMEOBJECT_FUNCTION_DIE_OFFSET);
        pFunc(this);
    }

    // Called when the object is serialized from a JSON
    virtual void RegisterForEvents() {};
    virtual void Function8() {};
    virtual void Function9() {};
    virtual void Function10() {};
    virtual void Function11() {};
    virtual void Destroy() {};
};