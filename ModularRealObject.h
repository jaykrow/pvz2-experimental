#pragma once
#include "RealObject.h"

class ModularRealObject : public RealObject
{
public:
	void* m_attachedEffects;
	int m_unk0x2C;
	int m_unk0x30;
	int m_unk0x34;

	virtual void Function33() {};
	virtual void Function34() {};
};