#ifndef _EXTEND_LUA_OS_H
#define _EXTEND_LUA_OS_H

#include <lua.h>
#include <lauxlib.h>

#include <3ds.h>
#include <citro2d.h>

#include <unistd.h> //getcwd
#include <dirent.h>

void luaextend_os(lua_State *);

#endif
