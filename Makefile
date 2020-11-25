TARGET	:=	smilelua
LUA 	:=	interpreter/lua
ROMFS	:=	romfs

.PHONY: all run 3dslink clean $(TARGET) $(LUA)

all: $(LUA) $(TARGET)

run: all
	citra $(TARGET).3dsx

3dslink: all
	3dslink -a 192.168.0.26 $(TARGET).3dsx

clean:
	#@$(MAKE) --no-print-directory -C $(LUA) clean
	@$(MAKE) --no-print-directory -C interpreter clean

$(LUA):
	@$(MAKE) --no-print-directory -C $(LUA) -j5

$(TARGET):
	#force re-link so that romfs can be added
	@rm -f $(TARGET).3dsx $(TARGET).elf
	@$(MAKE) --no-print-directory -C interpreter -j5
