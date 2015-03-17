#include "Hacks.h"

int32_t tX, tY;
int32_t sX, sY;
int32_t mX, mY;

uint8_t doneFetch = 0;

uint32_t fRet;
uint32_t fOesi; //frame cave's original esi

uint32_t tOesi; //teleport cave's original esi

uint32_t sOeax; //used by SP to store eax

#define kLen 0x100
uint8_t pressKeys[kLen] = { 0 };
uint8_t holdKeys[kLen] = { 0 };
void Hacks::KeySpam(int32_t k) {
	pressKeys[k] = 2;
}
void Hacks::KeyUnSpam(int32_t k) {
	pressKeys[k] = 0;
}
void Hacks::KeyPress(int32_t k)
{
	pressKeys[k] = 1;
	while (pressKeys[k] != 0)
		Sleep(0);
	return;
}
void Hacks::KeyDown(int32_t k)
{
	holdKeys[k] = 1;
}
void Hacks::KeyUp(int32_t k)
{
	holdKeys[k] = 2;
	while (pressKeys[k] != 0)
		Sleep(0);
	return;
}
void Hacks::EnableAutoPortal() {
	byte enable = 0x75;
	Memory::Write(AutoPortal, &enable, 1);
}
void Hacks::DisableAutoPortal() {
	byte disable = 0x74;
	Memory::Write(AutoPortal, &disable, 1);
}
__declspec(naked) void __stdcall RawTeleport() {
	__asm {
		mov tOesi,esi
		mov eax, CharBase
		mov esi, [eax] //Character base
		lea ecx, [esi + 04]
		//8B ? ? ? ? 00 85 C0 74 ? 83 ? ? 74 ? 83 ? ? C3 33 C0 C3 CC CC CC CC CC CC CC CC CC CC 8B ? ? ? ? 00 85 C0 74 ? 83 ? ? 74 ? 83 ? ? C3 33 C0 C3 CC CC CC CC CC CC CC CC CC CC 8B ? 24 ? 56 50 8B ? 8B
		mov edx, 0x015843E0
		call edx
		test eax, eax
		je TeleportEnd
		push tY //Y Coord
		push tX //X Coord
		push 00
		mov ecx, eax
		//8B ? 24 ? 8B ? ? 8B ? ? ? 8D ? ? 8B ? ? ? ? ? ? FF ? 85 C0 ? ? ? ? ? ? ? ? ? E8 [1st Result]
		mov edx, 0x01600DC0
		call edx
		
		TeleportEnd:
		mov esi,tOesi
		ret
	}
}
void Hacks::Teleport(int32_t x, int32_t y) {
	tX = x;
	tY = y;
	RawTeleport();
}
__declspec(naked) void __stdcall SPCave() {
	__asm {
		mov sOeax, eax
		mov eax, sX
		mov [esp+0x8], eax
		mov eax, sY
		mov [esp+0xC], eax

		mov eax, sOeax
		//6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 81 ? ? ? ? ? 53 55 56 57 A1 ? ? ? ? 33 ? 50 8D ? 24 ? ? ? ? 64 ? ? ? ? ? 8B ? 8B ? 24 ? ? ? ? 8B ? 24 ? ? ? ? 8B
		push SPOrig
		ret
	}
}
void Hacks::HookSP() {
	uint32_t addr = (unsigned int)SPCave;
	Memory::Write(SP, &addr, 4);
}
void Hacks::UnHookSP() {
	uint32_t orig = SPOrig;
	Memory::Write(SP, &orig, 4);
}
void Hacks::SetSP(int x, int y) {
	sX = x;
	sY = y;
}
POINT Char;
uint32_t MID = 0;
uint32_t Breath;
void __stdcall FetchChar() {
	Char.x =
		round(
			deref(
				(Memory::Pointer::read_int(CharBase, CharVec) + VecX),
				double,
				2147483647
			)
		);
	Char.y =
		round(
			deref(
				(Memory::Pointer::read_int(CharBase, CharVec) + VecY),
				double,
				2147483647
			)
		);
	Breath = Memory::Pointer::read_int(CharBase, CharBreath);
}
void __stdcall FetchMap() {
	MID = Memory::Pointer::read_int(MapInfo, MapIDOff);
}
int32_t MobCount = 0;
POINT MobClosest;
POINT * Mobs;
uint8_t RefreshMobs = 0;
void __stdcall FetchMob() {
	POINT closest = { 2147483647 };
	long area = 9999999;
	// mob base
	uint32_t mob = deref(MobBase, uint32_t, 0);
	if (!mob) {
		MobClosest = closest;
		return;
	}
	// mob count
	MobCount = deref(mob + MobCountOff, int32_t, 0);
	if (!MobCount) {
		MobClosest = closest;
		return;
	}
	// mob off1
	mob = deref(mob + Mob1Off, uint32_t, 0);
	if (!mob) {
		MobClosest = closest;
		return;
	}
	mob -= 0x10; // first mob
	if (RefreshMobs != 0)
		Mobs = new POINT [MobCount];
	int i = 0;
	while (mob) {
		uint32_t data = deref(mob + Mob2Off + 0x10, uint32_t, 0); // MobData1
		if (!data)
			return;
		data = deref(data + Mob3Off, uint32_t, 0); // MobData2
		if (!data)
			return;
		data = deref(data + Mob4Off, uint32_t, 0); // MobData3
		if (!data)
			return;
		POINT pos = *(POINT *)(data + MobXOff);
		POINT inv = *(POINT *)(data + MobXOff + sizeof(POINT));
		if (inv.x || inv.y) {
			POINT diff;
			diff.x = pos.x - Char.x;
			diff.y = pos.y - Char.y;
			long curarea = diff.x * diff.x + diff.y * diff.y;
			if (curarea < area) {
				area = curarea;
				closest = pos;
			}
			if (RefreshMobs != 0)
				Mobs[i] = POINT{ pos.x, pos.y };
			i++;
		}
		mob = deref(mob + Mob2Off, uint32_t, 0); // next
	}
	MobCount = i;
	RefreshMobs = 0;
	MobClosest = closest;
}
void __stdcall FetchAll() {
	doneFetch = 0;
	FetchChar();
	FetchMap();
	FetchMob();
	doneFetch = 1;
}
void WaitForFetch() {
	while (doneFetch == 0)
		Sleep(0);
}
POINT * Hacks::GetMobs() {
	WaitForFetch();
	RefreshMobs = 1;
	while (RefreshMobs != 0)
		Sleep(0);
	return Mobs;
}
POINT Hacks::GetChar() {
	WaitForFetch();
	return Char;
}
int32_t Hacks::GetMapID() {
	WaitForFetch();
	return MID;
}
int32_t Hacks::GetMobCount() {
	WaitForFetch();
	return MobCount;
}
POINT Hacks::GetMobClosest() {
	WaitForFetch();
	return MobClosest;
}
void Hacks::WaitForBreath() {
	while (true) {
		WaitForFetch();
		if (Breath != 0)
			Sleep(Breath);
		else
			break;
	}
	return;
}
uint32_t mOeax;
__declspec(naked) void __stdcall MoveCave() {
	__asm {
		mov mOeax, eax
		mov eax, mX
		mov [esp+0x20],eax //Left = #-1 or FFFFFFFF, Right = 1, Nothing = 0
		mov eax, mY
		mov [esp+0x1C],eax //Up = #-1 or FFFFFFFF, Down = 1, Nothing = 0
		mov eax, mOeax
		push MoveJmp+8
		ret
	}
}
void Hacks::HookMove() {
	byte nops[] = {0x90,0x90,0x90,0x90,0x90,0x90};
	Memory::Write(MoveDisable, &nops, 6);
	byte opcode = 0xE9;
	Memory::Write((void*)MoveJmp, &opcode, 1);
	uint32_t distance = jmp(MoveJmp, MoveCave);
	Memory::Write((void*)(MoveJmp + 1), &distance, 4);
	Memory::Write((void*)(MoveJmp + 5), &nops, 3);
}
void Hacks::UnHookMove() {
	byte orig[] = {0xFF,0x15,0xC0,0xB8,0xE3,0x01};
	Memory::Write(MoveDisable, &orig, 6);
	byte orig2[] = {0x89,0x7C,0x24,0x20,0x89,0x7C,0x24,0x1C};
	Memory::Write((void*)MoveJmp, &orig2, 8);
}
void Hacks::ResetKeys() {
	for (uint32_t i = 0; i < kLen; i++) {
		if (holdKeys[i] == 1)
			KeyUp(i);
		pressKeys[i] = 0;
	}
}
void Hacks::SetMove(int32_t x, int32_t y) {
	if (!(x < -1 || x > 1))
		mX = x;
	if (!(y < -1 || y > 1))
		mY = y;
}
int32_t Xoff;
int32_t MoveDelay;
void Hacks::SetMoveDelay(int32_t delay) {
	MoveDelay = delay;
}
void Hacks::SetMoveXOff(int32_t off) {
	Xoff = off;
}
void Hacks::MoveXOff(int32_t targetX, int32_t off) {
	bool right = targetX > GetChar().x;
	if (right) {
		SetMove(1, 0);
		while (targetX - off > GetChar().x)
			Sleep(0);
	}
	else {
		SetMove(-1, 0);
		while (targetX + off < GetChar().x)
			Sleep(0);
	}
	//int32_t dist = GetChar().x - targetX;
	//dist = dist < 0 ? -dist : dist;
	//if (dist > 10)
	//MoveXOff(targetX, off);
	//else {
		SetMove(0, 0);
		Sleep(MoveDelay);
	//}
}
void Hacks::MoveX(int32_t targetX) {
	MoveXOff(targetX, Xoff);
}
int32_t RopePollDelay;
void Hacks::SetRopePollDelay(int32_t delay) {
	RopePollDelay = delay;
}
void Hacks::Rope(int32_t dir) {
	SetMove(0, dir);
	int32_t oY; //original y
	do {
		oY = GetChar().y;
		Sleep(RopePollDelay);
	} while (GetChar().y != oY);
	SetMove(0, 0);
}
void Hacks::RopeY(int32_t targetY) {
	bool down = targetY > GetChar().y;
	if (down) {
		SetMove(0, 1);
		while (targetY > GetChar().y)
			Sleep(0);
	}
	else {
		SetMove(0, -1);
		while (targetY < GetChar().y)
			Sleep(0);
	}
	SetMove(0, 0);
}
int32_t FaceDelay;
void Hacks::SetFaceDelay(int32_t delay) {
	FaceDelay = delay;
}
void Hacks::FaceLeft() {
	SetMove(-1, 0);
	Sleep(FaceDelay);
	SetMove(0, 0);
}
void Hacks::FaceRight() {
	SetMove(1, 0);
	Sleep(FaceDelay);
	SetMove(0, 0);
}
void Hacks::KeyHoldFor(int32_t k, int32_t delay) {
	KeyDown(k);
	Sleep(delay);
	KeyUp(k);
}
void __stdcall SendKey(uint32_t VK, uint32_t mask) {
	uint32_t Key = VKtoMS(VK) | mask;
	__asm {
		mov fOesi, esi
		mov esi, [ServerBase] // Server Base (TSingleton<CWvsContext>)
		mov ecx, [esi + 0xA4] // Window manager offset
		push Key // 2nd parameter: key code for CTRL
		push VK // 1st parameter: Apparently unused parameter that seems to be related to the type of key that is being pressed. You can just set it to zero
		// ? ? ? ? ? 85 ? 74 ? 8D ? ? 8B ? 8B ? FF ? C2 08 00
		mov edx, 0x0163A8F0
		call edx // key press function
		mov esi, fOesi
	}
}
void __stdcall SendKeys() {
	for (uint32_t i = 0; i < kLen; i++) {
		if (holdKeys[i] == 1) {
			SendKey(i, MS_DOWN);
		}
		else if (holdKeys[i] == 2) {
			SendKey(i, MS_PRESS);
			SendKey(i, MS_UP);
			holdKeys[i] = 0;
		}
		else if (pressKeys[i] != 0) {
			SendKey(i, MS_DOWN);
			SendKey(i, MS_PRESS);
			SendKey(i, MS_UP);
			if (pressKeys[i] == 1)
				pressKeys[i] = 0;
		}
	}
}
__declspec(naked) void __stdcall FrameCave() {
	__asm {
		mov edi,edi //original
		push ebp    //original
		mov ebp,esp //original
		pushad
		call SendKeys
		popad
		pushad
		call FetchAll
		popad
		push fRet
		ret
	}
}
void Hacks::HookFrame() {
	HINSTANCE hMod = GetModuleHandle("user32.dll");
	byte* dispatchAddy = (byte*)((uint32_t)GetProcAddress(hMod, "PeekMessageA") + 0x0);
	fRet = (unsigned long)dispatchAddy+5;
	byte opcode = 0xE9;
	Memory::Write(dispatchAddy, &opcode, 1);
	uint32_t distance = jmp(dispatchAddy, FrameCave);
	Memory::Write(dispatchAddy+1, &distance, 4);
}