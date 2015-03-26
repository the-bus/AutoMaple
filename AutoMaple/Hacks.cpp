#include "Hacks.h"
#include <concurrent_queue.h>

int32_t sX, sY;
int32_t mX, mY;
int32_t tX, tY;

atomic<uint8_t> frameActive = 0;

uint32_t fRet;
uint32_t fOesi; //frame cave's original esi
uint32_t sOeax; //used by SP to store eax
uint32_t mOeax;

#define kLen 0x100
atomic<uint8_t> pressKeys[kLen] = { 0 };
atomic<uint8_t> holdKeys[kLen] = { 0 };

map<const char *, double> Char;
uint32_t MapID;

int32_t MobCount;
POINT MobClosest;
pair<POINT *, uint64_t> Mobs;
atomic<uint8_t> RefreshMobs;

pair<RECT *, uint64_t> Ropes;
RECT Map;
atomic<uint8_t> RefreshRopes;

int32_t Xoff;
int32_t MoveDelay;

uint8_t Moved;

time_t timeout;

int32_t HPKey;
int32_t HPMin;
int32_t MPKey;
int32_t MPMin;

Concurrency::concurrent_queue<void(*)()> functions;

#define timeoutWhile(cond) \
do { \
		time_t stime = time(NULL); \
		while (cond && (timeout <= 0 || time(NULL) - stime < timeout)); \
		ret = !(cond); \
} while (0);
#define DoFuncFrameWrap(func, code, ...) \
void Hacks::func(__VA_ARGS__) { \
	code \
	DoFuncFrame(Do ## func); \
}
void WaitForFrame() {
	while (frameActive != 0)
		Sleep(0);
}
void WaitForReframe() {
	while (frameActive != 1)
		Sleep(0);
}
void DoFuncFrame(void(*f)()) {
	WaitForFrame();
	functions.push(f);
	WaitForReframe();
}
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
void Hacks::AutoHP(int32_t k, int32_t minhp) {
	HPKey = k;
	HPMin = minhp;
}
void Hacks::AutoMP(int32_t k, int32_t minmp) {
	MPKey = k;
	MPMin = minmp;
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
void DoEnableAutoPortal() {
	byte enable = 0x75;
	Memory::Write((void*)AutoPortal, &enable, 1);
}
DoFuncFrameWrap(EnableAutoPortal, ;)
void DoDisableAutoPortal() {
	byte disable = 0x74;
	Memory::Write((void*)AutoPortal, &disable, 1);
}
DoFuncFrameWrap(DisableAutoPortal, ;)
/*translates to:
void Hacks::DisableAutoPortal() {
	DoFuncFrame(DoDisableAutoPortal);
}*/
typedef void(__fastcall *pfnCVecCtrlUser__OnTeleport)(void *pthis, void *edx, void *, long x, long y);
//8B ? 24 ? 8B ? ? 8B ? ? ? 8D ? ? 8B ? ? ? ? ? ? FF ? 85 C0 ? ? ? ? ? ? ? ? ? E8
static pfnCVecCtrlUser__OnTeleport CVecCtrlUser__OnTeleport = (pfnCVecCtrlUser__OnTeleport)0x01600FE0;
void DoTeleport() {
	auto Vec = DerefOff<uint32_t>(CharBase, CharVecOff, 0);
	CVecCtrlUser__OnTeleport((uint8_t *)Vec + 4, NULL, NULL, tX, tY);
}
DoFuncFrameWrap(Teleport, 
	tX = x; 
	tY = y;,
	int32_t x, int32_t y)
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
void DoHookSP() {
	uint32_t addr = (uint32_t)SPCave;
	Memory::Write(SP, &addr, 4);
}
DoFuncFrameWrap(HookSP, ;)
void DoUnHookSP() {
	uint32_t orig = SPOrig;
	Memory::Write(SP, &orig, 4);
}
DoFuncFrameWrap(UnHookSP, ;)
void Hacks::SetSP(int32_t x, int32_t y) {
	sX = x;
	sY = y;
}
void __stdcall FetchChar() {
	auto Vec = DerefOff<uint32_t>(CharBase, CharVecOff, 0);
	Char["x"] = round(
		Deref<double>(
			(Vec + VecXOff),
			INT_MAX
		));
	Char["y"] = round(
		Deref<double>(
			(Vec + VecYOff),
			INT_MAX
		));
	Char["breath"] = DerefOff<uint32_t>(CharBase, CharBreathOff, -1);
	Char["attackCount"] = DerefOff<uint32_t>(CharBase, CharAttackCountOff, -1);

	//stats
	Char["hp"] = DerefOff<uint32_t>(StatsBase, StatsHP, -1);
	Char["mp"] = DerefOff<uint32_t>(StatsBase, StatsMP, -1);
	Char["exp"] = DerefOff<double>(StatsBase, StatsEXP, -1);
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
	FetchChar();
	FetchMapInfo();
	FetchMob();
	FetchMap();
}
pair<POINT *, uint64_t> Hacks::GetMobs() {
	WaitForFrame(); //wait if a fetch is currently active
	RefreshMobs = 1; //tell the next fetch to refresh the mobs array
	while (RefreshMobs != 0)
		Sleep(0);
	return Mobs;
}
pair<RECT *, uint64_t> Hacks::GetRopes() {
	WaitForFrame(); //wait if a fetch is currently active
	RefreshRopes = 1; //tell the next fetch to refresh the mobs array
	while (RefreshRopes != 0)
		Sleep(0);
	return Ropes;
}
map<const char *, double> Hacks::GetChar() {
	WaitForFrame();
	return Char;
}
RECT Hacks::GetMap() {
	WaitForFrame();
	return Map;
}
int32_t Hacks::GetMapID() {
	WaitForFrame();
	return MapID;
}
int32_t Hacks::GetMobCount() {
	WaitForFrame();
	return MobCount;
}
POINT Hacks::GetMobClosest() {
	WaitForFrame();
	return MobClosest;
}
void Hacks::WaitForBreath() {
	do {
		WaitForFrame();
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
void DoHookMove() {
	byte nops[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
	Memory::Write(MoveDisable, &nops, 6);
	byte opcode = 0xE9;
	Memory::Write((void*)MoveJmp, &opcode, 1);
	uint32_t distance = jmp(MoveJmp, MoveCave);
	Memory::Write((void*)(MoveJmp + 1), &distance, 4);
	Memory::Write((void*)(MoveJmp + 5), &nops, 3);
}
DoFuncFrameWrap(HookMove, ;)
void DoUnHookMove() {
	byte orig2[] = { 0x89, 0x7C, 0x24, 0x20, 0x89, 0x7C, 0x24, 0x1C };
	Memory::Write((void*)MoveJmp, &orig2, 8);
	byte orig[] = { 0xFF, 0x15, 0xC0, 0xB8, 0xE3, 0x01 };
	Memory::Write(MoveDisable, &orig, 6);
}
DoFuncFrameWrap(UnHookMove, ;)
void Hacks::ResetKeys() {
	for (uint32_t i = 0; i < kLen; i++) {
		if (holdKeys[i] == 1)
			KeyUp(i);
		pressKeys[i] = 0;
	}
}
void Hacks::SetMove(int32_t dirX, int32_t dirY) {
	if (!(dirX < -1 || dirX > 1))
		mX = dirX;
	if (!(dirY < -1 || dirY > 1))
		mY = dirY;
	Moved = 0;
	while (Moved == 0)
		Sleep(0);
}
void Hacks::MoveTowardsX(int32_t targetX) {
	int32_t charX = GetChar()["x"];
	if (targetX > charX) {
		SetMove(1, 0);
	}
	else {
		SetMove(-1, 0);
	}
}
void Hacks::MoveTowardsY(int32_t targetY) {
	int32_t charY = GetChar()["y"];
	if (targetY > charY) {
		SetMove(1, 0);
	}
	else {
		SetMove(-1, 0);
	}
}
void Hacks::SetMoveDelay(int32_t delay) {
	MoveDelay = delay;
}
void Hacks::SetMoveXOff(int32_t off) {
	Xoff = off;
}
bool Hacks::MoveXOffNoStop(int32_t targetX, int32_t off) {
	bool right = targetX > GetChar()["x"];
	bool ret;
	if (right) {
		SetMove(1, 0);
		timeoutWhile(targetX - off > GetChar()["x"])
	}
	else {
		SetMove(-1, 0);
		timeoutWhile(targetX + off < GetChar()["x"])
	}
	return ret;
}
bool Hacks::MoveXOff(int32_t targetX, int32_t off) {
	bool ret = MoveXOffNoStop(targetX, off);
	SetMove(0, 0);
	Sleep(MoveDelay);
	return ret;
}
bool Hacks::MoveX(int32_t targetX) {
	return MoveXOff(targetX, Xoff);
}
int32_t RopePollDelay;
void Hacks::SetRopePollDelay(int32_t delay) {
	RopePollDelay = delay;
}
void Hacks::Rope(int32_t dirY) {
	SetMove(0, dirY);
	int32_t oY; //original y
	do {
		oY = GetChar()["y"];
		Sleep(RopePollDelay);
	} while (GetChar()["y"] != oY);
	SetMove(0, 0);
}
void Hacks::SetTimeout(int32_t t) {
	timeout = t;
}
bool Hacks::RopeY(int32_t targetY) {
	bool down = targetY > GetChar()["y"];
	bool ret;
	if (down) {
		SetMove(0, 1);
		timeoutWhile(targetY > GetChar()["y"])
	}
	else {
		SetMove(0, -1);
		timeoutWhile(targetY < GetChar()["y"])
	}
	SetMove(0, 0);
	return ret;
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
		pushad
		mov fOesi, esi
		mov esi, [ServerBase] // Server Base (TSingleton<CWvsContext>)
		mov ecx, [esi + 0xA4] // Window manager offset
		push Key // 2nd parameter: key code for CTRL
		push VK // 1st parameter: Apparently unused parameter that seems to be related to the type of key that is being pressed. You can just set it to zero
		// ? ? ? ? ? 85 ? 74 ? 8D ? ? 8B ? 8B ? FF ? C2 08 00
		mov edx, 0x0163AB10
		call edx // key press function
		mov esi, fOesi
		popad
	}
}
void __stdcall SendKeys() {
	if (HPMin > 0 && Char["hp"] <= HPMin)
		pressKeys[HPKey] = 1;
	if (MPMin > 0 && Char["hp"] <= MPMin)
		pressKeys[MPKey] = 1;
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
	__asm pushad
	frameActive = 1;
	FetchAll();
	void (*f)();
	while (functions.try_pop(f)) {
		f();
	}
	SendKeys();
	frameActive = 0;
	__asm {
		popad
		push fRet
		ret
	}
}
void Hacks::HookFrame() {
	HINSTANCE hMod = GetModuleHandle("user32.dll");
	byte* dispatchAddy = (byte*)((uint32_t)GetProcAddress(hMod, "DispatchMessageA") + 0x0);
	fRet = (uint32_t)(dispatchAddy)+2;
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
void Hacks::Reset() {
	DisableAutoPortal();
	UnHookMove();
	UnHookSP();
	HPMin = 0;
	MPMin = 0;
	ResetKeys();
	tX = tY = sX = sY = mX = mY = 0;
	RefreshRopes = 0;
	RefreshMobs = 0;
	Moved = 0;
	timeout = 0;
	//UnHookFrame();
}