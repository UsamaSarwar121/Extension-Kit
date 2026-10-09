#include <windows.h>
#include "beacon.h"

DECLSPEC_IMPORT HANDLE WINAPI KERNEL32$GetCurrentProcess();
DECLSPEC_IMPORT BOOL WINAPI KERNEL32$OpenProcessToken(HANDLE, DWORD, PHANDLE);
DECLSPEC_IMPORT BOOL WINAPI KERNEL32$GetTokenInformation(HANDLE, TOKEN_INFORMATION_CLASS, LPVOID, DWORD, PDWORD);
DECLSPEC_IMPORT BOOL WINAPI KERNEL32$LookupPrivilegeValueA(LPCSTR, LPCSTR, PLUID);
DECLSPEC_IMPORT BOOL WINAPI KERNEL32$CloseHandle(HANDLE);

DECLSPEC_IMPORT LRESULT WINAPI USER32$wsprintfA(char*, const char*, ...);

void go(char *args, int len) {
    datap parser;
    BeaconDataParse(&parser, args, len);
    
    HANDLE hToken;
    if (!KERNEL32$OpenProcessToken(KERNEL32$GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        BeaconPrintf(CALLBACK_ERROR, "[-] OpenProcessToken failed");
        return;
    }
    
    DWORD needed = 0;
    KERNEL32$GetTokenInformation(hToken, TokenPrivileges, NULL, 0, &needed);
    
    char buf[4096];
    if (needed > 4096) needed = 4096;
    
    if (!KERNEL32$GetTokenInformation(hToken, TokenPrivileges, buf, needed, &needed)) {
        BeaconPrintf(CALLBACK_ERROR, "[-] GetTokenInformation failed");
        KERNEL32$CloseHandle(hToken);
        return;
    }
    
    PTOKEN_PRIVILEGES tp = (PTOKEN_PRIVILEGES)buf;
    BeaconPrintf(CALLBACK_OUTPUT, "=== Token Privileges ===");
    BeaconPrintf(CALLBACK_OUTPUT, "Privilege Count: %lu", tp->PrivilegeCount);
    
    char *names[] = {
        "SeDebugPrivilege", "SeImpersonatePrivilege", "SeAssignPrimaryTokenPrivilege",
        "SeTcbPrivilege", "SeBackupPrivilege", "SeRestorePrivilege",
        "SeTakeOwnershipPrivilege", "SeLoadDriverPrivilege", "SeCreateTokenPrivilege",
        "SeCreateGlobalPrivilege", "SeAssignPrimaryTokenPrivilege", "SeLockMemoryPrivilege",
        "SeIncreaseQuotaPrivilege", "SeManageVolumePrivilege"
    };
    int nameCount = sizeof(names) / sizeof(names[0]);
    
    for (DWORD i = 0; i < tp->PrivilegeCount && i < (DWORD)nameCount; i++) {
        LUID luid = tp->Privileges[i].Luid;
        char privName[256] = {0};
        
        if (KERNEL32$LookupPrivilegeValueA(NULL, names[i], &luid)) {
            char *status = (tp->Privileges[i].Attributes & SE_PRIVILEGE_ENABLED) ? "ENABLED" : "disabled";
            BeaconPrintf(CALLBACK_OUTPUT, "  [%s] %s", status, names[i]);
        }
    }
    
    KERNEL32$CloseHandle(hToken);
}
