#pragma once

#define OFFSET_GMESSAGEROUTER 0x1C85050

#define OFFSET_MESSAGEROUTER_REGISTERCALLBACK 0x5FDA54
#define OFFSET_MESSAGEROUTER_DEREGISTERCALLBACK 0x5FDE64
// #define OFFSET_MESSAGEROUTER_UNKFUNC_5F1AC0 0x5F1AC0 // 9.6 address // 0x5FDDE0
#define OFFSET_MESSAGEROUTER_VTBL 0x01BD7BE0 

#include "Delegate.h"
#include "memUtils.h"

//typedef std::vector<TDelegate<void(GC::*)()>> DelegatesVec;
typedef std::vector<TDelegate<void(GC::*)()>> DelegatesVec;

class MessageRouter
{
public:
	virtual void Function0() {};
	virtual void Function1() {};
	virtual void Function2() {};
	virtual DelegatesVec& GetListenersForMessage(int msgId) {};

	template <typename T, typename F>
	void RegisterCallback(int msgId, T& obj, F func)
	{
		auto dlgt = make_delegate(func, obj);
		typedef void(*registerFunc)(MessageRouter*, int, void*);
		registerFunc pFunc = (registerFunc)GetActualOffset(OFFSET_MESSAGEROUTER_REGISTERCALLBACK);
		pFunc(this, msgId, &dlgt);
	}

	template <typename T, typename F>
	void RegisterCallback(int msgId, T* obj, F func)
	{
		auto dlgt = make_delegate(func, *obj);
		typedef void(*registerFunc)(MessageRouter*, int, void*);
		registerFunc pFunc = (registerFunc)GetActualOffset(OFFSET_MESSAGEROUTER_REGISTERCALLBACK);
		pFunc(this, msgId, &dlgt);
	}

	void DeregisterCallbacksOwnedByObject(void* obj)
	{
		typedef void(*deregisterFunc)(MessageRouter*, void*);
		deregisterFunc pFunc = (deregisterFunc)GetActualOffset(OFFSET_MESSAGEROUTER_DEREGISTERCALLBACK);
		pFunc(this, obj);
	}

	template <typename... Params>
	int ExecuteMessage(int msgId, Params... args)
	{
		DelegatesVec& listeners = GetListenersForMessage(msgId);

		int unk = unkInt + 1;
		unkInt = unk;

		for (DelegatesVec::iterator it = listeners.begin(); it != listeners.end(); ++it)
		{
			// WTF
			TDelegate<void(GC::*)()> delegateWithoutParams = *it;
			void* voidPtr = reinterpret_cast<void*>(&delegateWithoutParams);
			TDelegate<void(GC::*)(Params...)> dlgt = *reinterpret_cast<TDelegate<void(GC::*)(Params...)>*>(voidPtr);

			// Unholy
			dlgt.executeCallbackFunc_(& dlgt, args...);

			// TODO: Revise this to actually use the proper delegate system
			/*
			Sexy::DelegateBase* a = (Sexy::DelegateBase*)&dlgt;

			using foo = void(*)(uint, Params...);
			uint callbackAddr = a->callbackFunc;
			if (a->useOwnerVtable) {
				callbackAddr = *(uint*)a->callbackOwner + a->callbackFunc;
			}
			foo callback = reinterpret_cast<foo>(callbackAddr);

			callback(a->callbackOwner, args...);
			*/
		}

		int result = unk - 1;
		unkInt = result;
		if (!result)
		{
			typedef int(*func)(MessageRouter*);
			func pFunc = (func)GetActualOffset(0x5FDDE0);
			return pFunc(this);
		}
		return result;
	}

	static MessageRouter* GetInstance()
	{
		return *(MessageRouter**)GetActualOffset(OFFSET_GMESSAGEROUTER);
	}
private:
	int unk[9];
	int unkInt;
};