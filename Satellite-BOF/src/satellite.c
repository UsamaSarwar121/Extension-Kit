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
DECLSPEC_IMPORT size_t WINAPI MSVCRT$strlen(const char*);

#define WINHTTP_FLAG_SECURE 0x00800000
#define WINHTTP_OPTION_SECURITY_FLAGS 31

void go(char *args, int len) {
    datap parser;
    BeaconDataParse(&parser, args, len);
    
    int mode = BeaconDataInt(&parser);
    char *target = BeaconDataExtract(&parser, NULL);
    
    if (!target) target = "192.168.1.1";
    
    HINTERNET hSession = WINHTTP$WinHttpOpen(L"Mozilla/5.0", 0, NULL, NULL, 0);
    if (!hSession) {
        BeaconPrintf(CALLBACK_ERROR, "[-] WinHttpOpen failed: %d", KERNEL32$GetLastError());
        return;
    }
    
    DWORD secFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID | 
                     SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE;
    WINHTTP$WinHttpSetOption(hSession, WINHTTP_OPTION_SECURITY_FLAGS, &secFlags, sizeof(secFlags));
    
    if (mode == 0) {
        // VSAT modem web interface check
        HINTERNET hConnect = WINHTTP$WinHttpConnect(hSession, L"192.168.100.1", 80, 0);
        if (hConnect) {
            HINTERNET hRequest = WINHTTP$WinHttpOpenRequest(hConnect, L"GET", L"/", NULL, WINHTTP_NO_REFERER, NULL, 0);
            if (hRequest) {
                if (WINHTTP$WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
                    WINHTTP$WinHttpReceiveResponse(hRequest, NULL)) {
                    char buffer[4096] = {0};
                    DWORD bytesRead = 0;
                    WINHTTP$WinHttpReadData(hRequest, buffer, sizeof(buffer) - 1, &bytesRead);
                    BeaconPrintf(CALLBACK_OUTPUT, "=== VSAT Modem Interface ===");
                    BeaconPrintf(CALLBACK_OUTPUT, "[+] Response from 192.168.100.1:");
                    BeaconPrintf(CALLBACK_OUTPUT, "%s", buffer);
                }
                WINHTTP$WinHttpCloseHandle(hRequest);
            }
            WINHTTP$WinHttpCloseHandle(hConnect);
        }
        
        // Try alternative VSAT IPs
        char *vsat_ips[] = {"192.168.0.1", "192.168.1.254", "10.0.0.1"};
        for (int i = 0; i < 3; i++) {
            // Convert to wide string
            wchar_t wip[64] = {0};
            for (int j = 0; j < 64 && vsat_ips[i][j]; j++) wip[j] = (wchar_t)vsat_ips[i][j];
            
            hConnect = WINHTTP$WinHttpConnect(hSession, wip, 80, 0);
            if (hConnect) {
                HINTERNET hRequest = WINHTTP$WinHttpOpenRequest(hConnect, L"GET", L"/", NULL, WINHTTP_NO_REFERER, NULL, 0);
                if (hRequest) {
                    if (WINHTTP$WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
                        WINHTTP$WinHttpReceiveResponse(hRequest, NULL)) {
                        char buffer[4096] = {0};
                        DWORD bytesRead = 0;
                        WINHTTP$WinHttpReadData(hRequest, buffer, sizeof(buffer) - 1, &bytesRead);
                        BeaconPrintf(CALLBACK_OUTPUT, "[+] Response from %s:", vsat_ips[i]);
                        BeaconPrintf(CALLBACK_OUTPUT, "  %s", buffer);
                    }
                    WINHTTP$WinHttpCloseHandle(hRequest);
                }
                WINHTTP$WinHttpCloseHandle(hConnect);
            }
        }
    }
    else if (mode == 1) {
        // Network device detection
        BeaconPrintf(CALLBACK_OUTPUT, "=== Network Infrastructure ===");
        BeaconPrintf(CALLBACK_OUTPUT, "[*] Common VSAT/Satellite endpoints:");
        BeaconPrintf(CALLBACK_OUTPUT, "  192.168.100.1 - Hughes/Winegard VSAT modem");
        BeaconPrintf(CALLBACK_OUTPUT, "  192.168.100.254 - iDirect modem");
        BeaconPrintf(CALLBACK_OUTPUT, "  192.168.0.1 - Comtech/General Dynamics");
        BeaconPrintf(CALLBACK_OUTPUT, "  10.0.0.1 - Viasat/LinkStar");
        BeaconPrintf(CALLBACK_OUTPUT, "  172.16.0.1 - Newtec/DVB-S2");
        
        BeaconPrintf(CALLBACK_OUTPUT, "\n[*] Common SCADA/ICS ports to scan:");
        BeaconPrintf(CALLBACK_OUTPUT, "  502 - Modbus TCP");
        BeaconPrintf(CALLBACK_OUTPUT, "  102 - S7comm (Siemens)");
        BeaconPrintf(CALLBACK_OUTPUT, "  44818 - EtherNet/IP");
        BeaconPrintf(CALLBACK_OUTPUT, "  47808 - BACnet");
        BeaconPrintf(CALLBACK_OUTPUT, "  20000 - DNP3");
        BeaconPrintf(CALLBACK_OUTPUT, "  4840 - OPC-UA");
        BeaconPrintf(CALLBACK_OUTPUT, "  1089-1091 - FF HSE");
        BeaconPrintf(CALLBACK_OUTPUT, "  2222 - EtherCAT");
    }
    else if (mode == 2) {
        // ICS/SCADA specific
        BeaconPrintf(CALLBACK_OUTPUT, "=== ICS/SCADA Protocols ===");
        BeaconPrintf(CALLBACK_OUTPUT, "[*] Modbus TCP (port 502):");
        BeaconPrintf(CALLBACK_OUTPUT, "  FC01 - Read Coils");
        BeaconPrintf(CALLBACK_OUTPUT, "  FC03 - Read Holding Registers");
        BeaconPrintf(CALLBACK_OUTPUT, "  FC05 - Write Single Coil");
        BeaconPrintf(CALLBACK_OUTPUT, "  FC06 - Write Single Register");
        BeaconPrintf(CALLBACK_OUTPUT, "  FC15 - Write Multiple Coils");
        BeaconPrintf(CALLBACK_OUTPUT, "  FC16 - Write Multiple Registers");
        
        BeaconPrintf(CALLBACK_OUTPUT, "\n[*] Siemens S7 (port 102):");
        BeaconPrintf(CALLBACK_OUTPUT, "  PLC Start/Stop commands");
        BeaconPrintf(CALLBACK_OUTPUT, "  Variable read/write");
        BeaconPrintf(CALLBACK_OUTPUT, "  Program upload/download");
        
        BeaconPrintf(CALLBACK_OUTPUT, "\n[*] DNP3 (port 20000):");
        BeaconPrintf(CALLBACK_OUTPUT, "  Function codes 1-20");
        BeaconPrintf(CALLBACK_OUTPUT, "  Direct operate / Select before operate");
    }
    else if (mode == 3) {
        // Satellite ground station
        BeaconPrintf(CALLBACK_OUTPUT, "=== Satellite Ground Station ===");
        BeaconPrintf(CALLBACK_OUTPUT, "[*] Common ground station management ports:");
        BeaconPrintf(CALLBACK_OUTPUT, "  161 - SNMP (network management)");
        BeaconPrintf(CALLBACK_OUTPUT, "  23 - Telnet (legacy equipment)");
        BeaconPrintf(CALLBACK_OUTPUT, "  22 - SSH (modern equipment)");
        BeaconPrintf(CALLBACK_OUTPUT, "  80/443 - Web management");
        BeaconPrintf(CALLBACK_OUTPUT, "  502 - Modbus (antenna controllers)");
        BeaconPrintf(CALLBACK_OUTPUT, "  102 - S7comm (PLCs)");
        
        BeaconPrintf(CALLBACK_OUTPUT, "\n[*] Common antenna controller vendors:");
        BeaconPrintf(CALLBACK_OUTPUT, "  - iDirect (modems, BUCs)");
        BeaconPrintf(CALLBACK_OUTPUT, "  - Newtec/Miteq (modems)");
        BeaconPrintf(CALLBACK_OUTPUT, "  - Comtech (ODUs, modems)");
        BeaconPrintf(CALLBACK_OUTPUT, "  - General Dynamics (modems)");
        BeaconPrintf(CALLBACK_OUTPUT, "  - Vibro-Acoustics (antenna control)");
        BeaconPrintf(CALLBACK_OUTPUT, "  - sep nc (NMS software)");
        
        BeaconPrintf(CALLBACK_OUTPUT, "\n[*] VSAT network topology:");
        BeaconPrintf(CALLBACK_OUTPUT, "  Hub station <-> Satellite <-> Remote terminals");
        BeaconPrintf(CALLBACK_OUTPUT, "  Management typically via SNMP/Telnet/SSH");
    }
    
    WINHTTP$WinHttpCloseHandle(hSession);
}
