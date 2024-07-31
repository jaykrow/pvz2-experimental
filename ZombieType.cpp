#include "ZombieType.h"

void ZombieType::GetFromName(RtWeakPtr<ZombieType>* res, SexyString zombieName) {
	typedef int (*zombieTypeDirectoryGet)();
	int zombieTypeDirectoryInstance = ((zombieTypeDirectoryGet)getActualOffset(ZOMBIETYPEDIRECTORY_GET))();

	typedef void (*getAliasedType)(RtWeakPtr<ZombieType>*, int, SexyString*);
	((getAliasedType)getActualOffset(ZOMBIETYPEDIRECTORY_GETTYPE))(res, zombieTypeDirectoryInstance, &zombieName);
}