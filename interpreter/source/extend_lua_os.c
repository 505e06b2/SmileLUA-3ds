#include "extend_lua_os.h"

extern C3D_RenderTarget *top_screen;
static unsigned int clearConsoleOutput = 0;

static int lua_consoleloop(lua_State *L) { //for software rendering (hopefully just console)
	int ret = aptMainLoop();
	if(ret) {
		if(clearConsoleOutput) { //set if renderloop runs
			printf("\x1b[2J");
			clearConsoleOutput = 0;
		}
		gfxFlushBuffers();
		gfxSwapBuffers();
		gspWaitForVBlank();

		hidScanInput();
	}
	lua_pushboolean(L, ret);
	return 1;
}

static int lua_renderloop(lua_State *L) { //for hardware rendering
	int ret = aptMainLoop();
	if(ret) {
		clearConsoleOutput = 1;
		C3D_FrameEnd(0);

		hidScanInput();
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(top_screen, C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF));
		C2D_SceneBegin(top_screen);
	}
	lua_pushboolean(L, ret);
	return 1;
}

static int lua_sleep(lua_State *L) {
	lua_Number seconds = luaL_checknumber(L, 1);

	struct timespec ts;
    ts.tv_sec = seconds;
    ts.tv_nsec = (seconds-ts.tv_sec) * 1000000000; //get decimal part, then times
    nanosleep(&ts, NULL);
	return 0;
}

static int lua_getcwd(lua_State *L) {
	char buffer[1024] = {0};
	lua_pushstring(L, getcwd(buffer, sizeof(buffer)));
	return 1;
}


static int lua_mkdir(lua_State *L) {
	const char *path = luaL_checkstring(L, 1);
	lua_pushboolean(L, (mkdir(path, 0007777) == 0)); //file perms shouldn't even work?
	return 1;
}

static int lua_rmdir(lua_State *L) {
	const char *path = luaL_checkstring(L, 1);
	lua_pushboolean(L, (rmdir(path) == 0));
	return 1;
}

static int lua_listdir(lua_State *L) {
	const char *path = luaL_checkstring(L, 1);
	DIR *dp;
	struct dirent *ep;

	if((dp = opendir(path))) {
		size_t i = 1;
		lua_newtable(L);
		while((ep = readdir(dp))) {
			lua_pushinteger(L, i++);
			lua_pushstring(L, ep->d_name);
			lua_settable(L, -3);
		}
		closedir(dp);
		return 1;
	}

	return 0;
}

static int lua_chdir(lua_State *L) {
	const char *path = luaL_checkstring(L, 1);
	lua_pushboolean(L, (chdir(path) == 0));
	return 1;
}

static int lua_isdir(lua_State *L) {
	const char *path = luaL_checkstring(L, 1);
	DIR *dp;
	if((dp = opendir(path))) {
		lua_pushboolean(L, 1);
		closedir(dp);
	} else {
		lua_pushboolean(L, 0);
	}
	return 1;
}

static int lua_isfile(lua_State *L) {
	const char *path = luaL_checkstring(L, 1);
	FILE *f = fopen(path, "r");
	if(f) {
		lua_pushboolean(L, 1);
		fclose(f);
	} else {
		lua_pushboolean(L, 0);
	}
	return 1;
}

static int lua_storageInfo(lua_State *L) {
	struct statvfs buffer;
	if(statvfs("sdmc:/", &buffer)) return 0; //failure - at some point give error in 2nd result (ret, error)

	lua_newtable(L);
		lua_pushstring(L, "total");
		lua_pushinteger(L, buffer.f_blocks * buffer.f_bsize);
		lua_settable(L, -3);

		lua_pushstring(L, "free");
		lua_pushinteger(L, buffer.f_bavail * buffer.f_bsize);
		lua_settable(L, -3);
	return 1;
}

void luaextend_os(lua_State *L) {
	lua_getglobal(L, "os");
		lua_pushstring(L, "consoleLoop");
		lua_pushcfunction(L, lua_consoleloop);
		lua_settable(L, -3);

		lua_pushstring(L, "renderLoop");
		lua_pushcfunction(L, lua_renderloop);
		lua_settable(L, -3);

		lua_pushstring(L, "sleep");
		lua_pushcfunction(L, lua_sleep);
		lua_settable(L, -3);

		lua_pushstring(L, "getcwd");
		lua_pushcfunction(L, lua_getcwd);
		lua_settable(L, -3);

		lua_pushstring(L, "mkdir");
		lua_pushcfunction(L, lua_mkdir);
		lua_settable(L, -3);

		lua_pushstring(L, "rmdir");
		lua_pushcfunction(L, lua_rmdir);
		lua_settable(L, -3);

		lua_pushstring(L, "listdir");
		lua_pushcfunction(L, lua_listdir);
		lua_settable(L, -3);

		lua_pushstring(L, "chdir");
		lua_pushcfunction(L, lua_chdir);
		lua_settable(L, -3);

		lua_pushstring(L, "isdir");
		lua_pushcfunction(L, lua_isdir);
		lua_settable(L, -3);

		lua_pushstring(L, "isfile");
		lua_pushcfunction(L, lua_isfile);
		lua_settable(L, -3);

		lua_pushstring(L, "storageInfo");
		lua_pushcfunction(L, lua_storageInfo);
		lua_settable(L, -3);
}
