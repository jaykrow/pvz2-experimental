#include "Zombie.h"

void EnterState(Zombie* zombie, int state, int unk) {
	typedef void (*zombieEnterState)(Zombie*, int, int);
	((zombieEnterState)getActualOffset(0x8AEC44))(zombie, state, unk);
}

bool IsInState(Zombie* zombie, int state) {
	typedef bool (*isInState)(Zombie*, int);
	return ((isInState)getActualOffset(0x8AF9D0))(zombie, state);
}