#include <windows.h>
#include "beacon.h"

DECLSPEC_IMPORT int WINAPI WS2_32$WSAStartup(WORD, LPWSADATA);
DECLSPEC_IMPORT SOCKET WINAPI WS2_32$socket(int, int, int);
DECLSPEC_IMPORT int WINAPI WS2_32$connect(SOCKET, const struct sockaddr*, int);
DECLSPEC_IMPORT int WINAPI WS2_32$closesocket(SOCKET);
DECLSPEC_IMPORT int WINAPI WS2_32$WSAGetLastError();
DECLSPEC_IMPORT unsigned long WINAPI WS2_32$inet_addr(const char*);
DECLSPEC_IMPORT int WINAPI WS2_32$setsockopt(SOCKET, int, int, const char*, int);
DECLSPEC_IMPORT unsigned __int64 WINAPI KERNEL32$GetTickCount64();
DECLSPEC_IMPORT int WINAPI WS2_32$WSACleanup(void);

#define AF_INET 2
#define SOCK_STREAM 1
#define SOL_SOCKET 0x0000FFFF
#define SO_RCVTIMEO 0x1006
#define INVALID_SOCKET (~0)

void go(char *args, int len) {
    datap parser;
    BeaconDataParse(&parser, args, len);
    
    int mode = BeaconDataInt(&parser);
    char *target = BeaconDataExtract(&parser, NULL);
    int port = BeaconDataInt(&parser);
    int timeout = BeaconDataInt(&parser);
    
    if (!target) {
        BeaconPrintf(CALLBACK_ERROR, "[-] Target required");
        return;
    }
    if (port == 0) port = 80;
    if (timeout == 0) timeout = 2000;
    
    WSADATA wsa;
    if (WS2_32$WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        BeaconPrintf(CALLBACK_ERROR, "[-] WSAStartup failed");
        return;
    }
    
    if (mode == 0) {
        SOCKET sock = WS2_32$socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET) {
            BeaconPrintf(CALLBACK_ERROR, "[-] socket() failed");
            return;
        }
        
        DWORD timeoutMs = timeout;
        WS2_32$setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeoutMs, sizeof(timeoutMs));
        
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = __builtin_bswap16(port);
        addr.sin_addr.s_addr = WS2_32$inet_addr(target);
        
        unsigned __int64 start = KERNEL32$GetTickCount64();
        int result = WS2_32$connect(sock, (struct sockaddr*)&addr, sizeof(addr));
        unsigned __int64 elapsed = KERNEL32$GetTickCount64() - start;
        
        if (result == 0) {
            BeaconPrintf(CALLBACK_OUTPUT, "[+] %s:%d OPEN (%llums)", target, port, elapsed);
        } else {
            BeaconPrintf(CALLBACK_OUTPUT, "[-] %s:%d CLOSED/FILTERED (%llums)", target, port, elapsed);
        }
        
        WS2_32$closesocket(sock);
    }
    else if (mode == 1) {
        int endPort = BeaconDataInt(&parser);
        if (endPort == 0) endPort = port + 100;
        if (endPort > 65535) endPort = 65535;
        
        BeaconPrintf(CALLBACK_OUTPUT, "=== Port Scan: %s (%d-%d) ===", target, port, endPort);
        
        int openPorts = 0;
        for (int p = port; p <= endPort; p++) {
            SOCKET sock = WS2_32$socket(AF_INET, SOCK_STREAM, 0);
            if (sock == INVALID_SOCKET) continue;
            
            DWORD to = timeout;
            WS2_32$setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&to, sizeof(to));
            
            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = __builtin_bswap16(p);
            addr.sin_addr.s_addr = WS2_32$inet_addr(target);
            
            if (WS2_32$connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
                BeaconPrintf(CALLBACK_OUTPUT, "  [+] Port %d OPEN", p);
                openPorts++;
            }
            
            WS2_32$closesocket(sock);
        }
        
        BeaconPrintf(CALLBACK_OUTPUT, "=== Scan complete: %d open ports found ===", openPorts);
    }
    
    WS2_32$WSACleanup();
}
