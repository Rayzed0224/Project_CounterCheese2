#pragma once

#include <iostream>
#include <Windows.h>
#include "Offsets.h" // Include centralized offsets
#include "MemoryManager.h"

class CGame {
public:
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

    bool isRunning = true;          // Is the game loop running
    

    void Run(MemoryManager& memMgr); // Main game loop

    Addresses Address;

private:
    bool isESPEnabled = true;       // Is ESP enabled
};


