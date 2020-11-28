print("\x1b[2J") --clear screen
os.chdir("sdmc:/")
dofile("romfs:/bin/shell.lua")
