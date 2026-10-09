CC64 = x86_64-w64-mingw32-gcc
CC86 = i686-w64-mingw32-gcc
STRIP64 = x86_64-w64-mingw32-strip --strip-unneeded
STRIP86 = i686-w64-mingw32-strip --strip-unneeded
CFLAGS = -I ../_include -masm=intel -w -Wno-incompatible-pointer-types -Os -DBOF -c

bof: clean
	@mkdir _bin
	@echo "[+] portscan"
	@$(CC64) $(CFLAGS) src/portscan.c -o _bin/portscan.x64.o && $(STRIP64) _bin/portscan.x64.o
	@$(CC86) $(CFLAGS) src/portscan.c -o _bin/portscan.x32.o && $(STRIP86) _bin/portscan.x32.o

clean:
	@rm -rf _bin
