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
    SIZE_T bytesRead;

    extern HANDLE hProcess; // Declare process handle externally

    // Validate address before attempting to read
    if (address == 0) {
        std::wcerr << L"[ERROR] Attempted to read a null address!" << std::endl;
        return T{};
    }

    // Ensure hProcess is valid
    if (hProcess == nullptr) {
        std::wcerr << L"[ERROR] hProcess is null. Ensure the process is attached." << std::endl;
        return T{};
    }

    // Read memory safely
    if (!ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(address), &value, sizeof(T), &bytesRead)) {
        std::wcerr << L"[ERROR] Failed to read memory at address: 0x" << std::hex << address << std::endl;
        return T{};
    }

    return value;
}
