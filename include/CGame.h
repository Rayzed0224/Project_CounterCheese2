#pragma once

#include <iostream>
#include <Windows.h>

class CGame {
public:
    void Run();             // Main game loop
    bool InitAddress();     // Initialize game memory addresses
    bool Initialize();      // General initialization
    bool ProcessInput();    // Handle user input
    void DrawESP();         // Render ESP
    void Update();          // Update game state
    void Render();          // Render visuals (if needed)
    void Cleanup();         // Clean up resources

    struct Addresses {
        DWORD64 ClientDLL = 0;       // Base address of client.dll
        DWORD64 EntityList = 0;     // Address of entity list
        DWORD64 LocalPlayer = 0;    // Address of local player
    };

    bool isRunning = true;          // Is the game loop running
    bool isESPEnabled = true;       // Is ESP enabled

private:
    
    Addresses Address;              // Instance of Addresses struct
    
};

