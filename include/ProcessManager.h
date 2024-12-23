#pragma once

#include <Windows.h>
#include <string>

// Status codes for process manager operations
enum class StatusCode {
    SUCCEED = 0,
    PROCESS_NOT_FOUND,
    MODULE_NOT_FOUND
};

class ProcessManager {
public:
    // Attach to a process by its name
    StatusCode Attach(const std::wstring& processName);

    // Get the base address of a module in the attached process
    DWORD64 GetProcessModuleHandle(const std::wstring& moduleName);

    // Getter for hProcess
    HANDLE GetProcessHandle() const { return hProcess; }

    // Getter for process ID
    DWORD GetProcessID() const { return processID; }

private:
    HANDLE hProcess = nullptr;  // Handle to the attached process
    DWORD processID = 0;        // ID of the attached process
};

extern ProcessManager ProcessMgr; // Global instance of ProcessManager
