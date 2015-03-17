#include "inc.h"

/* the Lua interpreter */
lua_State* L = NULL;

void KeyPressNoHook(int32_t key)
{
	HWND MShwnd = FindProcessWindow("MapleStoryClass", GetCurrentProcessId());
	PostMessage(MShwnd, WM_KEYDOWN, key, VKtoMS(key));
	PostMessage(MShwnd, WM_KEYUP, key, VKtoMS(key));
}

void PopupInt(int32_t a) {
	MsgBox("%d", a);
}

void POINT2table(POINT * p) {
	lua_newtable(L);
	lua_pushinteger(L, p->x);
	lua_setfield(L, -2, "x");
	lua_pushinteger(L, p->y);
	lua_setfield(L, -2, "y");
}

template<typename T>
void arr2table(T a, void (f)(T), size_t len) {
	lua_createtable(L, len, 0);
	for (uint32_t i = 0; i < len; i++) {
		//MsgBox("%d", i);
		f(&a[i]);
		lua_rawseti(L, -2, i);
	}
}

//sketchy lua macros

#define valas(type) type val = (type)

#define rawlambdawrap(func, in, ret, ...) \
[](lua_State *L) { \
	in space::func(__VA_ARGS__); \
	ret; \
	return 0; \
}

#define rawelementwrap(func, name, in, ret, ...) { name, rawlambdawrap(func, in, ret, __VA_ARGS__) },

#define wrap(func, name, ...) rawelementwrap(func, name, ;, ;, __VA_ARGS__)
#define wrapRet(func, name, type, ret, ...) rawelementwrap(func, name, valas(type), ret, __VA_ARGS__)

#define rawsamewrap(func, in, ret, ...) rawelementwrap(func, STRINGIFY(func), in, ret, __VA_ARGS__)
#define samewrap(func, ...) rawsamewrap(func, ;, ;, __VA_ARGS__)
#define samewrapRetVal(func, type, ret, ...) rawsamewrap(func, valas(type), ret, __VA_ARGS__)
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

	samewrap(Teleport, integer(1), integer(2))
	samewrap(SetSP, integer(1), integer(2))
	samewrapRetVal(GetMapID, int32_t, lua_pushinteger(L, val); return 1;)

	samewrap(HookMove)
	samewrap(UnHookMove)
	samewrap(SetMove, integer(1), integer(2))

	samewrap(WaitForBreath)

	samewrap(ResetKeys)
	samewrap(MoveX, integer(1))
	samewrap(MoveXOff, integer(1), integer(2))
	samewrap(SetMoveXOff, integer(1))
	samewrap(SetMoveDelay, integer(1))

	samewrap(SetRopePollDelay, integer(1))
	samewrap(Rope, integer(1))
	samewrap(RopeY, integer(1))

	samewrap(SetFaceDelay, integer(1))
	samewrap(FaceLeft)
	samewrap(FaceRight)

	samewrap(KeyHoldFor, integer(1), integer(2))

	samewrapRetVal(GetMobCount, int32_t, lua_pushinteger(L, val); return 1;)
	samewrapRetVal(GetMobClosest, POINT, POINT2table(&val); return 1;)
	samewrapRetVal(GetChar, POINT, POINT2table(&val); return 1;)

	samewrapRetVal(GetMobs, POINT *, arr2table<POINT *>(val, POINT2table, Hacks::GetMobCount()); return 1;)

	#undef space
	#define space 
	samewrap(Sleep, integer(1))
	wrap(Sleep, "Wait", integer(1))

	samewrap(KeyPressNoHook, integer(1))
	samewrap(PopupInt, integer(1))
	samewrap(Message, lua_tolstring(L, 1, NULL))

	{ NULL, NULL }
};

///////////////////////////////////////

void initLua() {

	/* initialize Lua */
	L = luaL_newstate();

	/* load Lua base libraries */
	luaL_openlibs(L);
	luaL_newlib(L, mapleLib);
	lua_setglobal(L, "maple");

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