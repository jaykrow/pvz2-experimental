#pragma once
#include <unistd.h>
#include <cstdlib>
#include <stdio.h>
#include "Logging.h"

#define LOG_RETURN_ADDRESS(level) \
LOGI("%p", (uintptr_t)__builtin_return_address(level) - g_libAddress);

extern uint g_libAddress;
// Get base address of a library (.so) loaded in memory.
uint getLibraryAddress(const char* libName);
// Get actual offset of address inside libPVZ2.so
uint getActualOffset(uint offset);
// Get actual offset of address inside libPVZ2.so
uint GetActualOffset(uint offset);
// Get offset within libPVZ2.so from a running program
uint getOriginalOffset(uint actualOffset);
// Hook a function in libPVZ2
void PVZ2HookFunction(uint offset, void* replace, void** result, const char* funcName);

void* copyVFTable(int vftableAddr, int numVFuncs);

void patchVFTable(void* vftable, void* funcAddr, int index);

//void patchVFTableOnObj(void* obj, void* funcAddr, int index);

void copyVFTable(void* dest, int vftableAddr, int numVFuncsToCopy);

void* createChildVFTable(int vFuncsCount, int parentVftable, int nuMVFuncsToCopy);

void setVFTable(void* obj, uintptr_t newVftablePtr);

void* GetVirtualFunc(void* obj, int index);

//template <typename R, typename... Args>
//R CallVirtualFunc(void* obj, int index, Args... args);

template <typename R, typename... Args>
R CallVirtualFunc(void* obj, int index, Args... args) {
    void* func = GetVirtualFunc(obj, index);
    auto castedFunc = reinterpret_cast<R(*)(Args...)>(func);
    return castedFunc(args...);
}
