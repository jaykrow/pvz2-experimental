#include "pch.h"
#include "ZombotanyBaseProps.h"
#include "PvZ2/RtWeakPtr.h"

#define RTOBJECT_GETTYPE_INDEX 0
#define RTOBJECT_FUNC1_INDEX 1
#define RTOBJECT_CONSTRUCT_INDEX 2
#define RTOBJECT_DESTRUCT_INDEX 3
#define RTOBJECT_ISTYPE_INDEX 4
#define RTOBJECT_FUNC5_INDEX 5
#define RTOBJECT_FUNC6_INDEX 6

void* ZombotanyBaseProps::vftable = NULL;
void* ZombotanyBaseProps::s_rtClass = NULL;

void ZombotanyBaseProps::modInit() {
	LOGI("ZombotanyBaseProps init");

	vftable = copyVFTable(getActualOffset(0x1C144C8), 14);

	patchVFTable(vftable, (void*)ZombotanyBaseProps::getRTClass, RTOBJECT_GETTYPE_INDEX);

	ZombotanyBaseProps::getRTClass();

	LOGI("ZombotanyBaseProps finish init");
}

ZombotanyBaseProps* getZombotanyBaseProps(int bullZombie) {
	int v5 = 0;

	RtWeakPtr<int> v11 = RtWeakPtr<int>();

	typedef void (*fromOther)(RtWeakPtr<int>*, RtWeakPtr<int>*);
	((fromOther)(getActualOffset(0x10B59B8)))(&v11, (RtWeakPtr<int>*)(bullZombie + 0x300));

	typedef int (*RtWeakPtrRegistry)();
	int v3 = ((RtWeakPtrRegistry)(getActualOffset(0x10C6764)))();
	typedef void (*sub_10CC81C)(RtWeakPtr<int>*, int, RtWeakPtr<int>*);
	RtWeakPtr<int> a1;
	typedef void (*sub_10CC81C)(RtWeakPtr<int>*, int, RtWeakPtr<int>*);
	((sub_10CC81C)(getActualOffset(0x10CC81C)))(&a1, v3, &v11);

	if (!a1.is_invalid()) {
		int typeId = a1.type_id();
		typedef int (*sub_10CC404)(int, int);
		int* v7 = (int*)((sub_10CC404)(getActualOffset(0x10CC404)))(v3, typeId);
		if (v7) {
			typedef int (*resolveWeakPtr)(int*, RtWeakPtr<int>*);
			v5 = ((resolveWeakPtr)(getActualOffset(0x10C8780)))(v7, &a1);
		}
	}

	typedef void (*dtor)(RtWeakPtr<int>*);
	((dtor)(getActualOffset(0x10B58F8)))(&a1);

	if (v5) {
		typedef int (*temp)(int, int);
		temp isTypeCheck = *(temp*)(*(int*)v5 + 0x10); // v12 are all isType functions
		int zombotanyBaseType = (int) ZombotanyBaseProps::getRTClass();

		// LOGI("access fun at %u with args %d %d", getOriginalOffset((uint) v12), v9, v13);

		bool correctType = isTypeCheck(v5, zombotanyBaseType); // probably checking if v9 is the proper
		if (!correctType) {
			v5 = 0;
		}
	}
	((dtor)(getActualOffset(0x10B58F8)))(&v11);
	return (ZombotanyBaseProps*)v5;
}
