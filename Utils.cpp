#include "Utils.hpp"

DWORD FindProcessID(const std::string& processName) {
    PROCESSENTRY32 processEntry;
    processEntry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to create process snapshot!" << std::endl;
        return 0;
    }

    if (Process32First(snapshot, &processEntry)) {
        do {
            char cStr[MAX_PATH];
            size_t convertedChars = 0;

            // Convert wide char to multibyte and ensure null termination
            wcstombs_s(&convertedChars, cStr, processEntry.szExeFile, MAX_PATH);
            cStr[MAX_PATH - 1] = '\0'; // Ensure null termination

            if (_stricmp(cStr, processName.c_str()) == 0) {
                CloseHandle(snapshot);
                return processEntry.th32ProcessID;
            }
        } while (Process32Next(snapshot, &processEntry));
    }

    CloseHandle(snapshot);
    std::cerr << "Process not found: " << processName << std::endl;
    return 0;
}

uintptr_t GetModuleBaseAddress(DWORD processID, const std::string& moduleName) {
    MODULEENTRY32 moduleEntry;
    moduleEntry.dwSize = sizeof(MODULEENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processID);
    if (snapshot == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to create module snapshot!" << std::endl;
        return 0;
    }

    if (Module32First(snapshot, &moduleEntry)) {
        do {
            char cStr[MAX_PATH];
            size_t convertedChars = 0;

            // Safely convert wide-character string to multibyte string
            wcstombs_s(&convertedChars, cStr, moduleEntry.szModule, MAX_PATH);
            cStr[MAX_PATH - 1] = '\0'; // Ensure null termination

            if (_stricmp(cStr, moduleName.c_str()) == 0) {
                CloseHandle(snapshot);
                return (uintptr_t)moduleEntry.modBaseAddr;
            }
        } while (Module32Next(snapshot, &moduleEntry));
    }

    CloseHandle(snapshot);
    std::cerr << "Module not found: " << moduleName << std::endl;
    return 0;
}

template <typename T>
std::optional<T> ReadMemory(HANDLE hProcess, uintptr_t address) {
    T buffer;
    SIZE_T bytesRead;
    if (ReadProcessMemory(hProcess, (LPCVOID)address, &buffer, sizeof(T), &bytesRead) && bytesRead == sizeof(T)) {
        return buffer;
    }
    else {
        std::cerr << "Failed to read memory at address: " << std::hex << address << std::endl;
        return std::nullopt;
    }
}
