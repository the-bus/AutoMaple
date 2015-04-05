#include "Hacks.h"
#include <concurrent_queue.h>

#define POLL 50

CRITICAL_SECTION frame;

int32_t sX, sY;
int32_t mX, mY;
int32_t ktX, ktY;
int32_t ptX, ptY;

atomic<uint8_t> frameDone;

uint32_t fRet;
uint32_t fOesi; //frame cave's original esi
uint32_t sOeax; //used by SP to store eax
uint32_t mOeax;

atomic<uint8_t> interrupt;

#define kLen 0x100
#define KEY_UP 0
#define KEY_HOLDING 1
#define KEY_RELEASING 2
#define KEY_PRESSING 3
#define KEY_SPAMMING 4
#define KEY_NOT_HOLDING 5
#define KEY_NOT_SPAMMING 6
atomic<uint8_t> keyStates[kLen];

strmap(double) Char;
uint32_t MapID;

int32_t MobCount;
POINT MobClosest;
arrpair(POINT *) Mobs;
atomic<uint8_t> RefreshMobs;

arrpair(RECT *) Ropes;
RECT Map;
atomic<uint8_t> RefreshRopes;

arrpair(strmap(int32_t) *) Portals;
atomic<uint8_t> RefreshPortals;

arrpair(strmap(int32_t) *) Inventory[5];
atomic<uint8_t> RefreshInventory;

int32_t Xoff;
int32_t MoveDelay;

uint8_t Moved;

time_t timeout;

int32_t HPKey;
int32_t HPMin;
int32_t MPKey;
int32_t MPMin;

int32_t ptP;
byte * PortalSpace[128] = { 0 };
//uint32_t ptRet;

int32_t ItemCount;

Concurrency::concurrent_queue<void(*)()> functions;

#define timeoutWhile(cond) \
do { \
		time_t ftime = time(NULL) + timeout; \
		while (cond && (timeout <= 0 || time(NULL) < ftime) && interrupt == 0) \
			Sleep(POLL); \
		ret = !(cond); \
} while (0);
#define DoFuncFrameWrap(func, code, ...) \
void Hacks::func(__VA_ARGS__) { \
	code \
	DoFuncFrame(Do ## func); \
}
#define GetWrap(type, attr, code) \
type Hacks::Get ## attr() { \
	code \
	EnterCriticalSection(&frame); \
	auto ret = attr; \
	LeaveCriticalSection(&frame); \
	return ret; \
}
#define GetWrapLock(type, attr) \
type Hacks::Get ## attr() { \
	Refresh ## attr = 1; \
	while (Refresh ## attr != 0) \
		Sleep(POLL); \
	auto ret = attr; \
	return ret; \
}
void Hacks::Interrupt() {
	interrupt = 1;
}
void WaitForReframe() {
	frameDone = 0;
	while (frameDone == 0)
		Sleep(POLL);
}
void DoFuncFrame(void(*f)()) {
	EnterCriticalSection(&frame);
	functions.push(f);
	LeaveCriticalSection(&frame);
	WaitForReframe();
}
void Hacks::KeySpam(int32_t k) {
	KeyUp(k);
	keyStates[k] = KEY_NOT_SPAMMING;
	while (keyStates[k] != KEY_SPAMMING)
		Sleep(POLL);
}
void Hacks::KeyUnSpam(int32_t k) {
	keyStates[k] = KEY_UP;
}
void Hacks::KeyPress(int32_t k)
{
	KeyUp(k);
	keyStates[k] = KEY_PRESSING;
	while (keyStates[k] != KEY_UP)
		Sleep(POLL);
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
	keyStates[k] = KEY_NOT_HOLDING;
	while (keyStates[k] != KEY_HOLDING)
		Sleep(POLL);
}
void Hacks::KeyUp(int32_t k)
{
	if (keyStates[k] != KEY_HOLDING)
		return;
	keyStates[k] = KEY_RELEASING;
	while (keyStates[k] != KEY_UP)
		Sleep(POLL);
}
void DoEnableAutoPortal() {
	byte enable = 0x75;
	Memory::Write((void*)AutoPortal, &enable, 1);
}
DoFuncFrameWrap(EnableAutoPortal)
void DoDisableAutoPortal() {
	byte disable = 0x74;
	Memory::Write((void*)AutoPortal, &disable, 1);
}
DoFuncFrameWrap(DisableAutoPortal)
/*translates to:
void Hacks::DisableAutoPortal() {
	DoFuncFrame(DoDisableAutoPortal);
}*/
typedef void(__fastcall *pfnCVecCtrlUser__OnTeleport)(void *pthis, void *edx, void *, long x, long y);
//8B ? 24 ? 8B ? ? 8B ? ? ? 8D ? ? 8B ? ? ? ? ? ? FF ? 85 C0 ? ? ? ? ? ? ? ? ? E8
static pfnCVecCtrlUser__OnTeleport CVecCtrlUser__OnTeleport = (pfnCVecCtrlUser__OnTeleport)0x01600FE0;
void DoKamiTeleport() {
	auto Vec = DerefOff<uint32_t>(CharBase, CharVecOff, 0);
	CVecCtrlUser__OnTeleport((uint8_t *)Vec + 4, NULL, NULL, ktX, ktY);
}
DoFuncFrameWrap(KamiTeleport, 
	ktX = x; 
	ktY = y; ,
	int32_t x, int32_t y)
