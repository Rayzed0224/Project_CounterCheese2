#include "CGame.hpp"
#include "Config.hpp"
#include <string>
#include <iostream>
#include <windows.h>

void CGame::init(const char* processName, const char* moduleName) {
    std::string processNameStr = processName; // Convert to std::string
    std::string moduleNameStr = moduleName;   // Convert to std::string
    

    DWORD processID = FindProcessID(processNameStr);
    if (processID == 0) {
        std::cerr << "Failed to find the process: " << processName << std::endl;
        return;
    }

    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    if (!hProcess) {
        std::cerr << "Failed to open process!" << std::endl;
        return;
    }

    baseClient = GetModuleBaseAddress(processID, moduleNameStr);
    if (baseClient == 0) {
        std::cerr << "Failed to find module: " << moduleName << std::endl;
        return;
    }

    std::cout << "Successfully initialized CGame with client base: " << std::hex << baseClient << std::endl;
}

void CGame::loop() {
    static bool isRunning = false;

    if (isRunning) {
        std::cerr << "Game loop already running!" << std::endl;
        return;
    }

    isRunning = true;
    std::cout << "Entering game loop..." << std::endl;

    if (!hProcess || baseClient == 0) {
        std::cerr << "Invalid process or module base. Cannot execute loop." << std::endl;
        isRunning = false;
        return;
    }

    int retryCount = 0;
    const int maxRetries = 10;

    while (true) {
        
        std::cout << "Game loop running..." << std::endl;
        // Read the view matrix
        if (!ReadProcessMemory(hProcess, (LPCVOID)(baseClient + DW_VIEW_MATRIX), &viewMatrix, sizeof(viewMatrix), nullptr)) {
            std::cerr << "Failed to read view matrix. Retrying..." << std::endl;
            Sleep(100); // Pause before retry
            continue;
        }

        // Update player data
        updatePlayers();

        // Render players
        for (auto& player : players) {
            player.render(viewMatrix);
        }

        // Exit on user command
        if (GetAsyncKeyState(VK_END)) {
            std::cout << "Exiting game loop on user command." << std::endl;
            break;
        }

        Sleep(16); // Maintain ~60 FPS
    }

    isRunning = false;
    std::cout << "Game loop exited." << std::endl;
}

void CGame::close() {
    if (hProcess) {
        CloseHandle(hProcess);
        hProcess = nullptr;
        std::cout << "Process handle closed." << std::endl;
    }
}

void CGame::updatePlayers() {
    players.clear(); // Clear previous frame's data

    for (int i = 0; i < 64; ++i) {
        CPlayer player;
        if (player.update(hProcess, baseClient, i)) {
            players.push_back(player); // Store valid players
        }
    }
}