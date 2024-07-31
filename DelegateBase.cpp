#include "pch.h"
#include "DelegateBase.h"
#include "memUtils.h"

Sexy::DelegateBase CreateEmptyDelegate(uint vftableAddr) {
	Sexy::DelegateBase delegate = Sexy::DelegateBase();
	*(uint*)&delegate = getActualOffset(vftableAddr);
	return delegate;
}

Sexy::DelegateBase CreateDelegate(
	uint vftableAddr,
	uint callbackFunc,
	bool useOwnerVtable,
	uint callbackOwner,
	int unk0x10,
	uint executeCallbackFunction
) {
	Sexy::DelegateBase res = CreateEmptyDelegate(vftableAddr);
	res.callbackFunc = callbackFunc;
	res.useOwnerVtable = useOwnerVtable;
	res.callbackOwner = callbackOwner;
	res.unk0x10 = unk0x10;
	res.executeCallbackFunction = executeCallbackFunction;
	return res;
}

Sexy::DelegateBase CreateDelegate(
	uint callbackFunc,
	bool useOwnerVtable,
	uint callbackOwner,
	int unk0x10,
	uint executeCallbackFunction
) {
	return CreateDelegate(
		SEXYDELEGATE_VTBL,
		callbackFunc,
		useOwnerVtable,
		callbackOwner,
		unk0x10,
		executeCallbackFunction
	);
}