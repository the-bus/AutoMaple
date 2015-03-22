#include "Hacks.h"

int32_t tX, tY;
int32_t sX, sY;
int32_t mX, mY;

atomic<uint8_t> fetchActive = 0;

uint32_t fRet;
uint32_t fOesi; //frame cave's original esi
uint32_t tOesi; //teleport cave's original esi
uint32_t sOeax; //used by SP to store eax
uint32_t mOeax;

#define kLen 0x100
atomic<uint8_t> pressKeys[kLen] = { 0 };
atomic<uint8_t> holdKeys[kLen] = { 0 };

map<const char *, int32_t> Char;
uint32_t MapID;

int32_t MobCount;
POINT MobClosest;
pair<POINT *, uint64_t> Mobs;
atomic<uint8_t> RefreshMobs = 0;

atomic<uint8_t> RefreshRopes = 0;
pair<RECT *, uint64_t> Ropes;
RECT Map;

int32_t Xoff;
int32_t MoveDelay;
int32_t FaceDelay;

uint8_t Moved = 0;

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
	Memory::Write((void*)AutoPortal, &enable, 1);
}
void Hacks::DisableAutoPortal() {
	byte disable = 0x74;
	Memory::Write((void*)AutoPortal, &disable, 1);
}
__declspec(naked) void __stdcall RawTeleport() {
	__asm {
		mov tOesi,esi
		mov eax, CharBase
		mov esi, [eax] //Character base
		lea ecx, [esi + 04]
		//8B ? ? ? ? 00 85 C0 74 ? 83 ? ? 74 ? 83 ? ? C3 33 C0 C3 CC CC CC CC CC CC CC CC CC CC 8B ? ? ? ? 00 85 C0 74 ? 83 ? ? 74 ? 83 ? ? C3 33 C0 C3 CC CC CC CC CC CC CC CC CC CC 8B ? 24 ? 56 50 8B ? 8B
		mov edx, 0x01584600
		call edx
		test eax, eax
		je TeleportEnd
		push tY //Y Coord
		push tX //X Coord
		push 00
		mov ecx, eax
		//8B ? 24 ? 8B ? ? 8B ? ? ? 8D ? ? 8B ? ? ? ? ? ? FF ? 85 C0 ? ? ? ? ? ? ? ? ? E8 [1st Result]
		mov edx, 0x01600FE0
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
	uint32_t addr = (uint32_t)SPCave;
	Memory::Write(SP, &addr, 4);
}
void Hacks::UnHookSP() {
	uint32_t orig = SPOrig;
	Memory::Write(SP, &orig, 4);
}
void Hacks::SetSP(int32_t x, int32_t y) {
	sX = x;
	sY = y;
}
void __stdcall FetchChar() {
	auto Vec = DerefOff<uint32_t>(CharBase, CharVecOff, 0);
	Char["x"] =
		round(
			Deref<double>(
				(Vec + VecXOff),
				INT_MAX
			)
		);
	Char["y"] =
		round(
			Deref<double>(
				(Vec + VecYOff),
				INT_MAX
			)
		);
	Char["breath"] = DerefOff<uint32_t>(CharBase, CharBreathOff, -1);
	Char["attackCount"] = DerefOff<uint32_t>(CharBase, CharAttackCountOff, -1);
}
void __stdcall FetchMapInfo() {
	MapID = DerefOff<uint32_t>(MyMapInfo, MyMapIDOff, 0);
}
void __stdcall FetchMob() {
	POINT closest = { INT_MAX };
	long area = 9999999;
	// mob base
	uint32_t mob = Deref(MobBase, 0);
	if (!mob)
		goto end;
	// mob count
	MobCount = Deref(mob + MobCountOff, 0);
	if (!MobCount)
		goto end;
	// mob off1
	mob = Deref(mob + Mob1Off, 0);
	if (!mob)
		goto end;
	mob -= 0x10; // first mob
	if (RefreshMobs != 0) {
		delete Mobs.first;
		Mobs.first = new POINT[MobCount];
	}
	uint32_t i = 0;
	while (mob) {
		uint32_t data = Deref(mob + Mob2Off + 0x10, 0); // MobData1
		if (!data)
			goto end;
		data = Deref(data + Mob3Off, 0); // MobData2
		if (!data)
			goto end;
		data = Deref(data + Mob4Off, 0); // MobData3
		if (!data)
			goto end;
		POINT pos = *(POINT *)(data + MobXOff);
		POINT inv = *(POINT *)(data + MobXOff + sizeof(POINT));
		if (inv.x || inv.y) {
			POINT diff;
			diff.x = pos.x - Char["x"];
			diff.y = pos.y - Char["y"];
			long curarea = diff.x * diff.x + diff.y * diff.y;
			if (curarea < area) {
				area = curarea;
				closest = pos;
			}
			if (RefreshMobs != 0)
				Mobs.first[i] = pos;
			i++;
			Mobs.second = i;
		}
		mob = Deref(mob + Mob2Off, 0); // next
	}
	end:
	MobCount = i;
	RefreshMobs = 0;
	MobClosest = closest;
}
boolean InBoundsX(int32_t x) {
	return (x >= Map.left && x <= Map.right);
}
boolean InBoundsY(int32_t y) {
	return (y >= Map.top && y <= Map.bottom);
}
void __stdcall FetchMap() {
	auto MapLeft = DerefOff<int32_t>(MapBase, MapLeftOff, 0);
	auto MapRight = DerefOff<int32_t>(MapBase, MapRightOff, 0);
	auto MapTop = DerefOff<int32_t>(MapBase, MapTopOff, 0);
	auto MapBottom = DerefOff<int32_t>(MapBase, MapBottomOff, 0);
	Map = RECT{ MapLeft, MapTop, MapRight, MapBottom };
	if (RefreshRopes != 0) {
		auto Rope = DerefOff<uint32_t>(MapBase, RopeOff, 0);
		auto RopeCount = Deref<uint32_t>(Rope - 4, 0) - 1;
		delete Ropes.first;
		Ropes.first = new RECT[RopeCount];
		//vector<RECT> rects;
		Rope += 0x2C;
		uint32_t i = 0;
		for (; i < RopeCount; i++) {
			int32_t x = Deref(Rope, INT_MAX);
			//if (!InBoundsX(x))
			//	break;
			int32_t y1 = Deref(Rope + 4, INT_MAX);
			//if (!InBoundsY(y1))
			//	break;
			int32_t y2 = Deref(Rope + 8, INT_MAX);
			//if (!InBoundsY(y2))
			//	break;
			//rects.push_back(RECT{ x, y2, x, y1 });
			Ropes.first[i] = RECT{ x, y2, x, y1 };
			Rope += 0x20;
			Ropes.second = i + 1;
		}
		//RECT * arr = new RECT[rects.size()];
		//copy(rects.begin(), rects.end(), arr);
		//Ropes.first = arr;
		RefreshRopes = 0;
	}
}
void __stdcall FetchAll() {
	fetchActive = 1;
	FetchChar();
	FetchMapInfo();
	FetchMob();
	FetchMap();
	fetchActive = 0;
}
void WaitForFetch() {
	while (fetchActive != 0)
		Sleep(0);
}
pair<POINT *, uint64_t> Hacks::GetMobs() {
	WaitForFetch(); //wait if a fetch is currently active
	RefreshMobs = 1; //tell the next fetch to refresh the mobs array
	while (RefreshMobs != 0)
		Sleep(0);
	return Mobs;
}
pair<RECT *, uint64_t> Hacks::GetRopes() {
	WaitForFetch(); //wait if a fetch is currently active
	RefreshRopes = 1; //tell the next fetch to refresh the mobs array
	while (RefreshRopes != 0)
		Sleep(0);
	return Ropes;
}
map<const char *, int32_t> Hacks::GetChar() {
	WaitForFetch();
	return Char;
}
RECT Hacks::GetMap() {
	WaitForFetch();
	return Map;
}
int32_t Hacks::GetMapID() {
	WaitForFetch();
	return MapID;
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
	do {
		WaitForFetch();
		Sleep(Char["breath"]);
	} while (Char["breath"] != 0);
	return;
}
__declspec(naked) void __stdcall MoveCave() {
	Moved = 1;
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
	byte orig2[] = {0x89,0x7C,0x24,0x20,0x89,0x7C,0x24,0x1C};
	Memory::Write((void*)MoveJmp, &orig2, 8);
	byte orig[] = { 0xFF, 0x15, 0xC0, 0xB8, 0xE3, 0x01 };
	Memory::Write(MoveDisable, &orig, 6);
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
	Moved = 0;
	while (Moved == 0)
		Sleep(0);
}
void Hacks::SetMoveDelay(int32_t delay) {
	MoveDelay = delay;
}
void Hacks::SetMoveXOff(int32_t off) {
	Xoff = off;
}
void Hacks::MoveXOffNoStop(int32_t targetX, int32_t off) {
	bool right = targetX > GetChar()["x"];
	if (right) {
		SetMove(1, 0);
		while (targetX - off > GetChar()["x"]);
	}
	else {
		SetMove(-1, 0);
		while (targetX + off < GetChar()["x"]);
	}
}
void Hacks::MoveXOff(int32_t targetX, int32_t off) {
	MoveXOffNoStop(targetX, off);
	SetMove(0, 0);
	Sleep(MoveDelay);
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
		oY = GetChar()["y"];
		Sleep(RopePollDelay);
	} while (GetChar()["y"] != oY);
	SetMove(0, 0);
}
void Hacks::RopeY(int32_t targetY) {
	bool down = targetY > GetChar()["y"];
	if (down) {
		SetMove(0, 1);
		while (targetY > GetChar()["y"]);
	}
	else {
		SetMove(0, -1);
		while (targetY < GetChar()["y"]);
	}
	SetMove(0, 0);
}
void Hacks::FaceLeft() {
	SetMove(-1, 0);
	SetMove(0, 0);
}
void Hacks::FaceRight() {
	SetMove(1, 0);
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
		mov edx, 0x0163AB10
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
	byte* dispatchAddy = (byte*)((uint32_t)GetProcAddress(hMod, "DispatchMessageA") + 0x0);
	fRet = (uint32_t)(dispatchAddy) + 2;
	byte opcode = 0xE9;
	Memory::Write(dispatchAddy - 5, &opcode, 1);
	uint32_t distance = jmp(dispatchAddy - 5, FrameCave);
	Memory::Write(dispatchAddy - 4, &distance, 4);
	byte backShortJump[] = { 0xEB, 0xF9 };
	Memory::Write(dispatchAddy, &backShortJump, 2);
}
void Hacks::UnHookFrame() {
	HINSTANCE hMod = GetModuleHandle("user32.dll");
	byte* dispatchAddy = (byte*)((uint32_t)GetProcAddress(hMod, "DispatchMessageA") + 0x0);
	byte movEdiEdi[] = { 0x8B, 0xFF };
	Memory::Write(dispatchAddy, &movEdiEdi, 2);
}
bool Hacks::SendPacket(const char * packet) {
	string temp(packet);
	static MaplePacket p;
	if (!p.Parse(temp) || !p.Send())
		return false;
	return true;
}