__declspec(naked) void SPCave() {
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
DoFuncFrameWrap(HookSP)
void DoUnHookSP() {
	uint32_t orig = SPOrig;
	Memory::Write(SP, &orig, 4);
}
DoFuncFrameWrap(UnHookSP)
void Hacks::SetSP(int32_t x, int32_t y) {
	sX = x;
	sY = y;
}
typedef int(__fastcall* lpfnCWvsContext__GetCharacterLevel)(LPVOID lpvClass, LPVOID lpvEDX);
lpfnCWvsContext__GetCharacterLevel CWvsContext__GetCharacterLevel = (lpfnCWvsContext__GetCharacterLevel)0x005B4E30; //B9 ? ? ? ? E9 ? ? ? ? CC CC CC CC CC CC 83 3D
void FetchChar() {
	auto Vec = DerefOff<uint32_t>(CharBase, CharVecOff, 0);
	Char["level"] = CWvsContext__GetCharacterLevel((void*)0x01E49A68, NULL);
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
void FetchMapInfo() {
	MapID = DerefOff<uint32_t>(MyMapInfo, MyMapIDOff, 0);
}
void FetchMob() {
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
		delete [] Mobs.first;
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
void FetchRopes() {
	auto Rope = DerefOff<uint32_t>(MapBase, RopeOff, 0);
	auto RopeCount = Deref<uint32_t>(Rope - 4, 0) - 1;
	delete [] Ropes.first;
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
		Ropes.first[i] = RECT{ x, y1, x, y2 };
		Rope += 0x20;
		Ropes.second = i + 1;
	}
	//RECT * arr = new RECT[rects.size()];
	//copy(rects.begin(), rects.end(), arr);
	//Ropes.first = arr;
}
void FetchPortals(void(*f)(uint32_t, uint32_t)) {
	auto Portal = DerefOff<uint32_t>(PortalBase, PortalsOff, 0);
	auto PortalCount = Deref<uint32_t>(Portal + PortalsCountOff, 0);
	delete [] Portals.first;
	Portals.first = new strmap(int32_t)[PortalCount];
	Portals.second = PortalCount;
	Portal += PortalsFirst;
	for (uint32_t i = 0; i < PortalCount; i++) {
		auto cur = Deref<uint32_t>(Portal, 0);
		Portals.first[i]["x"] = Deref<int32_t>(cur + PortalXOff, INT_MAX);
		Portals.first[i]["y"] = Deref<int32_t>(cur + PortalYOff, INT_MAX);
		Portals.first[i]["tm"] = Deref<int32_t>(cur + PortalTMOff, 0);
		if (f != NULL)
			f(cur, i);
		Portal += PortalNextOff;
	}
}
__declspec(naked) void FakePortal() {
	__asm {
		lea edi, [PortalSpace]
		push 0x014DB229 // 8B ?? ?? 6A ?? 89 ?? ?? ?? 8B ?? ?? 68 ?? ?? ?? ?? 83 ?? ?? 6A ?? 50 51 8B ?? ?? ?? 89
		ret
	}
}
__declspec(naked) void RawTeleport() {
	__asm {
		//sub esp, 0x0C
		//push ebx
		//push esi
		pushad
		mov ebx, ptX
		mov[PortalSpace + 0x0C], ebx
		mov ebx, ptY
		mov[PortalSpace + 0x10], ebx
		mov edi, ptP
		mov eax, [edi + 0x24]
		mov ebx, [edi + 0x04]
		push 00
		push 00
		push eax
		push ebx
		push 00
		push 00
		mov ecx, CharBase
		mov ecx, [ecx]
		mov eax, 0x014DADB0
		call eax // CUserLocal__TryRegisterTeleport - addy on call E8 ? ? ? ? 85 ? 0F ? ? ? ? ? 8D ? ? ? 68 ? ? ? ? ? E8 ? ? ? ? 8B C8 E8 ? ? ? ? 8B 00 6A 64
		jmp Ending
		Ending:
		popad
		ret
		//push ptRet
		//ret
	}
}
__declspec(naked) void BlockSend() {
	__asm ret 0x0004
}
void DoTeleport() {
	ptP = NULL;
	FetchPortals([](uint32_t p, uint32_t i) {
		if (ptP == NULL && Portals.first[i]["tm"] != 999999999) {
			ptP = p;
		}
	});
	if (ptP == NULL)
		return;

	byte b;
	int32_t i;

	/*b = 0xE9;
	ptRet = 0x0163CAE0+5;
	Memory::Write((void*)0x0163CAE0, &b, 1); // 83 ?? ?? 53 56 57 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 33
	i = jmp(0x0163CAE0, RawTeleport);
	Memory::Write((byte*)0x0163CAE0 + 1, &i, 4);*/

	b = 0xE9;
	Memory::Write((void*)0x014DB213, &b, 1); // 83 ?? ?? 53 56 57 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 33
	i = jmp(0x014DB213, FakePortal);
	Memory::Write((byte*)0x014DB213 + 1, &i, 4);
	b = 0x90;
	Memory::Write((byte*)0x014DB213 + 5, &b, 1);

	b = 0xE8;
	Memory::Write((byte*)0x014DB353, &b, 1); // E8 ?? ?? ?? ?? 8D ?? ?? ?? ?? ?? ?? C7 ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? E8 ?? ?? ?? ?? E9 ?? ?? ?? ?? 83 ?? ?? ?? ?? ?? ?? ?? 0F
	i = jmp(0x014DB353, BlockSend);
	Memory::Write((byte*)0x014DB353 + 1, &i, 4);

	byte nops[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
	Memory::Write((void*)0x014DB05C, &nops, sizeof(nops)); //// 0F ?? ?? ?? ?? ?? 8B ?? ?? ?? ?? ?? 8D ?? ?? ?? ?? ?? ?? 52 89 ?? ?? ?? E8 ?? ?? ?? ?? 8B ?? ?? 8D

	b = 0x74;
	Memory::Write((void*)0x014DADF5, &b, 1);// 75 ?? 8B ?? ?? 8B ?? ?? 83 ?? ?? FF ?? 8B ?? 8B ?? 8B ?? ?? FF ?? 85 ?? 75 ?? 3B
	
	RawTeleport();
}
DoFuncFrameWrap(Teleport,
	ptX = x;
	ptY = y;,
	int32_t x, int32_t y)
void FetchInventory() {
	if (RefreshInventory == 0)
		return;
	vector<maple::item*> tabs[5];
	get_inv(tabs);
	for (uint32_t i = 0; i < 5; i++) {
		auto tab = tabs[i];
		int j = 0;
		delete Inventory[i].first;
		Inventory[i].first = new strmap(int32_t)[tab.size()];
		for (auto item : tab) {
			//Inventory[i].first[j].insert(make_pair("index", item->index));
			Inventory[i].first[j].insert(make_pair("id", item->item_id));
			Inventory[i].first[j].insert(make_pair("quantity", item->quantity));
			j++;
		}
		Inventory[i].second = j;
	}
	RefreshInventory = 0;
}
void FetchMap() {
	auto MapLeft = DerefOff<int32_t>(MapBase, MapLeftOff, 0);
	auto MapRight = DerefOff<int32_t>(MapBase, MapRightOff, 0);
	auto MapTop = DerefOff<int32_t>(MapBase, MapTopOff, 0);
	auto MapBottom = DerefOff<int32_t>(MapBase, MapBottomOff, 0);
	Map = RECT{ MapLeft, MapTop, MapRight, MapBottom };
	if (RefreshRopes != 0) {
		FetchRopes();
		RefreshRopes = 0;
	}
	if (RefreshPortals != 0) {
		FetchPortals(NULL);
		RefreshPortals = 0;
	}
}
void FetchItems() {
	ItemCount = DerefOff<int32_t>(ItemBase, ItemCountOff, -1);
}
void FetchAll() {
	FetchChar();
	FetchMapInfo();
	FetchMob();
	FetchMap();
	FetchItems();
	FetchInventory();
}
GetWrapLock(arrpair(strmap(int32_t) *) *, Inventory)
GetWrapLock(arrpair(strmap(int32_t) *), Portals)
GetWrapLock(arrpair(POINT *), Mobs)
GetWrapLock(arrpair(RECT *), Ropes)
GetWrap(strmap(double), Char)
GetWrap(RECT, Map)
GetWrap(int32_t, MapID)
GetWrap(int32_t, MobCount)
GetWrap(POINT, MobClosest)
GetWrap(int32_t, ItemCount)
void Hacks::WaitForBreath() {
	do {
		EnterCriticalSection(&frame);
		uint32_t t = Char["breath"];
		LeaveCriticalSection(&frame);
		Sleep(Char["breath"]);
	} while (Char["breath"] != 0);
}
__declspec(naked) void MoveCave() {
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
DoFuncFrameWrap(HookMove)
void DoUnHookMove() {
	byte orig2[] = { 0x89, 0x7C, 0x24, 0x20, 0x89, 0x7C, 0x24, 0x1C };
	Memory::Write((void*)MoveJmp, &orig2, 8);
	byte orig[] = { 0xFF, 0x15, 0xC0, 0xB8, 0xE3, 0x01 };
	Memory::Write(MoveDisable, &orig, 6);
}
DoFuncFrameWrap(UnHookMove)
void Hacks::ResetKeys() {
	for (uint32_t i = 0; i < kLen; i++) {
		KeyUp(i);
		keyStates[i] = KEY_UP;
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
void Hacks::FaceTowards(int x) {
	MoveTowardsX(x);
	SetMove(0, 0);
}
void Hacks::KeyHoldFor(int32_t k, int32_t delay) {
	KeyDown(k);
	Sleep(delay);
	KeyUp(k);
}
void SendKey(uint32_t VK, uint32_t mask) {
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
void SendKeys() {
	if (HPMin > 0 && Char["hp"] <= HPMin)
		keyStates[HPKey] = KEY_PRESSING;
	if (MPMin > 0 && Char["hp"] <= MPMin)
		keyStates[MPKey] = KEY_PRESSING;
	uint8_t temp;
	for (uint32_t i = 0; i < kLen; i++) {
		if (keyStates[i] == KEY_HOLDING || keyStates[i] == KEY_NOT_HOLDING) {
			SendKey(i, MS_DOWN);
			temp = KEY_NOT_HOLDING;
			keyStates[i].compare_exchange_strong(temp, KEY_HOLDING);
		}
		else if (keyStates[i] == KEY_RELEASING) {
			SendKey(i, MS_PRESS);
			SendKey(i, MS_UP);
			temp = KEY_RELEASING;
			keyStates[i].compare_exchange_strong(temp, KEY_UP);
		}
		else if (keyStates[i] == KEY_PRESSING || keyStates[i] == KEY_SPAMMING || keyStates[i] == KEY_NOT_SPAMMING) {
			SendKey(i, MS_DOWN);
			SendKey(i, MS_PRESS);
			SendKey(i, MS_UP);
			temp = KEY_PRESSING;
			keyStates[i].compare_exchange_strong(temp, KEY_UP);
			temp = KEY_NOT_SPAMMING;
			keyStates[i].compare_exchange_strong(temp, KEY_SPAMMING);
		}
	}
}
__declspec(naked) void FrameCave() {
	__asm pushad
	EnterCriticalSection(&frame);
	FetchAll();
	void (*f)();
	while (functions.try_pop(f)) {
		f();
	}
	SendKeys();
	frameDone = 1;
	LeaveCriticalSection(&frame);
	__asm {
		popad
		push fRet
		ret
	}
}
void Hacks::HookFrame() {
	DeleteCriticalSection(&frame);
	InitializeCriticalSection(&frame);
	frameDone = 0;
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
	ptX = ptY = ktX = ktY = sX = sY = mX = mY = 0;
	RefreshRopes = 0;
	RefreshMobs = 0;
	RefreshInventory = 0;
	Moved = 0;
	timeout = 0;
	interrupt = 0;
	//UnHookFrame();
}