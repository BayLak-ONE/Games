#include <windows.h>
#include <thread>
#include "Cp1.h" 

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        std::thread(MainThread).detach();
        std::thread(Main2Thread).detach();
    }
    return TRUE;
}
