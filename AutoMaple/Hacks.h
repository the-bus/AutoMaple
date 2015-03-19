#pragma once
#include "inc.h"

class Hacks {

public:
	static void HookFrame();
	static void KeyDown(int32_t);
	static void KeyUp(int32_t);
	static void KeyPress(int32_t);
	static void KeySpam(int32_t);
	static void KeyUnSpam(int32_t);
	static void EnableAutoPortal();
	static void DisableAutoPortal();
	static void Teleport(int32_t, int32_t);
	static void HookSP();
	static void UnHookSP();
	static void SetSP(int32_t, int32_t);
	static int32_t GetMapID();
	static void HookMove();
	static void UnHookMove();
	static void SetMove(int32_t, int32_t);
	static void WaitForBreath();
	static void ResetKeys();
	static void SetMoveDelay(int32_t);
	static void MoveX(int32_t);
	static void MoveXOff(int32_t, int32_t);
	static void SetMoveXOff(int32_t);
	static void SetRopePollDelay(int32_t);
	static void Rope(int32_t);
	static void RopeY(int32_t);
	static void SetFaceDelay(int32_t);
	static void FaceLeft();
	static void FaceRight();
	static void KeyHoldFor(int32_t, int32_t);
	static int32_t GetMobCount();
	static POINT GetMobClosest();
	static POINT GetChar();
	static pair<POINT *, uint64_t> GetMobs();
	static pair<RECT *, uint64_t> Hacks::GetRopes();
	static RECT Hacks::GetMap();

};