#pragma once

#include "PvZ2/RtWeakPtr.h"
#include "RType.h"

struct UnkStruct
{
public:
	int unk1;
	int unk2;
	int unk3;
	int unk4;
	RtWeakPtr<void>* weakPtrPtr;
};

struct VectorManipulatorStruct
{
public:
	UnkStruct* unkPtr;
	void* unkPtr2;
	bool unkBool;
};

template <typename T>
class TStdVectorManipulator
{
public:
	using GenericVector = std::vector<T>;

	virtual void NullSub() { };

	virtual void DestroyEffect(void* a1) { 
		operator delete(a1);
	};

	virtual bool Edit(GenericVector* vector,
		VectorManipulatorStruct* manipInfo,
		RtWeakPtr<void>* unkWeakPtr,
		RType* reflectionType)
	{
		if (manipInfo->unkBool)
		{
			int vecSize = 0;

			typedef bool(*func)(void*, RtWeakPtr<void>*, int&);
			func pFunc = (func)GetActualOffset(0x262FCC);

			if (pFunc(manipInfo->unkPtr, unkWeakPtr, vecSize))
			{
				if (vecSize > 0)
				{
					LOGI("Begin manip vector (size: %d)", vecSize);
					vector->resize(vecSize);

					for (int i = 0; i < vecSize; i++)
					{
						T* element = &vector->at(i);
						SexyString str;
						reflectionType->Function11(element, manipInfo, str);
					}
				}

				manipInfo->unkPtr->weakPtrPtr--;
				return true;
			}

			return false;
		}
		else
		{
			int** v19 = *(int***)((int) manipInfo + 4);
			typedef void (*sub262EC0)(int**, RtWeakPtr<void>*, int, char);
			((sub262EC0)getActualOffset(0x262EC0))(v19, unkWeakPtr, vector->size(), 0xFD);

			if (!vector->empty()) {
				for (int i = 0; i < vector->size(); ++i) {
					T* element = &vector->at(i);
					SexyString str;
					reflectionType->Function11(element, manipInfo, str);
				}
			}

			int* v17 = *v19;
			v19[2] = (int*)((char*)v19[2] - 1);

			uint8_t v21 = 254;
			typedef int (*sub10D2100)(int*, uint8_t*, int);
			((sub10D2100)getActualOffset(0x10D2100))(v17, &v21, 1);
			
			return true;

			// LOGW("Panic! Need to edit vector but manipInfo->unkBool is false");
		}
	}

	virtual int GetSize(GenericVector* vector)
	{
		return vector->size();
	}
};