#include "../include/MemoryManager.h"

bool MemoryManager::Attach(DWORD processID) {
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    if (!hProcess) {
        std::cerr << "[ERROR] Failed to attach to process ID: " << processID << std::endl;
        return false;
    }
    return true;
}

void MemoryManager::Detach() {
    if (hProcess) {
        CloseHandle(hProcess);
        hProcess = nullptr;
    }
}

bool MemoryManager::Read(DWORD64 address, void* buffer, SIZE_T size) {
    if (!hProcess || !buffer || address == 0) {
        std::cerr << "[ERROR] Invalid parameters for memory read. Address: 0x" << std::hex << address << std::endl;
        return false;
    }

    SIZE_T bytesRead;
    if (!ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(address), buffer, size, &bytesRead)) {
        std::cerr << "[ERROR] Failed to read memory at address: 0x" << std::hex << address << std::endl;
        return false;
    }

    return true;
}

bool MemoryManager::Write(DWORD64 address, const void* buffer, SIZE_T size) {
    if (!hProcess || !buffer || address == 0) {
        std::cerr << "[ERROR] Invalid parameters for memory write." << std::endl;
        return false;
    }

    SIZE_T bytesWritten;
    if (!WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(address), buffer, size, &bytesWritten)) {
        std::cerr << "[ERROR] Failed to write memory at address: 0x" << std::hex << address << std::endl;
        return false;
    }

    return true;
}
