#include "extend_lua_io.h"

static SwkbdState software_keyboard;
static char keyboard_buffer[1024];

char *openKeyboard() {
	getcwd(keyboard_buffer, sizeof(keyboard_buffer));
	swkbdSetHintText(&software_keyboard, keyboard_buffer);
	SwkbdButton button = swkbdInputText(&software_keyboard, keyboard_buffer, sizeof(keyboard_buffer));
	if(button == SWKBD_BUTTON_RIGHT) return keyboard_buffer;
	return NULL;
}

static int lua_openkeyboard(lua_State *L) {
	const char *ret = openKeyboard();
	if(ret) {
		lua_pushstring(L, ret);
		return 1;
	}
	return 0;
}

void luaextend_io(lua_State *L) {
	swkbdInit(&software_keyboard, SWKBD_TYPE_NORMAL, 2, -1);
	swkbdSetButton(&software_keyboard, SWKBD_BUTTON_LEFT, "Cancel", 0);
	swkbdSetButton(&software_keyboard, SWKBD_BUTTON_RIGHT, "Ok", 1);
	swkbdSetValidation(&software_keyboard, SWKBD_ANYTHING, 0, 2);
	swkbdSetFeatures(&software_keyboard, SWKBD_DARKEN_TOP_SCREEN | SWKBD_ALLOW_HOME | SWKBD_ALLOW_POWER);

	lua_getglobal(L, "io");
		lua_pushstring(L, "read");
		lua_pushcfunction(L, lua_openkeyboard);
		lua_settable(L, -3);
}
