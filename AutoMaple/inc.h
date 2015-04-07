//library includes
#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <list>
#include <limits>
#include <set>
#include <utility>
#include <iterator>
#include <map>
#include <unordered_map>
#include <chrono>
#include <limits.h>

//#define WIN

#ifndef _M_CEE
	#include <atomic>
#endif

using namespace std;

//libs
#pragma comment(lib, "Shlwapi.lib")
#include "Shlwapi.h"

#pragma comment(lib, "lua5.3.0.lib")
#include "lua.hpp"

//ignore some warnings
#pragma warning(disable: 4945)
#pragma warning(disable: 4244)
#pragma warning(disable: 4003)

//constants
#define ServerBase 0x01E26B10
#define MobBase 0x01E2B1E4
#define MobCountOff 0x00000010
#define Mob1Off 0x00000028
#define Mob2Off 0x00000004
#define Mob3Off 0x000001CC
#define Mob4Off 0x00000024
#define MobXOff 0x00000058
#define MobYOff (MobXOff + 4)
#define MobDeathOff 0x00000580
#define MyMapInfo 0x01E2B910
#define MyMapIDOff 0x13e4
#define AutoPortal 0x014E4F1A
#define CharBase 0x01E26B14
#define CharAttackCountOff 0x0000BF5C
#define CharBreathOff 0x000008B0
#define CharVecOff 0x517C
#define VecXOff 0xAC
#define VecYOff (VecXOff + 8)
#define MapBase 0x01E2B1EC
#define MapLeftOff 0x0000001C
#define MapTopOff (MapLeftOff + 4)
#define MapRightOff (MapLeftOff + 8)
#define MapBottomOff (MapLeftOff + 12)
#define RopeOff 0xB4
#define StatsBase 0x01E26B18
#define StatsHP 0x00002734
#define StatsMP (StatsHP+4)
#define StatsEXP 0x00002658
#define PortalBase 0x1e2b6a0
#define PortalsOff 4
#define PortalsCountOff -4
#define PortalsFirst 4
#define PortalNextOff 8
#define PortalXOff 0xC
#define PortalYOff (PortalXOff + 4)
#define PortalTMOff 0x1C
#define ItemBase 0x01E2B890
#define ItemCountOff 0x00000014

//8B ? 89 ? ? E8 ? ? ? ? 8B ? 89 ? ? E8 ? ? ? ? 0F ? ? 89 ? ? 8B ? E8 ? ? ? ? 0F

#define ItemFilter 0x006FC37D

//6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 81 ? ? ? ? ? 53 55 56 57 A1 ? ? ? ? 33 ? 50 8D ? 24 ? ? ? ? 64 ? ? ? ? ? 8B ? 8B ? 24 ? ? ? ? 8B ? 24 ? ? ? ? 8B

#define SP (void*)0x01BFDE48
#define SPOrig 0x015FF580

//FF 15 ?? ?? ?? ?? 3B C7 0F 85 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? E8

#define MoveDisable (void*)0x0161BDF9

//89 4C 24 20 89 4C 24 1C 8B 82 A0 5F 00 00 3B C1 74 16 8D 44 24 1C 50 8D

#define MoveJmp 0x0161BD92

#define MS_PRESS 0x00000000
#define MS_DOWN  0x00000030
#define MS_UP    0x80000000

//local includes
#include "Memory.h"
#include "Hacks.h"
#include "MaplePacket.h"
#include "Inventory.h"

extern lua_State* L;
extern HMODULE mod;
extern HANDLE hThread;

//macros
#define jmp(frm, to) (int)(((int)(to) - (int)(frm)) - 5)
#define OpenThread(func) CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&func, NULL, NULL, NULL);
#define OpenThreadArg(func, arg) CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&func, arg, NULL, NULL);
#define CloseThread(Handle) TerminateThread(Handle, 0);
#define STRINGIFY(x) #x
#define Message(ach) MessageBox(NULL, ach, "", MB_OK|MB_ICONEXCLAMATION)

//gui
void Log(const char * c);

//utils
HWND GetMShwnd();
int32_t MsgBox(PSTR sz, ...);
uint32_t VKtoMS(uint32_t key);
HWND WINAPI FindProcessWindow(__in_z LPCSTR lpcszWindowClass, __in DWORD dwProcessId);
void GetFilePathExe(char * buf, const char * file, uint64_t sz);
template<typename T>
T Deref(uint64_t addr, T bad) {
	__try { return *(T *)addr; }
	__except (EXCEPTION_EXECUTE_HANDLER) { return bad; }
}
template<typename T>
T DerefOff(uint64_t addr, int64_t off, T bad) {
	uint64_t addy = Deref(addr, 0);
	if (addy == 0)
		return bad;
	return Deref(addy + off, bad);
}

//etc
void initLua(const char * buf);
void clean();