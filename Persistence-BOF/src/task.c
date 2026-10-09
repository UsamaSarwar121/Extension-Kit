#include <windows.h>
#include "beacon.h"

DECLSPEC_IMPORT HRESULT WINAPI COMBASE$CoInitializeEx(LPVOID, DWORD);
DECLSPEC_IMPORT HRESULT WINAPI COMBASE$CoCreateInstance(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*);
DECLSPEC_IMPORT void WINAPI COMBASE$CoUninitialize();
DECLSPEC_IMPORT BSTR WINAPI OLEAUT32$SysAllocString(LPCOLESTR);
DECLSPEC_IMPORT void WINAPI OLEAUT32$SysFreeString(BSTR);

typedef long HRESULT;
typedef wchar_t* BSTR;

#define CLSID_TaskScheduler "0F87369F-A4E5-4CFC-BD3E-EBD1215E67D4"
#define IID_ITaskService  "2FABA4C7-4176-4194-AC9E-7ED3D1DE2752"

void go(char *args, int len) {
    datap parser;
    BeaconDataParse(&parser, args, len);
    
    int mode = BeaconDataInt(&parser);
    char *taskName = BeaconDataExtract(&parser, NULL);
    char *taskPath = BeaconDataExtract(&parser, NULL);
    
    if (!taskName) taskName = "AdaptixTask";
    
    HRESULT hr = COMBASE$CoInitializeEx(NULL, 0);
    if (FAILED(hr)) {
        BeaconPrintf(CALLBACK_ERROR, "[-] CoInitializeEx failed: 0x%08x", hr);
        return;
    }
    
    if (mode == 0) {
        BeaconPrintf(CALLBACK_OUTPUT, "[+] Scheduled task would be created: %s", taskName);
        BeaconPrintf(CALLBACK_OUTPUT, "[*] Use 'schtasks /create /tn \"%s\" /tr \"%s\" /sc hourly' on target", taskName, taskPath ? taskPath : "cmd.exe");
    }
    else if (mode == 1) {
        BeaconPrintf(CALLBACK_OUTPUT, "[+] Scheduled task would be deleted: %s", taskName);
        BeaconPrintf(CALLBACK_OUTPUT, "[*] Use 'schtasks /delete /tn \"%s\" /f' on target", taskName);
    }
    
    COMBASE$CoUninitialize();
}
