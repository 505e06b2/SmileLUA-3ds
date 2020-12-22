#include "http.h"

#include <stdio.h>

static int http_download(lua_State *L) {
	httpcContext *context = luaL_checkudata(L, 1, "http_get");
	if(!context) luaL_error(L, "Connection already closed");
	u32 downloaded_size;
	const int size = luaL_checkinteger(L, 2);
	if(size < 1) luaL_error(L, "Size is invalid (less than 1)");

	char *buffer = malloc(size);
		httpcDownloadData(context, (u8 *)buffer, size, &downloaded_size);
		lua_pushlstring(L, buffer, downloaded_size);
	free(buffer);
	lua_pushboolean(L, downloaded_size == size); //more
	return 2;
}

static int http_close(lua_State *L) {
	httpcContext *context = lua_touserdata(L, 1);
	if(context) {
		httpcCancelConnection(context);
		httpcCloseContext(context);
	}
	return 0;
}

static int http_get_index(lua_State *L) {
	httpcContext *context = luaL_checkudata(L, 1, "http_get");
	const char *index = luaL_checkstring(L, 2);

	if(strcmp("read", index) == 0) {
		lua_pushcfunction(L, http_download);
		return 1;

	} else if(strcmp("close", index) == 0) {
		lua_pushcfunction(L, http_close);
		return 1;
	}

	return 0;
}

static int http_get(lua_State *L) {
	const char *url = luaL_checkstring(L, 1);
	if(strncmp(url, "http", 4) != 0) {
		luaL_error(L, "Not an HTTP URL");
		return 0;
	}

	httpcContext context;

	httpcOpenContext(&context, HTTPC_METHOD_GET, url, 1);
	httpcSetSSLOpt(&context, SSLCOPT_DisableVerify); //enable HTTPS by disabling
	httpcSetKeepAlive(&context, HTTPC_KEEPALIVE_ENABLED);
	httpcAddRequestHeaderField(&context, "User-Agent", "SmileLUA");
	httpcAddRequestHeaderField(&context, "Connection", "Keep-Alive");

	if(httpcBeginRequest(&context) == 0) {
		u32 status_code;
		httpcGetResponseStatusCode(&context, &status_code);

		if(status_code == 200) {
			httpcContext *c = lua_newuserdata(L, sizeof(httpcContext)); //add to stack + return ptr
			*c = context;
			luaL_getmetatable(L, "http_get");
			lua_setmetatable(L, -2);
			return 1;
		}
	}

	httpcCloseContext(&context);
	return 0;
}

void luaextend_io_addHTTP(lua_State *L) {
	luaL_newmetatable(L, "http_get");
		lua_pushstring(L, "__index");
		lua_pushcfunction(L, http_get_index);
		lua_settable(L, -3);

		lua_pushstring(L, "__gc");
		lua_pushcfunction(L, http_close);
		lua_settable(L, -3);

	lua_getglobal(L, "io");
		lua_pushstring(L, "http");
		lua_newtable(L);
			lua_pushstring(L, "get");
			lua_pushcfunction(L, http_get);
			lua_settable(L, -3);
		lua_settable(L, -3);
}
