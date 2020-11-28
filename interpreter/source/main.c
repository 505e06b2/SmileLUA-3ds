#include <3ds.h>
#include <stdio.h>
#include <string.h> //strcmp
#include <stdint.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "extend_lua_io.h"
#include "extend_lua_os.h"

#define ROM_ROOT "romfs:/"
#define SD_ROOT "sdmc:/smilelua/"
#define STARTUP "init.lua"

int main() {
	lua_State *L = NULL;

	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);

	if(romfsInit()) {
		puts("\x1b[31mCan't initialise RomFS\x1b[0m");
	}

	L = luaL_newstate(); // open Lua
	if(L == NULL) return -1;
	luaL_openlibs(L);
	luaextend_io(L);
	luaextend_os(L);

	{
		char buffer[1024] = {0};
		FILE *f = NULL;

		if((f = fopen(SD_ROOT STARTUP, "r"))) { //sd file exists
			fclose(f);
			chdir(SD_ROOT);
		} else {
			chdir(ROM_ROOT);
		}

		getcwd(buffer, sizeof(buffer));
		printf("Attempting to launch: %s%s\n", buffer, STARTUP);
	}

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
