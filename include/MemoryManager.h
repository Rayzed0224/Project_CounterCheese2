#pragma once

#include <Windows.h>
#include <iostream>

class MemoryManager {
public:
    // Attach to the target process
    bool Attach(DWORD processID);

    // Detach from the target process
    void Detach();

    // Read a single value from memory
    bool Read(DWORD64 address, void* buffer, SIZE_T size);

    // Write a single value to memory
    bool Write(DWORD64 address, const void* buffer, SIZE_T size);

    // Check if the process is attached
    bool IsAttached() const { return hProcess != nullptr; }

private:
    HANDLE hProcess = nullptr; // Handle to the target process
};
