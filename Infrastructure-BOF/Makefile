CC64 = x86_64-w64-mingw32-gcc
CC86 = i686-w64-mingw32-gcc
STRIP64 = x86_64-w64-mingw32-strip --strip-unneeded
STRIP86 = i686-w64-mingw32-strip --strip-unneeded
CFLAGS = -I ../_include -masm=intel -w -Wno-incompatible-pointer-types -Os -DBOF -c

bof: clean
	@mkdir _bin
	@echo "[+] network_device"
	@$(CC64) $(CFLAGS) src/network_device.c -o _bin/network_device.x64.o && $(STRIP64) _bin/network_device.x64.o
	@$(CC86) $(CFLAGS) src/network_device.c -o _bin/network_device.x32.o && $(STRIP86) _bin/network_device.x32.o

clean:
	@rm -rf _bin
