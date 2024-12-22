#ifndef UTILS_HPP
#define UTILS_HPP

#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include <optional>

struct Vector3 {
    float x, y, z;
};

struct ViewMatrix {
    float matrix[4][4];
};

DWORD FindProcessID(const std::string& processName);
uintptr_t GetModuleBaseAddress(DWORD processID, const std::string& moduleName);
template <typename T>
std::optional<T> ReadMemory(HANDLE hProcess, uintptr_t address);

#endif