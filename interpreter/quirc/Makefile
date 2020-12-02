CC=		arm-none-eabi-gcc -std=gnu99
ARCH=	-march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft
CFLAGS= -Wall -O2 -mword-relocations -fomit-frame-pointer -ffunction-sections $(ARCH)

LIB_OBJ = src/decode.o src/identify.o src/quirc.o src/version_db.o

libquirc.a: $(LIB_OBJ)
	arm-none-eabi-ar cru $@ $(LIB_OBJ)
	arm-none-eabi-ranlib $@

.c.o:
	$(CC) $(CFLAGS) -Iinclude -o $@ -c $<

PHONY: clean

clean:
	rm -f src/*.o
	rm -f libquirc.a
