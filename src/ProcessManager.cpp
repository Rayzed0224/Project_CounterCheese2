#include "../include/ProcessManager.h"
#include <TlHelp32.h>
#include <iostream>

ProcessManager ProcessMgr;

HANDLE hProcess = nullptr; // Global process handle

StatusCode ProcessManager::Attach(const std::wstring& processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        std::wcerr << L"[ERROR] Failed to create process snapshot." << std::endl;
        return StatusCode::PROCESS_NOT_FOUND;
    }

    PROCESSENTRY32W processEntry{};
    processEntry.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(hSnapshot, &processEntry)) {
        do {
            if (processName == processEntry.szExeFile) {
                processID = processEntry.th32ProcessID;
                hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);

                if (hProcess != nullptr) {
                    CloseHandle(hSnapshot);
                    std::wcout << L"[INFO] Attached to process: " << processName
                        << L" (PID: " << processID << L")" << std::endl;
                    return StatusCode::SUCCEED;
                }
            }
        } while (Process32NextW(hSnapshot, &processEntry));
    }

    CloseHandle(hSnapshot);
    std::wcerr << L"[ERROR] Failed to find process: " << processName << std::endl;
    return StatusCode::PROCESS_NOT_FOUND;
}

DWORD64 ProcessManager::GetProcessModuleHandle(const std::wstring& moduleName) {
    if (processID == 0 || hProcess == nullptr) {
        std::wcerr << L"[ERROR] No attached process. Call Attach() first." << std::endl;
        return 0;
    }

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processID);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        std::wcerr << L"[ERROR] Failed to create module snapshot." << std::endl;
        return 0;
    }

    MODULEENTRY32W moduleEntry{};
    moduleEntry.dwSize = sizeof(MODULEENTRY32W);

    if (Module32FirstW(hSnapshot, &moduleEntry)) {
        do {
            if (moduleName == moduleEntry.szModule) {
                CloseHandle(hSnapshot);
                std::wcout << L"[INFO] Found module: " << moduleName
                    << L" at 0x" << std::hex << reinterpret_cast<DWORD64>(moduleEntry.modBaseAddr) << std::endl;
                return reinterpret_cast<DWORD64>(moduleEntry.modBaseAddr);
            }
        } while (Module32NextW(hSnapshot, &moduleEntry));
    }

    CloseHandle(hSnapshot);
    std::wcerr << L"[ERROR] Failed to find module: " << moduleName << std::endl;
    return 0;
}
