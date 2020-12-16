#include <3ds.h>

#include <stdio.h>
#include <string.h> //strcmp
#include <stdint.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "extend_lua_io.h"
#include "extend_lua_os.h"
#include "extend_lua_table.h"

#define ROM_ROOT "romfs:/"
#define SD_ROOT "sdmc:/smilelua/"
#define STARTUP "init.lua"

PrintConsole console_top;
PrintConsole console_bottom;

int main() {
	lua_State *L = NULL;

	gfxInitDefault();
	consoleInit(GFX_BOTTOM, &console_bottom);
	consoleInit(GFX_TOP, &console_top);
	//pixel formats set to GSP_RGB565_OES - double buffering disabled

	if(romfsInit()) {
		puts("\x1b[31;1mCan't initialise RomFS\x1b[0m");
	}

	L = luaL_newstate(); // open Lua
	if(L == NULL) return -1;
	luaL_openlibs(L);
	luaextend_io(L); //must come after consoleinit and mustn't change pixel format
	luaextend_os(L);
	luaextend_table(L);

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
		consoleSelect(&console_bottom);
		printf("\x1b[2JPress Start to exit");
		consoleSelect(&console_top);
		printf("\x1b[2J\x1b[31;1mLua Error\x1b[0m\n%s", lua_tostring(L, -1));
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
