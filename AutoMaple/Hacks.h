#pragma once
#include "inc.h"

class Hacks {

public:
	static void HookFrame();
	static void KeyDown(uint32_t);
	static void KeyUp(uint32_t);
	static void KeyPress(uint32_t);
	static void EnableAutoPortal();
	static void DisableAutoPortal();
	static void Teleport(int32_t, int32_t);
	static void HookSP();
	static void UnHookSP();
	static void SetSP(int32_t, int32_t);
	static uint32_t GetMapID();
	static void HookMove();
	static void UnHookMove();
	static void SetMove(int32_t x, int32_t y);
	static int32_t GetX();
	static int32_t GetY();
	static void WaitForBreath();

};