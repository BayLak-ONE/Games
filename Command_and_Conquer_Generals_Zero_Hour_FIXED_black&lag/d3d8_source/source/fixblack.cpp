#include <windows.h>
#include <vector>
#include <thread>

class MemoryTool {
public:
    HANDLE hProcess;

    MemoryTool() {
        hProcess = GetCurrentProcess(); 
    }

    template<typename T>
    bool Read(uintptr_t addr, T* out) {
        SIZE_T bytesRead;
        return ReadProcessMemory(hProcess, (LPCVOID)addr, out, sizeof(T), &bytesRead) && bytesRead == sizeof(T);
    }

    template<typename T>
    bool Write(uintptr_t addr, T value) {
        SIZE_T bytesWritten;
        return WriteProcessMemory(hProcess, (LPVOID)addr, &value, sizeof(T), &bytesWritten) && bytesWritten == sizeof(T);
    }

    uintptr_t ResolvePointer(uintptr_t base, const std::vector<int>& offsets) {
        uintptr_t addr = base;
        for (int offset : offsets) {
            if (!Read(addr, &addr)) return 0;
            addr += offset;
        }
        return addr;
    }
};

MemoryTool mem;

void MainThread() {
    uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL) + 0x647F4C;

    std::vector<std::vector<int>> offsetsList = {
        {0x28, 0x18, 0x4, 0x30, 0x34, 0x34, 0x3E0},
        {0x28, 0x18, 0x1C, 0x18, 0x18, 0x4, 0x6B4},
        {0x8, 0x500, 0x264, 0x17C, 0x30, 0x18, 0xAC0},
        {0x14, 0x18, 0x18, 0x18, 0x18, 0x1C, 0x438},
        {0x2C, 0x1C, 0x34, 0x18, 0x34, 0x34, 0x3DC}
    };

    std::vector<unsigned long> targetValues = {
        1061997773,
        1080344404,
        3197059054,
        1049575406,
        1070945621
    };

    std::vector<unsigned long> newValues = {
        1065353216,
        1077346168,
        3199406056,
        1051922408,
        1068149419
    };

    while (true) {
        for (size_t i = 0; i < offsetsList.size(); ++i) {
            uintptr_t addr = mem.ResolvePointer(baseAddress, offsetsList[i]);
            if (addr == 0) continue;

            unsigned long currentValue = 0;
            if (mem.Read(addr, &currentValue)) {
                if (currentValue == targetValues[i]) {
                    mem.Write(addr, newValues[i]);
                }
            }
        }
        Sleep(1);
    }
}

