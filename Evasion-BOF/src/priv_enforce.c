#include <windows.h>
#include "beacon.h"

DECLSPEC_IMPORT HANDLE WINAPI KERNEL32$GetCurrentProcess();
DECLSPEC_IMPORT BOOL WINAPI KERNEL32$OpenProcessToken(HANDLE, DWORD, PHANDLE);
DECLSPEC_IMPORT BOOL WINAPI KERNEL32$LookupPrivilegeValueA(LPCSTR, LPCSTR, PLUID);
DECLSPEC_IMPORT BOOL WINAPI KERNEL32$AdjustTokenPrivileges(HANDLE, BOOL, PTOKEN_PRIVILEGES, DWORD, PTOKEN_PRIVILEGES, PDWORD);
DECLSPEC_IMPORT DWORD WINAPI KERNEL32$GetLastError();
DECLSPEC_IMPORT BOOL WINAPI KERNEL32$CloseHandle(HANDLE);

void go(char *args, int len) {
    datap parser;
    BeaconDataParse(&parser, args, len);
    
    HANDLE hToken;
    if (!KERNEL32$OpenProcessToken(KERNEL32$GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        BeaconPrintf(CALLBACK_ERROR, "[-] OpenProcessToken failed: %d", KERNEL32$GetLastError());
        return;
    }
    
    TOKEN_PRIVILEGES tp;
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    
    char *privs[] = {
        "SeDebugPrivilege",
        "SeImpersonatePrivilege",
        "SeAssignPrimaryTokenPrivilege",
        "SeTcbPrivilege",
        "SeBackupPrivilege",
        "SeRestorePrivilege",
        "SeTakeOwnershipPrivilege",
        "SeLoadDriverPrivilege",
        "SeCreateTokenPrivilege",
        "SeCreateGlobalPrivilege"
    };
    
    int count = sizeof(privs) / sizeof(privs[0]);
    int enabled = 0;
    
    for (int i = 0; i < count; i++) {
        if (KERNEL32$LookupPrivilegeValueA(NULL, privs[i], &tp.Privileges[0].Luid)) {
            if (KERNEL32$AdjustTokenPrivileges(hToken, FALSE, &tp, 0, NULL, NULL)) {
                DWORD err = KERNEL32$GetLastError();
                if (err == 0) {
                    BeaconPrintf(CALLBACK_OUTPUT, "[+] Enabled: %s", privs[i]);
                    enabled++;
                } else {
                    BeaconPrintf(CALLBACK_OUTPUT, "[-] Failed: %s (error %d)", privs[i], err);
                }
            }
        }
    }
    
    BeaconPrintf(CALLBACK_OUTPUT, "[+] %d/%d privileges enabled", enabled, count);
    KERNEL32$CloseHandle(hToken);
}
