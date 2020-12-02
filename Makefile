TARGET	:=	smilelua
LUA 	:=	interpreter/lua
QUIRC	:=	interpreter/quirc
ROMFS	:=	romfs

.PHONY: all run 3dslink cia hostcia clean $(TARGET) $(LUA) $(QUIRC)

all: $(TARGET)

run: all
	@killall -s SIGKILL -n 0 citra-qt && echo "Killed existing Citra" || citra $(TARGET).3dsx

3dslink: all
	3dslink -a 192.168.0.26 $(TARGET).3dsx

cia: all
	bannertool makebanner -i banner.png -a banner.wav -o $(TARGET).bnr
	bannertool makesmdh -s "SmileLUA" -l "Lua interpreter" -p "505e06b2" -i icon.png  -o $(TARGET).icn
	makerom -f cia -o $(TARGET).cia -DAPP_ENCRYPTED=false -rsf manifest.rsf -target t -exefslogo -elf $(TARGET).elf -icon $(TARGET).icn -banner $(TARGET).bnr

hostcia: cia
	python3 -m http.server

clean:
	@$(MAKE) --no-print-directory -C interpreter clean
	@$(MAKE) --no-print-directory -C $(LUA) clean
	@$(MAKE) --no-print-directory -C $(QUIRC) clean

$(LUA):
	@$(MAKE) --no-print-directory -C $(LUA) -j5

$(QUIRC):
	@$(MAKE) --no-print-directory -C $(QUIRC) -j5

$(TARGET): $(LUA) $(QUIRC)
	#force re-link so that romfs can be added
	@rm -f $(TARGET).3dsx $(TARGET).elf
	@$(MAKE) --no-print-directory -C interpreter -j5

