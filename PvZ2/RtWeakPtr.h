#pragma once

#include "../memUtils.h"

// THESE ARE 9.6 OFFSETS. IDC RN BECAUSE IM ONLY USING THIS CLASS TO SUBSTITUTE IN MEMORY

#ifdef V96
#define OFFSET_RTWEAKPTR_GETREGISTRY 0x10D99A4
#define OFFSET_RTWEAKPTR_UNKFUNC1 0x10DFA5C
#define OFFSET_RTWEAKPTR_UNKFUNC2 0x10DF644
#define OFFSET_RTWEAKPTR_EQUALS 0x24D110
#define OFFSET_RTWEAKPTR_ISVALIDPTR 0x279198
#define OFFSET_RTWEAKPTR_RESOLVE 0x10DB9C0
#define OFFSET_RTWEAKPTR_FROMOTHER 0x10C8BF8
#define OFFSET_RTWEAKPTR_SETWEAKPTR 0x10C88C8
#endif

#define V94
#ifdef V94
#define OFFSET_RTWEAKPTR_GETREGISTRY 0x10C6764
#define OFFSET_RTWEAKPTR_UNKFUNC1 0x10CC81C
#define OFFSET_RTWEAKPTR_UNKFUNC2 0x10CC404
#define OFFSET_RTWEAKPTR_EQUALS 0x25BE80
#define OFFSET_RTWEAKPTR_ISVALIDPTR 0x287E28
#define OFFSET_RTWEAKPTR_RESOLVE 0x10C8780
#define OFFSET_RTWEAKPTR_FROMOTHER 0x10B59B8
#define OFFSET_RTWEAKPTR_SETWEAKPTR 0x10B5688
#define OFFSET_RTWEAKPTR_MOVE 0x10B5970
#endif

template <typename T>
class RtWeakPtr
{
public:

	RtWeakPtr()
	{
		data1 = NULL;
		data2 = NULL;
	}

	~RtWeakPtr()
	{
		typedef void(*func)(RtWeakPtr*, int64_t);
		func pSetValue = (func)getActualOffset(OFFSET_RTWEAKPTR_SETWEAKPTR);
		pSetValue(this, 0LL);
	}

	RtWeakPtr(RtWeakPtr* other) {
		data1 = NULL;
		data2 = NULL;

		this->FromOther(other);
	}

	/*
	RtWeakPtr(RtWeakPtr&& other) {
		data1 = NULL;
		data2 = NULL;

		typedef void(*func)(RtWeakPtr*, RtWeakPtr*);
		func pFunc = (func)getActualOffset(OFFSET_RTWEAKPTR_MOVE);
		pFunc(this, &other);
	}

	RtWeakPtr(const RtWeakPtr &other) {
		data1 = NULL;
		data2 = NULL;

		typedef void(*func)(RtWeakPtr*, const RtWeakPtr&);
		func pSetValue = (func)getActualOffset(OFFSET_RTWEAKPTR_FROMOTHER);
		pSetValue(this, other);
	}
	*/

	/*bool Equals(RtWeakPtr<T>* other)
	{
		typedef bool(*eqOp)(RtWeakPtr<T>*, RtWeakPtr<T>*);
		eqOp pEqOp = (eqOp)GetActualOffset(OFFSET_RTWEAKPTR_EQUALS);
		return pEqOp(this, other);
	}*/

	//bool operator==(const RtWeakPtr<T>& other) const
	//{
	//	typedef bool(*eqOp)(RtWeakPtr<T>*, RtWeakPtr<T>*);
	//	eqOp pEqOp = (eqOp)GetActualOffset(OFFSET_RTWEAKPTR_EQUALS);
	//	return pEqOp(this, &other);
	//}

	//bool operator==(RtWeakPtr<T>& other)
	//{
	//	typedef bool(*eqOp)(RtWeakPtr<T>*, RtWeakPtr<T>*);
	//	eqOp pEqOp = (eqOp)GetActualOffset(OFFSET_RTWEAKPTR_EQUALS);
	//	return pEqOp(this, &other);
	//}

