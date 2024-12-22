#include "../include/MemoryManager.h"

template<typename T>
T MemoryManager::Read(DWORD64 address) {
    T buffer{};
    SIZE_T bytesRead;
    if (!ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<LPCVOID>(address), &buffer, sizeof(T), &bytesRead)) {
        std::wcerr << L"[ERROR] Failed to read memory at address: " << std::hex << address << std::endl;
    }
    return buffer;
}
