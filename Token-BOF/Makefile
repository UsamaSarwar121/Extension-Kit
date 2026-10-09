CC64 = x86_64-w64-mingw32-gcc
CC86 = i686-w64-mingw32-gcc
STRIP64 = x86_64-w64-mingw32-strip --strip-unneeded
STRIP86 = i686-w64-mingw32-strip --strip-unneeded
CFLAGS = -I ../_include -masm=intel -w -Wno-incompatible-pointer-types -Os -DBOF -c

bof: clean
	@mkdir _bin
	@echo "[+] token"
	@$(CC64) $(CFLAGS) src/token.c -o _bin/token.x64.o && $(STRIP64) _bin/token.x64.o
	@$(CC86) $(CFLAGS) src/token.c -o _bin/token.x32.o && $(STRIP86) _bin/token.x32.o

clean:
	@rm -rf _bin
