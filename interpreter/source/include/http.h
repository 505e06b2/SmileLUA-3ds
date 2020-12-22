#ifndef _HTTP_H
#define _HTTP_H

#include <3ds.h>
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>

typedef struct http_response_s {
	httpcContext context;
	u32 size;
} HTTPResponse;

void luaextend_io_addHTTP(lua_State *);

#endif
