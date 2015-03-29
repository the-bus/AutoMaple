#include "inc.h"

/* the Lua interpreter */
lua_State* L = NULL;

void KeyPressNoHook(int32_t key)
{
	HWND ms = GetMShwnd();
	PostMessage(ms, WM_KEYDOWN, key, VKtoMS(key));
	PostMessage(ms, WM_KEYUP, key, VKtoMS(key));
}

void MessageInt(int32_t a) {
	MsgBox("%d", a);
}

void MessageNum(double a) {
	MsgBox("%f", a);
}

void push(int32_t n) {
	lua_pushinteger(L, n);
}

void push(bool b) {
	lua_pushboolean(L, b);
}

void push(double d) {
	lua_pushnumber(L, d);
}

void POINT2table(POINT p) {
	lua_createtable(L, 0, 2);
	push(p.x);
	lua_setfield(L, -2, "x");
	push(p.y);
	lua_setfield(L, -2, "y");
}

template<typename T>
void arr2table(T * a, void (f)(T), size_t len) {
	lua_createtable(L, len, 0);
	for (uint32_t i = 0; i < len; i++) {
		f(a[i]);
		lua_rawseti(L, -2, i);
	}
}

template<typename T>
void map2table(map<const char *, T> m) {
	lua_createtable(L, 0, m.size());
	for (auto p : m)
	{
		push(p.second);
		lua_setfield(L, -2, p.first);
	}
}

void RECT2table(RECT r) {
	POINT val[] = { POINT{ r.left, r.bottom }, POINT{ r.right, r.top } };
	arr2table<POINT>(val, POINT2table, 2);
}

void log(const char * func, int len) {
	lua_Debug ar;
	lua_getstack(L, 1, &ar);
	lua_getinfo(L, "l", &ar);
	uint32_t line = ar.currentline;
#define mymsg ": "
	uint32_t sz = len + sizeof(mymsg) + 10 + 1;
	char * c = new char[sz];
	char n[11];
	_itoa_s(line, n, 10);
	c[0] = '\0';
	strcat_s(c, sz, n);
	strcat_s(c, sz, mymsg);
	strcat_s(c, sz, func);
	Log(c);
	delete c;
}

//sketchy lua macros

#define getval() auto val = 

#define pushval() push(val); return 1;

#define rawelementwrap(func, name, in, ret, ...) { name, [](lua_State *L) { \
	log(name, sizeof(name)); \
	in space::func(__VA_ARGS__); \
	ret; \
	return 0; \
} },

#define wrap(func, name, ...) rawelementwrap(func, name, ;, ;, __VA_ARGS__)
#define wrapRet(func, name, ret, ...) rawelementwrap(func, name, getval(), ret, __VA_ARGS__)

#define rawsamewrap(func, in, ret, ...) rawelementwrap(func, STRINGIFY(func), in, ret, __VA_ARGS__)
#define samewrap(func, ...) rawsamewrap(func, ;, ;, __VA_ARGS__)
#define samewrapRetVal(func, ...) rawsamewrap(func, getval(), pushval(), __VA_ARGS__)
#define samewrapVal(func, ret, ...) rawsamewrap(func, getval(), ret, __VA_ARGS__)

#define integer(n) lua_tointeger(L, n)
#define number(n) lua_tonumber(L, n)

///////////////////////////////////////

