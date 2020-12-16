#include "extend_lua_io.h"

extern PrintConsole console_bottom;
static SwkbdState software_keyboard;

static void initKeyboard() {
	swkbdInit(&software_keyboard, SWKBD_TYPE_NORMAL, 2, -1);
	swkbdSetButton(&software_keyboard, SWKBD_BUTTON_LEFT, "Cancel", 0);
	swkbdSetButton(&software_keyboard, SWKBD_BUTTON_RIGHT, "Ok", 1);
	swkbdSetValidation(&software_keyboard, SWKBD_ANYTHING, 0, 2);
	swkbdSetFeatures(&software_keyboard, SWKBD_DARKEN_TOP_SCREEN | SWKBD_ALLOW_RESET | SWKBD_ALLOW_HOME | SWKBD_ALLOW_POWER);
}

static int lua_read(lua_State *L) {
	char buffer[1024] = {0};
	const char *prefilled = luaL_optstring(L, 1, "");

	swkbdSetInitialText(&software_keyboard, prefilled);
	SwkbdButton button = swkbdInputText(&software_keyboard, buffer, sizeof(buffer));

	if(button == SWKBD_BUTTON_RIGHT) {
		lua_pushstring(L, buffer);
		return 1;
	}
	return 0;
}

static int lua_readcontrols(lua_State *L) {
	u32 keys = hidKeysHeld();

	lua_newtable(L);
		lua_pushstring(L, "A"); lua_pushboolean(L, keys & KEY_A); lua_settable(L, -3);
		lua_pushstring(L, "B"); lua_pushboolean(L, keys & KEY_B); lua_settable(L, -3);
		lua_pushstring(L, "X"); lua_pushboolean(L, keys & KEY_X); lua_settable(L, -3);
		lua_pushstring(L, "Y"); lua_pushboolean(L, keys & KEY_Y); lua_settable(L, -3);

		lua_pushstring(L, "L"); lua_pushboolean(L, keys & KEY_L); lua_settable(L, -3);
		lua_pushstring(L, "ZL"); lua_pushboolean(L, keys & KEY_ZL); lua_settable(L, -3);
		lua_pushstring(L, "R"); lua_pushboolean(L, keys & KEY_R); lua_settable(L, -3);
		lua_pushstring(L, "ZR"); lua_pushboolean(L, keys & KEY_ZR); lua_settable(L, -3);

		lua_pushstring(L, "Start"); lua_pushboolean(L, keys & KEY_START); lua_settable(L, -3);
		lua_pushstring(L, "Select"); lua_pushboolean(L, keys & KEY_SELECT); lua_settable(L, -3);

		lua_pushstring(L, "D-pad Up"); lua_pushboolean(L, keys & KEY_DUP); lua_settable(L, -3);
		lua_pushstring(L, "D-pad Left"); lua_pushboolean(L, keys & KEY_DLEFT); lua_settable(L, -3);
		lua_pushstring(L, "D-pad Right"); lua_pushboolean(L, keys & KEY_DRIGHT); lua_settable(L, -3);
		lua_pushstring(L, "D-pad Down"); lua_pushboolean(L, keys & KEY_DDOWN); lua_settable(L, -3);

		//This doesn't work, but has a code in the API
		//lua_pushstring(L, "Touchscreen"); lua_pushboolean(L, keys & KEY_TOUCH); lua_settable(L, -3);

		lua_pushstring(L, "C-Stick Up"); lua_pushboolean(L, keys & KEY_CSTICK_UP); lua_settable(L, -3);
		lua_pushstring(L, "C-Stick Left"); lua_pushboolean(L, keys & KEY_CSTICK_LEFT); lua_settable(L, -3);
		lua_pushstring(L, "C-Stick Right"); lua_pushboolean(L, keys & KEY_CSTICK_RIGHT); lua_settable(L, -3);
		lua_pushstring(L, "C-Stick Down"); lua_pushboolean(L, keys & KEY_CSTICK_DOWN); lua_settable(L, -3);

		lua_pushstring(L, "Circle Pad Up"); lua_pushboolean(L, keys & KEY_CPAD_UP); lua_settable(L, -3);
		lua_pushstring(L, "Circle Pad Left"); lua_pushboolean(L, keys & KEY_CPAD_LEFT); lua_settable(L, -3);
		lua_pushstring(L, "Circle Pad Right"); lua_pushboolean(L, keys & KEY_CPAD_RIGHT); lua_settable(L, -3);
		lua_pushstring(L, "Circle Pad Down"); lua_pushboolean(L, keys & KEY_CPAD_DOWN); lua_settable(L, -3);

		lua_pushstring(L, "Up"); lua_pushboolean(L, keys & KEY_UP); lua_settable(L, -3);
		lua_pushstring(L, "Left"); lua_pushboolean(L, keys & KEY_LEFT); lua_settable(L, -3);
		lua_pushstring(L, "Right"); lua_pushboolean(L, keys & KEY_RIGHT); lua_settable(L, -3);
		lua_pushstring(L, "Down"); lua_pushboolean(L, keys & KEY_DOWN); lua_settable(L, -3);
	return 1;
}

static int lua_readtouchscreen(lua_State *L) {
	touchPosition touch;
	hidTouchRead(&touch);
	if(touch.px || touch.py) {
		lua_pushinteger(L, touch.px);
		lua_pushinteger(L, touch.py);
		return 2;
	}
	return 0;
}

static int lua_readqr(lua_State *L) {
	struct quirc_data *data = qr_read();
	if(data) {
		lua_pushlstring(L, (char *)data->payload, data->payload_len);
		free(data);
		return 1;
	}
	return 0;
}

static int lua_writebottom(lua_State *L) {
	int top = lua_gettop(L);
	PrintConsole *previous_console = consoleSelect(&console_bottom);
	for(int i = 1; i <= top; i++) {
		printf("%s", lua_tostring(L, i));
	}
	consoleSelect(previous_console); //back to default

	//return file obj, like write would
	lua_getglobal(L, "io");
	lua_getfield(L, -1, "stdout");
	return 1;
}

static int lua_http_get(lua_State *L) { //check http.c for more details
	const char *url = luaL_checkstring(L, 1);
	if(strncmp(url, "http", 4) != 0) {
		luaL_error(L, "Not an HTTP URL");
		return 0;
	}

	http_string_t http_ret;

	http_get(url, &http_ret);
	if(http_ret.ptr) {
		lua_pushlstring(L, (char *)http_ret.ptr, http_ret.length);
		free(http_ret.ptr);
		return 1;
	}
	return 0;
}

void luaextend_io(lua_State *L) {
	initKeyboard();

	lua_getglobal(L, "io");
		lua_pushstring(L, "read");
		lua_pushcfunction(L, lua_read);
		lua_settable(L, -3);

		lua_pushstring(L, "readControls");
		lua_pushcfunction(L, lua_readcontrols);
		lua_settable(L, -3);

		lua_pushstring(L, "readTouchscreen");
		lua_pushcfunction(L, lua_readtouchscreen);
		lua_settable(L, -3);

		lua_pushstring(L, "readQR");
		lua_pushcfunction(L, lua_readqr);
		lua_settable(L, -3);

		lua_pushstring(L, "writeBottom");
		lua_pushcfunction(L, lua_writebottom);
		lua_settable(L, -3);

		lua_pushstring(L, "http");
		lua_newtable(L);
			lua_pushstring(L, "get");
			lua_pushcfunction(L, lua_http_get);
			lua_settable(L, -3);
		lua_settable(L, -3);

	luaextend_io_addFramebuffer(L);
}
