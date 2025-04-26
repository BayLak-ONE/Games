#include <windows.h>
#include <tlhelp32.h>  
#include <iostream>
#include <vector>
#include <thread>
#include <string>

using namespace std;

const wstring targetProcessName = L"generals.exe"; 
const int baseOffset = 0x0062B5FC;  
vector<int> offsets = { 0x1C }; 
int valueToFreeze = 600;  

bool ReadMemory(HANDLE hProcess, LPCVOID address, void* buffer, SIZE_T size) {
    SIZE_T bytesRead;
    return ReadProcessMemory(hProcess, address, buffer, size, &bytesRead) && bytesRead == size;
}

bool WriteMemory(HANDLE hProcess, LPVOID address, const void* buffer, SIZE_T size) {
    SIZE_T bytesWritten;
    return WriteProcessMemory(hProcess, address, buffer, size, &bytesWritten) && bytesWritten == size;
}

uintptr_t GetPointerAddress(HANDLE hProcess, uintptr_t baseAddress, const vector<int>& offsets) {
    uintptr_t address = baseAddress;
    for (size_t i = 0; i < offsets.size(); i++) {
        if (!ReadMemory(hProcess, (LPCVOID)address, &address, sizeof(address))) {
            return 0;
        }
        address += offsets[i];
    }
    return address;
}

void Main2Thread() {
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) {
        cout << "Failed to get process snapshot." << endl;
        return;
    }

    if (Process32First(hSnap, &pe32)) {
        do {
            if (wcscmp(pe32.szExeFile, targetProcessName.c_str()) == 0) {
                DWORD processId = pe32.th32ProcessID;
                HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
                if (hProcess == NULL) {
                    cout << "Failed to open process." << endl;
                    continue;
                }

                uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL) + baseOffset;

                while (true) {
                    bool foundTarget = false;
                    uintptr_t address = GetPointerAddress(hProcess, baseAddress, offsets);

                    if (address == 0) {
                        continue;
                    }

                    int currentValue = 0;
                    if (ReadMemory(hProcess, (LPCVOID)address, &currentValue, sizeof(currentValue))) {
                        if (currentValue == 480) {
                            cout << "Detected 480 -> Changing to 600" << endl;
                            WriteMemory(hProcess, (LPVOID)address, &valueToFreeze, sizeof(valueToFreeze));
                            foundTarget = true;
                        }
                    }

                    if (!foundTarget) {
                        Sleep(1);
                    }

                    Sleep(1);
                }
            }
        } while (Process32Next(hSnap, &pe32));
    }

    CloseHandle(hSnap);
}