static const luaL_Reg mapleLib[] = {
	
#undef space
#define space Hacks
	samewrap(KeyDown, integer(1))
	samewrap(KeyUp, integer(1))
	samewrap(KeyPress, integer(1))
	samewrap(KeySpam, integer(1))
	samewrap(KeyHoldFor, integer(1), integer(2))
	samewrap(KeyUnSpam, integer(1))
	samewrap(ResetKeys)

	samewrap(EnableAutoPortal)
	samewrap(DisableAutoPortal)

	samewrap(Teleport, integer(1), integer(2))
	samewrap(KamiTeleport, integer(1), integer(2))

	samewrap(HookSP)
	samewrap(UnHookSP)
	samewrap(SetSP, integer(1), integer(2))

	samewrap(WaitForBreath)

	samewrapRetVal(GetMapID)
	samewrapRetVal(GetMobCount)
	samewrapVal(GetMobClosest, POINT2table(val); return 1;)
	samewrapVal(GetChar, map2table(val);  return 1;)
	samewrapVal(GetMobs, arr2table(val.first, POINT2table, val.second); return 1;)
	samewrapVal(GetRopes, arr2table(val.first, RECT2table, val.second); return 1;)
	samewrapVal(GetPortals, arr2table(val.first, map2table, val.second); return 1;)
	samewrapVal(GetMap, RECT2table(val); return 1;)

	samewrap(AutoHP, integer(1), integer(2)) //key, minimum value
	samewrap(AutoMP, integer(1), integer(2)) //key, minimum value

	samewrapRetVal(SendPacket, lua_tostring(L, 1))

	samewrap(SetMoveDelay, integer(1))
	samewrap(SetMoveXOff, integer(1))
	samewrap(SetRopePollDelay, integer(1))

	samewrap(HookMove)
	samewrap(UnHookMove)

	samewrap(SetMove, integer(1), integer(2))

	samewrapRetVal(MoveX, integer(1))
	samewrapRetVal(MoveXOff, integer(1), integer(2))
	samewrapRetVal(MoveXOffNoStop, integer(1), integer(2))

	samewrap(Rope, integer(1))
	samewrapRetVal(RopeY, integer(1))

	samewrap(FaceLeft)
	samewrap(FaceRight)

	samewrap(MoveTowardsX, integer(1))
	samewrap(MoveTowardsY, integer(1))

	samewrap(SetTimeout, integer(1))

#undef space
#define space 
	samewrap(Sleep, integer(1))
	wrap(Sleep, "Wait", integer(1))
	
	samewrap(KeyPressNoHook, integer(1))
	samewrap(MessageInt, integer(1))
	samewrap(MessageNum, number(1))
	samewrap(Message, lua_tolstring(L, 1, NULL))
	samewrap(Log, lua_tolstring(L, 1, NULL))

	{ NULL, NULL }
};

int index(lua_State *L, const char * c) {
	lua_Debug ar;
	lua_getstack(L, 1, &ar);
	lua_getinfo(L, "nSl", &ar);
	uint32_t line = ar.currentline;
	const char emsg[] = "Error! The following does not exist in ";
	uint32_t sz;
	const char * err = lua_tolstring(L, -1, &sz);
#define lineMsg "\nLine: "
	char ln[10 + sizeof(lineMsg) + 1];
	sprintf_s(ln, lineMsg "%d", line);
	sz += strlen(c) + sizeof(emsg) + strlen(ln) + 1;
	char * msg = new char[sz];
	if (!msg)
		goto end;
	msg[0] = '\0';
	strcat_s(msg, sz, emsg);
	strcat_s(msg, sz, c);
	strcat_s(msg, sz, err);
	strcat_s(msg, sz, ln);
	Message(msg);
	end:
	delete msg;
	clean();
	return 0;
}

///////////////////////////////////////

void initLua(const char * buf) {
	/* initialize Lua */
	L = luaL_newstate();

	/* load Lua base libraries */
	luaL_openlibs(L);

	lua_getglobal(L, "_G");
	luaL_newmetatable(L, "_GMETA");
	lua_pushcclosure(L, [](lua_State *L) -> int {
		index(L, "global: ");
		return 0;
	}, 0);
	lua_setfield(L, -2, "__index");
	lua_setmetatable(L, -2);

	luaL_newlib(L, mapleLib);
	lua_setglobal(L, "maple");

	lua_getglobal(L, "maple");
	luaL_newmetatable(L, "mapleMETA");
	lua_pushcclosure(L, [](lua_State *L) -> int {
		index(L, "maple: ");
		return 0;
	}, 0);
	lua_setfield(L, -2, "__index");
	lua_setmetatable(L, -2);

	/* run the script */
	int error = luaL_loadfile(L, buf);
	if (error) // if non-0, then an error
	{
		// the top of the stack should be the error string
		if (lua_isstring(L, lua_gettop(L))) {
			// get the top of the stack as the error and pop it off
			Message(lua_tostring(L, lua_gettop(L)));
			lua_pop(L, 1);
		} else {
			Message("Unknown error");
		}
	}
	else
	{
		// if not an error, then the top of the stack will be the function to call to run the file
		lua_pcall(L, 0, LUA_MULTRET, 0); // once again, returns non-0 on error, you should probably add a little check
	}
	/* cleanup Lua */
	lua_close(L);
	L = NULL;
	
}