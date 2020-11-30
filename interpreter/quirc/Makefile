CC=arm-none-eabi-gcc -std=gnu99
CFLAGS= -O2 -Wall

LIB_OBJ = src/decode.o src/identify.o src/quirc.o src/version_db.o

lib/libquirc.a: $(LIB_OBJ)
	arm-none-eabi-ar cru $@ $(LIB_OBJ)
	arm-none-eabi-ranlib $@

.c.o:
	$(CC) $(CFLAGS) -Iinclude -o $@ -c $<
