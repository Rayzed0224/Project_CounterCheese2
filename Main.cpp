#include <iostream>
#include "include/CGame.h"        // Ensure correct relative path
#include "include/ProcessManager.h"
#include "include/ImGuiManager.h"

int main() {
    // Create the game instance
    CGame game;

    // Step 1: Attach to the game process
    auto processStatus = ProcessMgr.Attach(L"cs2.exe"); // Attach to the CS2 process
    if (processStatus != StatusCode::SUCCEED) {
        std::wcerr << L"[ERROR] Failed to attach to the process 'cs2.exe'. StatusCode: "
            << static_cast<int>(processStatus) << std::endl;
        return -1;
    }
    std::wcout << L"[INFO] Successfully attached to 'cs2.exe' process." << std::endl;

    // Step 2: Initialize addresses and memory
    if (!game.InitAddress()) {
        std::wcerr << L"[ERROR] Failed to initialize game addresses." << std::endl;
        return -1;
    }

    // Step 3: Initialize ImGui for rendering UI
    if (!ImGuiManager::Initialize()) {
        std::wcerr << L"[ERROR] Failed to initialize ImGui." << std::endl;
        return -1;
    }

    // Step 4: Run the main game loop
    game.Run();

    // Step 5: Cleanup resources
    ImGuiManager::Cleanup();

    std::wcout << L"[INFO] Exiting application." << std::endl;
    return 0;
}