#include "framebuffer.h"

/*
 * When is comes to using the raw framebuffer,
 * remember that the DS screens are rotated -90 degrees,
 * buffer[0] is the bottom left
 */

//this file should be altered at a later date to optimise code re-use


//https://stackoverflow.com/questions/11689135/share-array-between-lua-and-c

//These will expose the underlying C structure for the framebuffer
static int fb_top_raw_index(lua_State* L) {
	u16** parray = luaL_checkudata(L, 1, "framebuffer_top_raw");
	int index = luaL_checkinteger(L, 2);
	if(index < 1 || index > TOP_SIZE) return 0; //bounds check
	lua_pushnumber(L, (*parray)[index-1]);
	return 1;
}

static int fb_top_raw_newindex(lua_State* L) {
	u16** parray = luaL_checkudata(L, 1, "framebuffer_top_raw");
	int index = luaL_checkinteger(L, 2);
	u16 value = luaL_checkinteger(L, 3);
	if(index < 1 || index > TOP_SIZE) return 0; //bounds check
	(*parray)[index-1] = value;
	return 0;
}

static int fb_bottom_raw_index(lua_State* L) {
	u16** parray = luaL_checkudata(L, 1, "framebuffer_bottom_raw");
	int index = luaL_checkinteger(L, 2);
	if(index < 1 || index > BOTTOM_SIZE) return 0; //bounds check
	lua_pushnumber(L, (*parray)[index-1]);
	return 1;
}

static int fb_bottom_raw_newindex(lua_State* L) {
	u16** parray = luaL_checkudata(L, 1, "framebuffer_bottom_raw");
	int index = luaL_checkinteger(L, 2);
	u16 value = luaL_checkinteger(L, 3);
	if(index < 1 || index > BOTTOM_SIZE) return 0; //bounds check
	(*parray)[index-1] = value;
	return 0;
}

//These will rotate the index by 90degrees so that it will be left, to right, row, then col
//top rotate framebuffer
static int fb_top_index(lua_State* L) {
	u16** parray = luaL_checkudata(L, 1, "framebuffer_top");
	int index = luaL_checkinteger(L, 2);
	if(index < 1 || index > TOP_SIZE) return 0; //bounds check

	index--; //0-index
	int wanted_x = index % TOP_WIDTH;
	int wanted_y = index / TOP_WIDTH;
	lua_pushnumber(L, (*parray)[(TOP_HEIGHT - wanted_y -1) + (wanted_x * TOP_HEIGHT)]);
	return 1;
}

static int fb_top_newindex(lua_State* L) {
	u16** parray = luaL_checkudata(L, 1, "framebuffer_top");
	int index = luaL_checkinteger(L, 2);
	u16 value = luaL_checkinteger(L, 3);
	if(index < 1 || index > TOP_SIZE) return 0; //bounds check

	index--; //0-index
	int wanted_x = index % TOP_WIDTH;
	int wanted_y = index / TOP_WIDTH;
	(*parray)[(TOP_HEIGHT - wanted_y -1) + (wanted_x * TOP_HEIGHT)] = value;
	return 0;
}

//bottom rotated framebuffer
static int fb_bottom_index(lua_State* L) {
	u16** parray = luaL_checkudata(L, 1, "framebuffer_bottom");
	int index = luaL_checkinteger(L, 2);
	if(index < 1 || index > BOTTOM_SIZE) return 0; //bounds check

	index--; //0-index
	int wanted_x = index % BOTTOM_WIDTH;
	int wanted_y = index / BOTTOM_WIDTH;
	lua_pushnumber(L, (*parray)[(BOTTOM_HEIGHT - wanted_y -1) + (wanted_x * BOTTOM_HEIGHT)]);
	return 1;
}

