#include "lua_time.h"

static int lua_sleep(lua_State *L) {
	lua_Number seconds = luaL_checknumber(L, 1);

	struct timespec ts;
    ts.tv_sec = seconds;
    ts.tv_nsec = (seconds-ts.tv_sec) * 1000000000; //get decimal part, then times
    nanosleep(&ts, NULL);
	return 0;
}

void luaopen_time(lua_State *L) {
	lua_newtable(L);
		lua_pushstring(L, "sleep");
		lua_pushcfunction(L, lua_sleep);
		lua_settable(L, -3);
	lua_setglobal(L, "time");
}
