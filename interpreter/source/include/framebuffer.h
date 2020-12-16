#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

#include <3ds.h>

#include <lua.h>
#include <lauxlib.h>

#define TOP_HEIGHT 240
#define TOP_WIDTH 400
#define TOP_SIZE (TOP_HEIGHT * TOP_WIDTH)

#define BOTTOM_HEIGHT 240
#define BOTTOM_WIDTH 320
#define BOTTOM_SIZE (BOTTOM_HEIGHT * BOTTOM_WIDTH)

void luaextend_io_addFramebuffer(lua_State *);

#endif
