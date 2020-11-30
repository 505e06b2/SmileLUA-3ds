#include <3ds.h>
#include <citro2d.h>

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

C3D_RenderTarget *top_screen;
PrintConsole console_bottom;
PrintConsole console_top;

int main() {
	lua_State *L = NULL;

	gfxInitDefault();
	consoleInit(GFX_BOTTOM, &console_bottom);
	consoleInit(GFX_TOP, &console_top);

	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	top_screen = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

	if(romfsInit()) {
		puts("\x1b[31;1mCan't initialise RomFS\x1b[0m");
	}

	L = luaL_newstate(); // open Lua
	if(L == NULL) return -1;
	luaL_openlibs(L);
	luaextend_io(L);
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
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	return 0;
}
