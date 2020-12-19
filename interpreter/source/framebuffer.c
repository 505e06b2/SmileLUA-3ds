#include "framebuffer.h"

/*
 * When is comes to using the raw framebuffer,
 * remember that the DS screens are rotated -90 degrees,
 * buffer[0] is the bottom left
 */

const int SCREEN_DATA[][3] = {
	//w   h    size
	{400, 240, 400*240}, //top
	{320, 240, 320*240} //bottom
};

typedef enum {
	SCREEN_TOP = 0,
	SCREEN_BOTTOM = 1,

	SCREEN_WIDTH = 0,
	SCREEN_HEIGHT = 1,
	SCREEN_SIZE = 2,
} screen_data_types;

typedef enum {
	FB_RAW,
	FB_ROTATED,
	FB_SMALL,
} framebuffer_types;

//https://stackoverflow.com/questions/11689135/share-array-between-lua-and-c


//having this file structed the way it is, saved a lot of pollution / repeating code
static int fb_index(lua_State* L) {
	framebuffer_types type = luaL_checkinteger(L, lua_upvalueindex(1));
	screen_data_types screen = luaL_checkinteger(L, lua_upvalueindex(2));

	u16** array = lua_touserdata(L, 1);
	int index = luaL_checkinteger(L, 2);
	if(index < 1 || index > SCREEN_DATA[screen][SCREEN_SIZE]) return 0; //bounds check

	index--; //0-index
	switch(type) {
		case FB_ROTATED: {
				const int height = SCREEN_DATA[screen][SCREEN_HEIGHT];
				const int x = index % SCREEN_DATA[screen][SCREEN_WIDTH];
				const int y = index / SCREEN_DATA[screen][SCREEN_WIDTH];
				lua_pushnumber(L, (*array)[(height - y -1) + (x * height)]);
			}
			break;

		default: //raw
			lua_pushnumber(L, (*array)[index]);
	}
	return 1;
}

static int fb_newindex(lua_State* L) {
	framebuffer_types type = luaL_checkinteger(L, lua_upvalueindex(1));
	screen_data_types screen = luaL_checkinteger(L, lua_upvalueindex(2));

	u16** array = lua_touserdata(L, 1);
	int index = luaL_checkinteger(L, 2);
	u16 value = luaL_checkinteger(L, 3);
	if(index < 1 || index > SCREEN_DATA[screen][SCREEN_SIZE]) return 0; //bounds check

	index--; //0-index
	switch(type) {
		case FB_ROTATED: {
				const int height = SCREEN_DATA[screen][SCREEN_HEIGHT];
				const int x = index % SCREEN_DATA[screen][SCREEN_WIDTH];
				const int y = index / SCREEN_DATA[screen][SCREEN_WIDTH];
				(*array)[(height - y -1) + (x * height)] = value;
			}
			break;

		default: //raw
			(*array)[index] = value;
	}
	return 0;
}

static int fb_len(lua_State* L) {
	screen_data_types screen = luaL_checkinteger(L, lua_upvalueindex(1));
	lua_pushinteger(L, SCREEN_DATA[screen][SCREEN_SIZE]);
	return 1;
}

static void add_metatable_to_lua(lua_State *L, const char *name, screen_data_types screen, framebuffer_types type) {
	luaL_newmetatable(L, name);
		lua_pushstring(L, "__index");
		lua_pushinteger(L, type);
		lua_pushinteger(L, screen);
		lua_pushcclosure(L, fb_index, 2);
		lua_settable(L, -3);

		lua_pushstring(L, "__newindex");
		lua_pushinteger(L, type);
		lua_pushinteger(L, screen);
		lua_pushcclosure(L, fb_newindex, 2);
		lua_settable(L, -3);

		lua_pushstring(L, "__len"); //this must be a function for "#" to work
		lua_pushinteger(L, screen);
		lua_pushcclosure(L, fb_len, 1);
		lua_settable(L, -3);
}

static void add_framebuffer_type_to_lua(lua_State *L, const char *name, const char *metatable) {
	lua_pushstring(L, name);
	u16 **framebuffer_pointer = lua_newuserdata(L, sizeof(u16**)); //add to stack + return ptr
		*framebuffer_pointer = (u16 *)gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
		luaL_getmetatable(L, metatable);
		lua_setmetatable(L, -2);
	lua_settable(L, -3);
}

void luaextend_io_addFramebuffer(lua_State *L) {
	add_metatable_to_lua(L, "framebuffer_top", SCREEN_TOP, FB_ROTATED);
	add_metatable_to_lua(L, "framebuffer_bottom", SCREEN_BOTTOM, FB_ROTATED);

	add_metatable_to_lua(L, "framebuffer_top_raw", SCREEN_TOP, FB_RAW);
	add_metatable_to_lua(L, "framebuffer_bottom_raw", SCREEN_BOTTOM, FB_RAW);

	//add to global
	lua_getglobal(L, "io");
		lua_pushstring(L, "framebuffer");
		lua_newtable(L);
			add_framebuffer_type_to_lua(L, "topRaw", "framebuffer_top_raw"); //turn this into io.framebuffer.top.raw in the future with __index
			add_framebuffer_type_to_lua(L, "bottomRaw", "framebuffer_bottom_raw");

			add_framebuffer_type_to_lua(L, "top", "framebuffer_top");
			add_framebuffer_type_to_lua(L, "bottom", "framebuffer_bottom");
		lua_settable(L, -3);
}
