#pragma once

#include <iostream>
#include <Windows.h>
#include "Offsets.h" // Include centralized offsets
#include "MemoryManager.h"

class CGame {
public:

    CGame() : isRunning(true), isESPEnabled(true) {}
    void ProcessInput();

    void Run(MemoryManager& memMgr, HWND overlayWindow);;
    void SetESPState(bool state);
    bool IsRunning() const { return isRunning; }
    void Stop() { isRunning = false; }

    bool InitAddress(MemoryManager& memMgr); // Initialize game memory addresses
    bool Initialize();      // General initialization
    bool ProcessInput();    // Handle user input
    void DrawESP(MemoryManager& memMgr); // Render ESP
    void Update(MemoryManager& memMgr);          // Update game state
    void Render();          // Render visuals (if needed)
    void Cleanup();         // Clean up resources
    void SetESPState(bool state);

    struct Addresses {
        DWORD64 ClientDLL = 0;       // Base address of client.dll
        DWORD64 EntityList = 0;     // Address of entity list
        DWORD64 LocalPlayer = 0;    // Address of local player
        DWORD64 ViewMatrix = 0;
    };

    Addresses Address;

private:
    bool isESPEnabled;       // Is ESP enabled
    bool isRunning;
};


