#pragma once

#include <Windows.h>
#include <iostream>

class MemoryManager {
public:
    template <typename T>
    static T Read(DWORD64 address);
};

// Template implementation
template <typename T>
T MemoryManager::Read(DWORD64 address) {
    T value{};
    SIZE_T bytesRead = 0;

    // Replace this with your process handle
    extern HANDLE hProcess; // Ensure the process handle is declared and initialized elsewhere

    if (ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(address), &value, sizeof(T), &bytesRead)) {
        return value; // Return the value if the memory read succeeds
    }

    std::wcerr << L"[ERROR] Failed to read memory at address: 0x" << std::hex << address << std::endl;
    return T{}; // Return a default-constructed value if ReadProcessMemory fails
}