	/*
	RtWeakPtr<T>& operator=(const RtWeakPtr<T>& other)
	{
		/*typedef void(*func)(RtWeakPtr*, const RtWeakPtr&);
		func pSetValue = (func)getActualOffset(OFFSET_RTWEAKPTR_FROMOTHER);
		pSetValue(this, other);

		// This is how I've been using the assignment operator so far
		data1 = other.data1;
		data2 = other.data2;

		return *this;
	}
	*/

	/*RtWeakPtr<T>& operator=(RtWeakPtr<T>& other)
	{
		typedef void(*func)(RtWeakPtr*, RtWeakPtr&);
		func pSetValue = (func)getActualOffset(OFFSET_RTWEAKPTR_FROMOTHER);
		pSetValue(this, other);
	}

	RtWeakPtr<T>& operator=(RtWeakPtr<T>* other)
	{
		typedef void(*func)(RtWeakPtr*, RtWeakPtr*);
		func pSetValue = (func)getActualOffset(OFFSET_RTWEAKPTR_FROMOTHER);
		pSetValue(this, other);
	}

	RtWeakPtr<T>& operator=(const RtWeakPtr<T>* other)
	{
		typedef void(*func)(RtWeakPtr*, RtWeakPtr*);
		func pSetValue = (func)getActualOffset(OFFSET_RTWEAKPTR_FROMOTHER);
		pSetValue(this, other);
	}*/

	bool equals(RtWeakPtr<T>* other) {
		typedef bool(*eqOp)(RtWeakPtr<T>*, RtWeakPtr<T>*);
		eqOp pEqOp = (eqOp)GetActualOffset(OFFSET_RTWEAKPTR_EQUALS);
		return pEqOp(this, other);
	}

	bool is_invalid()
	{
		return data1 == NULL;
	}

	bool is_valid()
	{
		typedef bool(*func)(RtWeakPtr<T>*);
		func pFunc = (func)getActualOffset(OFFSET_RTWEAKPTR_ISVALIDPTR);
		return pFunc(this);
	}

	int type_id()
	{
		return (this->data2 >> 0x11) & 0x3FFF;
	}

	int GetSomething()
	{
		return this->data2 >> 0x1F;
	}

	void FromOther(RtWeakPtr* other)
	{
		typedef void(*func)(RtWeakPtr*, RtWeakPtr*);
		func pSetValue = (func)getActualOffset(OFFSET_RTWEAKPTR_FROMOTHER);
		pSetValue(this, other);
	}

	// Get the object this points to, if valid.
	T* get()
	{
		typedef void* (*getV8)();
		getV8 pGetRegistry = (getV8)getActualOffset(OFFSET_RTWEAKPTR_GETREGISTRY);

		void* registry = pGetRegistry();

		typedef void (*sub10CC81C)(RtWeakPtr*, void*, RtWeakPtr*);
		sub10CC81C pSub10CC81C = (sub10CC81C)getActualOffset(OFFSET_RTWEAKPTR_UNKFUNC1);

		RtWeakPtr<T> temp;
		pSub10CC81C(&temp, registry, this);

		// LOGI("Every check passed so far %d %d", data1, data2);

		T* retVal = NULL;
		if (!temp.is_invalid())
		{
			// LOGI("Not invalid");
			//typedef void* (*sub10B5924)(RtWeakPtr*);
			//sub10B5924 pSub10B5924 = (sub10B5924)GetActualOffset(0x10B5924);

			//void* v10 = pSub10B5924(&temp);
			int typeId = temp.type_id();

			typedef void* (*sub10CC404)(void*, int);
			sub10CC404 pSub10CC404 = (sub10CC404)getActualOffset(OFFSET_RTWEAKPTR_UNKFUNC2);

			void* v11 = pSub10CC404(registry, typeId);
			if (v11)
			{
				// LOGI("is correct type");
				typedef T* (*sub10C8780)(void*, RtWeakPtr*);
				sub10C8780 pResolveWeakPtr = (sub10C8780)getActualOffset(OFFSET_RTWEAKPTR_RESOLVE);
				retVal = pResolveWeakPtr(v11, &temp);
			}
		}
		//temp.MarkForDeletion();

		return retVal;
	}

private:
	int data1;
	int data2;
};

int getWeakPtr(int addr);
