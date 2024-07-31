#pragma once

#define SEXYDELEGATE_VTBL 0x1BB8A90

namespace Sexy {
	class DelegateBase
	{
	public:
		uint callbackFunc;
		int useOwnerVtable;
		uint callbackOwner;
		int unk0x10;
		uint executeCallbackFunction;

		virtual void Function0() {};
		virtual void Function1() {};
		virtual void Function2() {};
		virtual void Delete() {};
	};
}

Sexy::DelegateBase CreateEmptyDelegate(uint vftableAddr);

Sexy::DelegateBase CreateDelegate(
	uint vftableAddr,
	uint callbackFunc,
	bool useOwnerVtable,
	uint callbackOwner,
	int unk0x10,
	uint executeCallbackFunction
);

Sexy::DelegateBase CreateDelegate(
	uint callbackFunc,
	bool useOwnerVtable,
	uint callbackOwner,
	int unk0x10,
	uint executeCallbackFunction
);
