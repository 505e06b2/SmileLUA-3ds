#include <3ds.h>
#include <stdio.h>
#include <string.h> //strcmp
#include <stdint.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "extend_lua_io.h"
#include "extend_lua_os.h"
#include "lua_time.h"

#define ROMFS "romfs:/"
#define SDMC "sdmc:/"
#define STARTUP "init.lua"

int main() {
	lua_State *L = NULL;
	uint8_t use_romfs = 0;

	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);

	if(romfsInit() == 0) use_romfs = 1;
	chdir(use_romfs ? ROMFS : SDMC);

	L = luaL_newstate(); // open Lua
	if(L == NULL) return -1;
	luaL_openlibs(L);
	luaextend_io(L);
	luaextend_os(L);
	luaopen_time(L);


	printf("Launching: %s%s\n", (use_romfs ? ROMFS : SDMC), "init.lua");
	if(luaL_dofile(L, "init.lua")) {
		printf("Lua Error: %s\n", lua_tostring(L, -1));
		printf("Press Start to exit");
		while(aptMainLoop()) {
			hidScanInput();

			if(hidKeysDown() & KEY_START) break;

			gfxFlushBuffers();
			gfxSwapBuffers();

			gspWaitForVBlank();
		}
	}

	lua_close(L);
	gfxExit();
	return 0;
}
