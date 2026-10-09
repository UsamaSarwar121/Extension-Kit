CC64 = x86_64-w64-mingw32-gcc
CC86 = i686-w64-mingw32-gcc
STRIP64 = x86_64-w64-mingw32-strip --strip-unneeded
STRIP86 = i686-w64-mingw32-strip --strip-unneeded
CFLAGS = -I ../_include -masm=intel -w -Wno-incompatible-pointer-types -Os -DBOF -c

bof: clean
	@mkdir _bin
	@echo "[+] metadata"
	@$(CC64) $(CFLAGS) src/metadata.c -o _bin/metadata.x64.o -lwinhttp && $(STRIP64) _bin/metadata.x64.o
	@$(CC86) $(CFLAGS) src/metadata.c -o _bin/metadata.x32.o -lwinhttp && $(STRIP86) _bin/metadata.x32.o
	@echo "[+] credentials"
	@$(CC64) $(CFLAGS) src/credentials.c -o _bin/credentials.x64.o && $(STRIP64) _bin/credentials.x64.o
	@$(CC86) $(CFLAGS) src/credentials.c -o _bin/credentials.x32.o && $(STRIP86) _bin/credentials.x32.o

clean:
	@rm -rf _bin
