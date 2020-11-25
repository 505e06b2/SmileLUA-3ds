#ifndef _LUA_TIME_H
#define _LUA_TIME_H

#include <lua.h>
#include <lauxlib.h>

#include <time.h> //sleep

void luaopen_time(lua_State *);

#endif