static int fb_bottom_newindex(lua_State* L) {
	u16** parray = luaL_checkudata(L, 1, "framebuffer_bottom");
	int index = luaL_checkinteger(L, 2);
	u16 value = luaL_checkinteger(L, 3);
	if(index < 1 || index > BOTTOM_SIZE) return 0; //bounds check

	index--; //0-index
	int wanted_x = index % BOTTOM_WIDTH;
	int wanted_y = index / BOTTOM_WIDTH;
	(*parray)[(BOTTOM_HEIGHT - wanted_y -1) + (wanted_x * BOTTOM_HEIGHT)] = value;
	return 0;
}


void luaextend_io_addFramebuffer(lua_State *L) {
	u16 **framebuffer_pointer;

	//create types
	luaL_newmetatable(L, "framebuffer_top_raw");
		lua_pushstring(L, "__index");
		lua_pushcfunction(L, fb_top_raw_index);
		lua_settable(L, -3);

		lua_pushstring(L, "__newindex");
		lua_pushcfunction(L, fb_top_raw_newindex);
		lua_settable(L, -3);

		lua_pushstring(L, "__len");
		lua_pushinteger(L, TOP_SIZE);
		lua_settable(L, -3);

	luaL_newmetatable(L, "framebuffer_bottom_raw");
		lua_pushstring(L, "__index");
		lua_pushcfunction(L, fb_bottom_raw_index);
		lua_settable(L, -3);

		lua_pushstring(L, "__newindex");
		lua_pushcfunction(L, fb_bottom_raw_newindex);
		lua_settable(L, -3);

		lua_pushstring(L, "__len");
		lua_pushinteger(L, BOTTOM_SIZE);
		lua_settable(L, -3);

	luaL_newmetatable(L, "framebuffer_top");
		lua_pushstring(L, "__index");
		lua_pushcfunction(L, fb_top_index);
		lua_settable(L, -3);

		lua_pushstring(L, "__newindex");
		lua_pushcfunction(L, fb_top_newindex);
		lua_settable(L, -3);

		lua_pushstring(L, "__len");
		lua_pushinteger(L, TOP_SIZE);
		lua_settable(L, -3);

	luaL_newmetatable(L, "framebuffer_bottom");
		lua_pushstring(L, "__index");
		lua_pushcfunction(L, fb_bottom_index);
		lua_settable(L, -3);

		lua_pushstring(L, "__newindex");
		lua_pushcfunction(L, fb_bottom_newindex);
		lua_settable(L, -3);

		lua_pushstring(L, "__len");
		lua_pushinteger(L, BOTTOM_SIZE);
		lua_settable(L, -3);


	//add to global
	lua_getglobal(L, "io");
		lua_pushstring(L, "framebuffer");
		lua_newtable(L);
			lua_pushstring(L, "topRaw");
			framebuffer_pointer = lua_newuserdata(L, sizeof(u16**)); //add to stack + return ptr
				*framebuffer_pointer = (u16 *)gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
				luaL_getmetatable(L, "framebuffer_top_raw");
				lua_setmetatable(L, -2);
			lua_settable(L, -3);

			lua_pushstring(L, "bottomRaw");
			framebuffer_pointer = lua_newuserdata(L, sizeof(u16**)); //add to stack + return ptr
				*framebuffer_pointer = (u16 *)gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
				luaL_getmetatable(L, "framebuffer_bottom_raw");
				lua_setmetatable(L, -2);
			lua_settable(L, -3);

			lua_pushstring(L, "top");
			framebuffer_pointer = lua_newuserdata(L, sizeof(u16**)); //add to stack + return ptr
				*framebuffer_pointer = (u16 *)gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
				luaL_getmetatable(L, "framebuffer_top");
				lua_setmetatable(L, -2);
			lua_settable(L, -3);

			lua_pushstring(L, "bottom");
			framebuffer_pointer = lua_newuserdata(L, sizeof(u16**)); //add to stack + return ptr
				*framebuffer_pointer = (u16 *)gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
				luaL_getmetatable(L, "framebuffer_bottom");
				lua_setmetatable(L, -2);
			lua_settable(L, -3);

		lua_settable(L, -3);
}
