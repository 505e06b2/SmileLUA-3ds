#include "extend_lua_os.h"

static int lua_mainloop(lua_State *L) {
	int ret = aptMainLoop();
	if(ret) {
		gfxFlushBuffers();
		gfxSwapBuffers();
		gspWaitForVBlank();

		hidScanInput();
	}
	lua_pushboolean(L, ret);
	return 1;
}

static int lua_chdir(lua_State *L) {
	const char *path = luaL_checkstring(L, 1);
	lua_pushboolean(L, chdir(path));
	return 1;
}

void luaextend_os(lua_State *L) {
	lua_getglobal(L, "os");
		lua_pushstring(L, "mainLoop");
		lua_pushcfunction(L, lua_mainloop);
		lua_settable(L, -3);

		lua_pushstring(L, "chdir");
		lua_pushcfunction(L, lua_chdir);
		lua_settable(L, -3);
}
