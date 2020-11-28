#ifndef _EXTEND_LUA_IO_H
#define _EXTEND_LUA_IO_H

#include <lua.h>
#include <lauxlib.h>

#include <3ds.h>
#include <unistd.h> //getcwd

void luaextend_io(lua_State *);

#endif
