CC64 = x86_64-w64-mingw32-gcc
CC86 = i686-w64-mingw32-gcc
STRIP64 = x86_64-w64-mingw32-strip --strip-unneeded
STRIP86 = i686-w64-mingw32-strip --strip-unneeded
CFLAGS = -I ../_include -masm=intel -w -Wno-incompatible-pointer-types -Os -DBOF -c

bof: clean
	@mkdir _bin
	@echo "[+] mobile"
	@$(CC64) $(CFLAGS) src/mobile.c -o _bin/mobile.x64.o && $(STRIP64) _bin/mobile.x64.o
	@$(CC86) $(CFLAGS) src/mobile.c -o _bin/mobile.x32.o && $(STRIP86) _bin/mobile.x32.o

clean:
	@rm -rf _bin
