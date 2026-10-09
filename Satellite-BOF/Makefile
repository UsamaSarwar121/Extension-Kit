CC64 = x86_64-w64-mingw32-gcc
CC86 = i686-w64-mingw32-gcc
STRIP64 = x86_64-w64-mingw32-strip --strip-unneeded
STRIP86 = i686-w64-mingw32-strip --strip-unneeded
CFLAGS = -I ../_include -masm=intel -w -Wno-incompatible-pointer-types -Os -DBOF -c

bof: clean
	@mkdir _bin
	@echo "[+] satellite"
	@$(CC64) $(CFLAGS) src/satellite.c -o _bin/satellite.x64.o -lwinhttp && $(STRIP64) _bin/satellite.x64.o
	@$(CC86) $(CFLAGS) src/satellite.c -o _bin/satellite.x32.o -lwinhttp && $(STRIP86) _bin/satellite.x32.o

clean:
	@rm -rf _bin
