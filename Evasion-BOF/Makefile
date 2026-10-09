CC64 = x86_64-w64-mingw32-gcc
CC86 = i686-w64-mingw32-gcc
STRIP64 = x86_64-w64-mingw32-strip --strip-unneeded
STRIP86 = i686-w64-mingw32-strip --strip-unneeded
CFLAGS = -I ../_include -masm=intel -w -Wno-incompatible-pointer-types -Os -DBOF -c

bof: clean
	@mkdir _bin
	@echo "[+] amsi_etw"
	@$(CC64) $(CFLAGS) src/amsi_etw.c -o _bin/amsi_etw.x64.o && $(STRIP64) _bin/amsi_etw.x64.o
	@$(CC86) $(CFLAGS) src/amsi_etw.c -o _bin/amsi_etw.x32.o && $(STRIP86) _bin/amsi_etw.x32.o
	@echo "[+] priv_enforce"
	@$(CC64) $(CFLAGS) src/priv_enforce.c -o _bin/priv_enforce.x64.o && $(STRIP64) _bin/priv_enforce.x64.o
	@$(CC86) $(CFLAGS) src/priv_enforce.c -o _bin/priv_enforce.x32.o && $(STRIP86) _bin/priv_enforce.x32.o
	@echo "[+] token_priv"
	@$(CC64) $(CFLAGS) src/token_priv.c -o _bin/token_priv.x64.o && $(STRIP64) _bin/token_priv.x64.o
	@$(CC86) $(CFLAGS) src/token_priv.c -o _bin/token_priv.x32.o && $(STRIP86) _bin/token_priv.x32.o

clean:
	@rm -rf _bin
