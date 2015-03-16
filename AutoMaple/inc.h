//library includes
#include <windows.h>
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

//libs
#pragma comment(lib, "Shlwapi.lib")
#include "Shlwapi.h"

#pragma comment(lib, "lua5.3.0.lib")
#include "lua.hpp"


//ignore some warnings
#pragma warning(disable: 4945)
#pragma warning(disable: 4244)

//constants
#define MapInfo (unsigned long)0x01E2B910
#define MapIDOff (unsigned long)0x13e4
#define AutoPortal (void*)0x014E4EDA
#define CharBase 0x01E26B14
#define CharBreath 0x000008B0
#define CharVec (unsigned long)0x517C
#define VecX (unsigned long)0xAC
#define VecY (unsigned long)(VecX + 8)
#define CharXOff (unsigned long)0xCCC4
#define CharYOff (unsigned long)(CharXOff + 4)

//6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 81 ? ? ? ? ? 53 55 56 57 A1 ? ? ? ? 33 ? 50 8D ? 24 ? ? ? ? 64 ? ? ? ? ? 8B ? 8B ? 24 ? ? ? ? 8B ? 24 ? ? ? ? 8B

#define SP (void*)0x01BFDE50
#define SPOrig 0x015FF360

//FF 15 ?? ?? ?? ?? 3B C7 0F 85 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? E8

#define MoveDisable (void*)0x0161BBD9

//89 4C 24 20 89 4C 24 1C 8B 82 A0 5F 00 00 3B C1 74 16 8D 44 24 1C 50 8D

#define MoveJmp 0x0161BB72

#define MS_PRESS 0x00000000
#define MS_DOWN  0x00000030
#define MS_UP    0x80000000

//local includes
#include "Memory.h"
#include "Hacks.h"

using namespace std;

extern lua_State* L;
extern HMODULE mod;
extern HANDLE hThread;

//macros

#define jmp(frm, to) (int)(((int)to - (int)frm) - 5)
#define OpenThread(void) CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&void, NULL, NULL, NULL);
#define CloseThread(Handle) TerminateThread(Handle, 0);
#define STRINGIFY(x) #x
#define Message(ach) MessageBox(NULL, ach, "", MB_OK|MB_ICONEXCLAMATION)
#define deref(addy, type, bad) IsBadReadPtr((void*)addy, sizeof(type)) ? bad : *(type *) addy

//functions

//utils
int32_t MsgBox(PSTR sz, ...);
uint32_t VKtoMS(uint32_t key);
HWND WINAPI FindProcessWindow(__in_z LPCSTR lpcszWindowClass, __in DWORD dwProcessId);

//etc
void initLua();
void Close();