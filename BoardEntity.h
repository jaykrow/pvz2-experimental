#pragma once
#include "ModularRealObject.h"

struct DamageInfo;

/*
THIS BOARD ENTITY IS MISSING 4 BYTES
*/
class BoardEntity : public ModularRealObject
{
public:
	int m_level;
	int m_unk0x3C;
	Rect m_collisionRect;
	int m_boardEntityFlags;
	int m_unk0x54;
	RtWeakPtr<void> m_componentRunner;
	float m_createdTime;

	void Die() {
		typedef void (*DieFunc)(BoardEntity*);
		((DieFunc) GetActualOffset(0x54BA18))(this);
	}

	void GetGridCoords(int* res) {
		res[0] = (this->m_position.mX - 200.0) / 64.0;
		res[1] = (this->m_position.mY - 160.0) / 76.0;
	}

	virtual void TakeDamage(DamageInfo* dmgInfo) {};
	virtual void Function36() {};
	virtual void Function37() {};
	virtual void Function38() {};
	virtual void Function39() {};
	virtual void Function40() {};
	virtual void Function41() {};
	virtual void Function42() {};
	virtual void Function43() {};
	virtual void Function44() {};
	virtual void Function45() {};
	virtual void Function46() {};
	virtual void Function47() {};
	virtual SexyString* GetTypeName() {};
	virtual void OnSpawn() {};
};