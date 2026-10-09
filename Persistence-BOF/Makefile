CC64 = x86_64-w64-mingw32-gcc
CC86 = i686-w64-mingw32-gcc
STRIP64 = x86_64-w64-mingw32-strip --strip-unneeded
STRIP86 = i686-w64-mingw32-strip --strip-unneeded
CFLAGS = -I ../_include -masm=intel -w -Wno-incompatible-pointer-types -Os -DBOF -c

bof: clean
	@mkdir _bin
	@echo "[+] registry"
	@$(CC64) $(CFLAGS) src/registry.c -o _bin/registry.x64.o && $(STRIP64) _bin/registry.x64.o
	@$(CC86) $(CFLAGS) src/registry.c -o _bin/registry.x32.o && $(STRIP86) _bin/registry.x32.o
	@echo "[+] service"
	@$(CC64) $(CFLAGS) src/service.c -o _bin/service.x64.o && $(STRIP64) _bin/service.x64.o
	@$(CC86) $(CFLAGS) src/service.c -o _bin/service.x32.o && $(STRIP86) _bin/service.x32.o
	@echo "[+] task"
	@$(CC64) $(CFLAGS) src/task.c -o _bin/task.x64.o && $(STRIP64) _bin/task.x64.o
	@$(CC86) $(CFLAGS) src/task.c -o _bin/task.x32.o && $(STRIP86) _bin/task.x32.o

clean:
	@rm -rf _bin
