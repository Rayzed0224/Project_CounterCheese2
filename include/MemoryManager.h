#pragma once

#include <Windows.h>
#include <iostream>

class MemoryManager {
public:
    template <typename T>
    static T Read(DWORD64 address);
};

// Template implementation (must stay in the header)
template <typename T>
T MemoryManager::Read(DWORD64 address) {
    T value{};
    SIZE_T bytesRead = 0;

    extern HANDLE hProcess; // Declare process handle externally

    if (ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(address), &value, sizeof(T), &bytesRead)) {
        return value;
    }

    std::cerr << "[ERROR] Failed to read memory at address: 0x" << std::hex << address << std::endl;
    return T{};
}