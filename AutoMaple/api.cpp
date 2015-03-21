#include "inc.h"

/* the Lua interpreter */
lua_State* L = NULL;

void KeyPressNoHook(int32_t key)
{
	PostMessage(hwnd, WM_KEYDOWN, key, VKtoMS(key));
	PostMessage(hwnd, WM_KEYUP, key, VKtoMS(key));
}

void MessageInt(int32_t a) {
	MsgBox("%d", a);
}

void POINT2table(POINT p) {
	lua_createtable(L, 0, 2);
	lua_pushinteger(L, p.x);
	lua_setfield(L, -2, "x");
	lua_pushinteger(L, p.y);
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

void RECT2table(RECT r) {
	POINT val[] = { POINT{ r.left, r.bottom }, POINT{ r.right, r.top } };
	arr2table<POINT>(val, POINT2table, 2);
}

//sketchy lua macros

#define getval() auto val = 

#define rawlambdawrap(func, in, ret, ...) \
[](lua_State *L) { \
	in space::func(__VA_ARGS__); \
	ret; \
	return 0; \
}

#define rawelementwrap(func, name, in, ret, ...) { name, rawlambdawrap(func, in, ret, __VA_ARGS__) },

#define wrap(func, name, ...) rawelementwrap(func, name, ;, ;, __VA_ARGS__)
#define wrapRet(func, name, ret, ...) rawelementwrap(func, name, getval(), ret, __VA_ARGS__)

#define rawsamewrap(func, in, ret, ...) rawelementwrap(func, STRINGIFY(func), in, ret, __VA_ARGS__)
#define samewrap(func, ...) rawsamewrap(func, ;, ;, __VA_ARGS__)
#define samewrapRetVal(func, ret, ...) rawsamewrap(func, getval(), ret, __VA_ARGS__)
#define samewrapRet(func, ret, ...) rawsamewrap(func, ;, ret, __VA_ARGS__)


#define integer(n) lua_tointeger(L, n)

///////////////////////////////////////

static const luaL_Reg mapleLib[] = {
	
	#undef space
	#define space Hacks
	samewrap(EnableAutoPortal)
	samewrap(DisableAutoPortal)
	samewrap(HookSP)
	samewrap(UnHookSP)

	samewrap(KeyUp, integer(1))
	samewrap(KeyDown, integer(1))
	samewrap(KeyPress, integer(1))
	samewrap(KeySpam, integer(1))
	samewrap(KeyUnSpam, integer(1))
	samewrap(ResetKeys)

	samewrap(Teleport, integer(1), integer(2))
	samewrap(SetSP, integer(1), integer(2))
	samewrapRetVal(GetMapID, lua_pushinteger(L, val); return 1;)

	samewrap(HookMove)
	samewrap(UnHookMove)
	samewrap(SetMove, integer(1), integer(2))

	samewrap(WaitForBreath)

	samewrap(MoveX, integer(1))
	samewrap(MoveXOff, integer(1), integer(2))
	samewrap(MoveXOffNoStop, integer(1), integer(2))
	samewrap(SetMoveXOff, integer(1))
	samewrap(SetMoveDelay, integer(1))

	samewrap(SetRopePollDelay, integer(1))
	samewrap(Rope, integer(1))
	samewrap(RopeY, integer(1))

	samewrap(FaceLeft)
	samewrap(FaceRight)

	samewrap(KeyHoldFor, integer(1), integer(2))

	samewrapRetVal(GetMobCount, lua_pushinteger(L, val); return 1;)
	samewrapRetVal(GetMobClosest, POINT2table(val); return 1;)
	samewrapRetVal(GetChar, POINT2table(val); return 1;)

	samewrapRetVal(GetMap, RECT2table(val); return 1;)

	samewrapRetVal(GetMobs, arr2table<POINT>(val.first, POINT2table, val.second); return 1;)
	samewrapRetVal(GetRopes, arr2table<RECT>(val.first, RECT2table, val.second); return 1;)

	#undef space
	#define space 
	samewrap(Sleep, integer(1))
	wrap(Sleep, "Wait", integer(1))

	samewrap(KeyPressNoHook, integer(1))
	samewrap(MessageInt, integer(1))
	samewrap(Message, lua_tolstring(L, 1, NULL))

	{ NULL, NULL }
};

int index(lua_State *L) {
	Message("Error! The following does not exist in the maple table:");
	Message(lua_tolstring(L, -1, NULL));
	clean();
	return 0;
}

static const luaL_Reg meta[] = {
	{ "__index", index },
	{ NULL, NULL }
};

///////////////////////////////////////

void initLua() {

	/* initialize Lua */
	L = luaL_newstate();

	/* load Lua base libraries */
	luaL_openlibs(L);

	luaL_newlib(L, mapleLib);
	lua_pushvalue(L, -1);
	lua_setglobal(L, "maple");

	luaL_newmetatable(L, "MyMetaTableFallback");
	luaL_setfuncs(L, meta, 0);
	lua_setmetatable(L, -2);

	/* run the script */
	char file[] = "\\test.lua";
	char buf[32768];
	GetModuleFileName(NULL, buf, 32768);
	PathRemoveFileSpec(buf);
	strcat_s(buf, file);
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
		Message("No errors");
		// if not an error, then the top of the stack will be the function to call to run the file
		lua_pcall(L, 0, LUA_MULTRET, 0); // once again, returns non-0 on error, you should probably add a little check
	}
	/* cleanup Lua */
	lua_close(L);
	L = NULL;
	
}