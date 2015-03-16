#include "inc.h"

/* the Lua interpreter */
lua_State* L = NULL;

void KeyPressNoHook(int32_t key)
{
	PostMessage(FindWindow("MapleStoryClass", 0), WM_KEYDOWN, key, VKtoMS(key));
	PostMessage(FindWindow("MapleStoryClass", 0), WM_KEYUP, key, VKtoMS(key));
}

void PopupInt(int32_t a) {
	MsgBox("%d", a);
}

//sketchy lua macros

#define valas(type) type val = (type)

#define rawwrap(name, func, in, ret, ...) \
static int name(lua_State *L) { \
	in space::func(__VA_ARGS__); \
	ret; \
	return 0; \
}

#define rawsamewrap(func, in, ret, ...) rawwrap(func, func, in, ret, __VA_ARGS__)
#define samewrap(func, ...) rawsamewrap(func, ;, ;, __VA_ARGS__)
#define samewrapRet(func, type, ret, ...) rawsamewrap(func, valas(type), ret, __VA_ARGS__)

#define integer(n) lua_tointeger(L, n)

///////////////////////////////////////

#undef space
#define space Hacks
samewrap(EnableAutoPortal)
samewrap(DisableAutoPortal)

samewrap(HookSP)
samewrap(UnHookSP)

samewrap(KeyUp, integer(1))
samewrap(KeyDown, integer(1))
samewrap(KeyPress, integer(1))

samewrap(Teleport, integer(1), integer(2))
samewrap(SetSP, integer(1), integer(2))
samewrapRet(GetMapID, int32_t, lua_pushinteger(L, val); return 1;)

samewrap(HookMove)
samewrap(UnHookMove)
samewrap(SetMove, integer(1), integer(2))

samewrapRet(GetX, int32_t, lua_pushinteger(L, val); return 1;)
samewrapRet(GetY, int32_t, lua_pushinteger(L, val); return 1;)
samewrap(WaitForBreath)

samewrap(ResetKeys)
samewrap(MoveX, integer(1))
samewrap(MoveXOff, integer(1), integer(2))
samewrap(SetMoveXOff, integer(1))
samewrap(SetMoveDelay, integer(1))

samewrap(Rope, integer(1))

samewrap(SetFaceDelay, integer(1))
samewrap(FaceLeft)
samewrap(FaceRight)

samewrap(KeyHoldFor, integer(1), integer(2))

#undef space
#define space 
samewrap(Sleep, integer(1))
samewrap(KeyPressNoHook, integer(1))
samewrap(PopupInt, integer(1))

///////////////////////////////////////

#define regfuncas(func, name, ...) lua_register(L, name, func);
#define regfunc(func, ...) regfuncas(func, STRINGIFY(func), __VA_ARGS__)
void initLua() {

	/* initialize Lua */
	L = luaL_newstate();

	/* load Lua base libraries */
	luaL_openlibs(L);

	/* register our functions */
///////////////////////////////////////

#undef space
#define space Hacks
regfunc(EnableAutoPortal)
regfunc(DisableAutoPortal)

regfunc(HookSP)
regfunc(UnHookSP)

regfunc(KeyUp, integer(1))
regfunc(KeyDown, integer(1))
regfunc(KeyPress, integer(1))

regfunc(Teleport, integer(1), integer(2))
regfunc(SetSP, integer(1), integer(2))
regfunc(GetMapID, int32_t, lua_pushinteger(L, val); return 1;)

regfunc(HookMove)
regfunc(UnHookMove)
regfunc(SetMove, integer(1), integer(2))

regfunc(GetX, int32_t, lua_pushinteger(L, val); return 1;)
regfunc(GetY, int32_t, lua_pushinteger(L, val); return 1;)
regfunc(WaitForBreath)

regfunc(ResetKeys)
regfunc(MoveX, integer(1))
regfunc(MoveXOff, integer(1), integer(2))
regfunc(SetMoveXOff, integer(1))
regfunc(SetMoveDelay, integer(1))

regfunc(Rope, integer(1))

regfunc(SetFaceDelay, integer(1))
regfunc(FaceLeft)
regfunc(FaceRight)

regfunc(KeyHoldFor, integer(1), integer(2))

#undef space
#define space 
regfunc(Sleep, integer(1))
regfunc(KeyPressNoHook, integer(1))
regfunc(PopupInt, integer(1))

regfuncas(Sleep, "Wait")
///////////////////////////////////////
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